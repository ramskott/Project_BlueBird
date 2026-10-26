# Raspberry Pi Pico (RP2040) TensorFlow Lite Micro Deployment

This folder contains the firmware and build configuration to run real-time sparrow sound classification directly on the **Raspberry Pi Pico**.

---

## Hardware Setup

1. **Microcontroller**: Raspberry Pi Pico or Pico W (RP2040, 264KB SRAM, 2MB Flash).
2. **Microphone**:
   - **Analog Microphone Module**: MAX9814 or MAX4466.
     - `VCC` -> 3.3V (Pin 36 / 3V3)
     - `GND` -> GND (Pin 38)
     - `OUT` -> GPIO 26 (ADC0 / Pin 31)
   - *(Optional)* PDM digital microphone (e.g. MP34DT01).
3. **Indicator**: Onboard LED (GPIO 25) lights up when sparrow sound is detected.

---

## Memory & Model Considerations for Pico

| Hardware Resource | RP2040 Capacity | Micro-Speech Model (INT8) | Teachable Machine (Float32) |
| :--- | :--- | :--- | :--- |
| **Flash Memory** | 2,048 KB (2 MB) | ~20 KB - 40 KB | ~5,774 KB (Exceeds Flash) |
| **SRAM (RAM)** | 264 KB | ~60 KB Tensor Arena | Exceeds RAM |

> **Note**: Standard Teachable Machine exports are float32 models (~5.7 MB) designed for desktop/mobile. For Pico deployment, use an INT8 quantized TFLite Micro model generated using the Micro Speech architecture or quantize using `src/export_for_pico.py`.

---

## Build and Flash Steps

### 1. Prerequisites
- Raspberry Pi Pico C/C++ SDK
- ARM GCC Toolchain (`arm-none-eabi-gcc`)
- CMake >= 3.13
- `pico-tflmicro` library

### 2. Export Model Array
Generate the C++ byte array header and source files:
```bash
python src/export_for_pico.py --model models/your_quantized_model.tflite
```

### 3. Build Firmware
```bash
cd pico
mkdir build && cd build
cmake .. -DPICO_SDK_PATH=/path/to/pico-sdk
make -j4
```

### 4. Flash to Pico
1. Hold down the **BOOTSEL** button on your Pico.
2. Plug the USB cable into your computer.
3. Release BOOTSEL. The Pico appears as a mass storage drive named `RPI-RP2`.
4. Drag and drop `bluebird_pico.uf2` into the `RPI-RP2` drive.
5. Open a serial monitor (e.g., `minicom -b 115200 -o -D /dev/tty.usbmodem*` or VS Code Serial Monitor) to view live sparrow detection logs!
