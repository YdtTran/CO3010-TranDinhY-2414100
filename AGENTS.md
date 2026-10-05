# CO3010 - Microprocessors Lab (HCMUT)

This repository contains firmware projects for the **CO3010 (Microprocessors)** course at **Ho Chi Minh City University of Technology (HCMUT)**.

## Environment & Hardware

- **Target MCU**: STM32F103xB (ARM Cortex-M3, 64 KB Flash, 20 KB SRAM)
- **Toolchain**: `arm-none-eabi-gcc`, CMake (>= 3.22), Ninja
- **Framework**: STM32Cube HAL (`STM32F1xx_HAL_Driver`)
- **Simulation**: Proteus (`protues/*.pdsprj`)

---

## Build Commands

- **Configure CMake (Debug preset)**:
  ```powershell
  cmake --preset Debug
  ```
- **Build Firmware**:
  ```powershell
  cmake --build --preset Debug
  # or directly via Ninja:
  ninja -C build/Debug
  ```
- **Output Artifacts**:
  - `build/Debug/CO3010.elf`
  - `build/Debug/CO3010.hex` (loadable into Proteus STM32F103 microcontroller properties)

---

## Codebase Architecture

- **`Core/Inc/` & `Core/Src/`**:
  - [`main.c`](file:///e:/Workspace/University/HCMUT/CO3010/Core/Src/main.c): Application entry, clock configuration, peripheral setup (`MX_GPIO_Init`, `MX_TIM2_Init`), and main loop / timer interrupt callbacks (`HAL_TIM_PeriodElapsedCallback`).
  - [`traffic_light.h`](file:///e:/Workspace/University/HCMUT/CO3010/Core/Inc/traffic_light.h) / `traffic_light.c`: Traffic light state machine, timing counters, and LED pin bindings.
  - [`seg7.h`](file:///e:/Workspace/University/HCMUT/CO3010/Core/Inc/seg7.h) / [`seg7.c`](file:///e:/Workspace/University/HCMUT/CO3010/Core/Src/seg7.c): Low-level 7-segment digit decoder (common cathode / common anode), segment pin mapping (`gpio[8]`).
  - [`seg7_display.h`](file:///e:/Workspace/University/HCMUT/CO3010/Core/Inc/seg7_display.h) / [`seg7_display.c`](file:///e:/Workspace/University/HCMUT/CO3010/Core/Src/seg7_display.c): High-level multi-digit display controller, digit multiplexing/scanning using [`seg7_display_control_t`](file:///e:/Workspace/University/HCMUT/CO3010/Core/Inc/seg7_display.h#L7-L11) and [`seg7_display_t`](file:///e:/Workspace/University/HCMUT/CO3010/Core/Inc/seg7_display.h#L13-L19).
- **`CMakeLists.txt`**: Project definition. When adding any new `.c` source files, ensure they are added to `add_executable(CO3010 ...)`.
- **`cmake/stm32cubemx/`**: Auto-generated HAL library files and toolchain definitions.
- **`protues/`**: Proteus simulation schematic files (`lab1.pdsprj`, `lab2.pdsprj`).

---

## Coding Guidelines & Conventions

1. **User Code Blocks in Generated Files**:
   - In files generated or touched by STM32CubeMX (such as [`Core/Src/main.c`](file:///e:/Workspace/University/HCMUT/CO3010/Core/Src/main.c)), place custom code strictly between `/* USER CODE BEGIN ... */` and `/* USER CODE END ... */` tags to prevent inadvertent overwrites.
2. **Peripheral Struct Patterns**:
   - Follow existing project conventions for GPIO descriptors: use `GPIO_TypeDef *port` and `uint32_t gpio` (or an array of `uint32_t` pin constants).
3. **C Standards**:
   - C11 with GNU extensions (`-std=gnu11`).
4. **Verification**:
   - Always run `ninja -C build/Debug` to ensure all headers, source files, and link steps succeed cleanly before finalizing changes.
