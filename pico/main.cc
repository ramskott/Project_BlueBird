/*
 * BlueBird - Raspberry Pi Pico (RP2040) Live Sparrow Sound Classifier
 * Runs TensorFlow Lite for Microcontrollers (pico-tflmicro)
 */

#include <cstdio>
#include <cstring>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/timer.h"

// TensorFlow Lite Micro headers
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "model_settings.h"
#include "model_data.h"

// Global Tensor Arena allocated in SRAM
alignas(16) static uint8_t tensor_arena[kTensorArenaSize];

// Forward declarations
void init_hardware();
void capture_audio_adc(int16_t* audio_buffer, int num_samples);

int main() {
    // 1. Initialize Pico stdio and hardware
    tflite::InitializeTarget();
    stdio_init_all();
    init_hardware();

    printf("\n=========================================\n");
    printf("   BlueBird: Sparrow Sound Classifier\n");
    printf("   Running on Raspberry Pi Pico (RP2040)\n");
    printf("=========================================\n\n");

    // 2. Map model from Flash memory
    const tflite::Model* model = tflite::GetModel(g_model);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf("Error: Model schema version %lu != %d\n", model->version(), TFLITE_SCHEMA_VERSION);
        return 1;
    }
    printf("Model loaded from Flash successfully (%d bytes).\n", g_model_len);

    // 3. Setup Op Resolver
    tflite::AllOpsResolver resolver;

    // 4. Instantiate MicroInterpreter
    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, kTensorArenaSize);

    // Allocate memory from tensor_arena for model's tensors
    TfLiteStatus allocate_status = interpreter.AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        printf("Error: AllocateTensors() failed! Consider increasing kTensorArenaSize.\n");
        return 1;
    }
    printf("Tensors allocated successfully in %d KB SRAM Arena.\n", kTensorArenaSize / 1024);

    TfLiteTensor* input = interpreter.input(0);
    TfLiteTensor* output = interpreter.output(0);

    printf("Input tensor: type %d, size %d bytes\n", input->type, input->bytes);
    printf("Output categories: %d\n\n", kCategoryCount);

    // Audio capture buffer (16-bit PCM)
    static int16_t audio_buffer[kAudioSampleCount];

    printf("Listening to microphone on ADC pin %d...\n", kMicAdcPin);

    // 5. Main Inference Loop
    while (true) {
        // Step A: Sample audio from ADC / Microphone
        capture_audio_adc(audio_buffer, kAudioSampleCount);

        // Step B: Copy and quantize audio into model input tensor
        if (input->type == kTfLiteInt8) {
            int8_t* input_data = input->data.int8;
            for (int i = 0; i < input->bytes; ++i) {
                // Normalize 16-bit PCM audio [-32768..32767] to INT8 [-128..127]
                input_data[i] = static_cast<int8_t>(audio_buffer[i] >> 8);
            }
        } else if (input->type == kTfLiteFloat32) {
            float* input_data = input->data.f;
            for (int i = 0; i < kAudioSampleCount; ++i) {
                input_data[i] = audio_buffer[i] / 32768.0f;
            }
        }

        // Step C: Run inference
        TfLiteStatus invoke_status = interpreter.Invoke();
        if (invoke_status != kTfLiteOk) {
            printf("Inference invoke failed!\n");
            continue;
        }

        // Step D: Parse prediction outputs
        int best_index = 0;
        float best_score = 0.0f;
        float sparrow_score = 0.0f;

        if (output->type == kTfLiteInt8) {
            int8_t* out_data = output->data.int8;
            float scale = output->params.scale;
            int zero_point = output->params.zero_point;

            for (int i = 0; i < kCategoryCount; ++i) {
                float prob = (out_data[i] - zero_point) * scale;
                if (prob > best_score) {
                    best_score = prob;
                    best_index = i;
                }
                if (i == kSparrowSoundsIndex) {
                    sparrow_score = prob;
                }
            }
        } else if (output->type == kTfLiteFloat32) {
            float* out_data = output->data.f;
            for (int i = 0; i < kCategoryCount; ++i) {
                if (out_data[i] > best_score) {
                    best_score = out_data[i];
                    best_index = i;
                }
                if (i == kSparrowSoundsIndex) {
                    sparrow_score = out_data[i];
                }
            }
        }

        // Step E: Trigger Detection Indicator (LED & Serial Alert)
        if (sparrow_score >= kDetectionThreshold) {
            gpio_put(kLedPin, 1);  // Turn ON on-board LED
            printf("[SPARROW DETECTED!] Confidence: %.1f%%  (Class: %s)\n",
                   sparrow_score * 100.0f, kCategoryLabels[kSparrowSoundsIndex]);
        } else {
            gpio_put(kLedPin, 0);  // Turn OFF LED
            if (best_score >= 0.50f) {
                printf("[Status] %s (%.1f%%)\n", kCategoryLabels[best_index], best_score * 100.0f);
            }
        }

        sleep_ms(100);  // 100ms pause before next window
    }

    return 0;
}

void init_hardware() {
    // Initialize onboard LED
    gpio_init(kLedPin);
    gpio_set_dir(kLedPin, GPIO_OUT);
    gpio_put(kLedPin, 0);

    // Initialize ADC for microphone input
    adc_init();
    adc_gpio_init(kMicAdcPin);
    adc_select_input(0);  // ADC0 corresponds to GPIO 26
}

void capture_audio_adc(int16_t* audio_buffer, int num_samples) {
    // Sample microphone using ADC at kAudioSampleFrequency (16 kHz = 62.5 us per sample)
    const uint64_t interval_us = 1000000 / kAudioSampleFrequency;

    for (int i = 0; i < num_samples; ++i) {
        uint64_t next_sample_time = time_us_64() + interval_us;
        
        // Read 12-bit ADC value [0..4095]
        uint16_t raw = adc_read();
        
        // Remove DC bias (center at 2048) and scale to signed 16-bit [-32768..32767]
        int32_t centered = static_cast<int32_t>(raw) - 2048;
        audio_buffer[i] = static_cast<int16_t>(centered << 4);

        // Accurate timing delay
        while (time_us_64() < next_sample_time) {
            tight_loop_contents();
        }
    }
}
