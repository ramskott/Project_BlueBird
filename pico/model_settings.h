#ifndef MODEL_SETTINGS_H_
#define MODEL_SETTINGS_H_

// Audio configuration for Pico microphone
constexpr int kAudioSampleFrequency = 16000;      // 16 kHz audio sampling
constexpr int kAudioDurationMs = 1000;            // 1 second audio window
constexpr int kAudioSampleCount = (kAudioSampleFrequency * kAudioDurationMs) / 1000;

// Category labels
constexpr int kCategoryCount = 4;
constexpr const char* kCategoryLabels[kCategoryCount] = {
    "Background Noise",
    "Human Sparrow",
    "Sparrow Sounds",
    "Rain"
};

// Sparrow detection indices and threshold
constexpr int kSparrowSoundsIndex = 2;
constexpr int kHumanSparrowIndex = 1;
constexpr float kDetectionThreshold = 0.60f;      // 60% confidence threshold

// Tensor Arena size in SRAM (RP2040 has 264 KB RAM)
constexpr int kTensorArenaSize = 60 * 1024;        // 60 KB tensor arena

// Hardware GPIO pin definitions
constexpr int kLedPin = 25;                       // On-board LED on standard Pico (GPIO 25)
constexpr int kMicAdcPin = 26;                    // ADC0 (GPIO 26) for analog microphone module (e.g. MAX9814 / MAX4466)

#endif  // MODEL_SETTINGS_H_
