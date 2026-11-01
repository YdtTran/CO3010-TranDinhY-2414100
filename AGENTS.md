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

- **`src/`**: Application and STM32 support sources, with their headers alongside them.
  - [`main.c`](src/main.c): Application entry, clock configuration, peripheral setup (`MX_GPIO_Init`, `MX_TIM2_Init`), and main loop / timer interrupt callbacks (`HAL_TIM_PeriodElapsedCallback`).
  - [`app.c`](src/app.c): Application initialization, clock counting, and scheduling of display scans, matrix animation, and the blinking dot.
  - `stm32f1xx_it.c`, `stm32f1xx_hal_msp.c`, `system_stm32f1xx.c`, `syscalls.c`, and `sysmem.c`: Interrupts, HAL setup, system initialization, and runtime support.
- **`lib/`**: Reusable modules, each containing its own source and header files.
  - [`seg7/`](lib/seg7/): Low-level 7-segment digit decoder (common cathode / common anode) and segment pin mapping (`gpio[8]`).
  - [`seg7_display/`](lib/seg7_display/): Four-digit display pin bindings, clock-to-digit conversion, and multiplex scanning.
  - [`led_matrix/`](lib/led_matrix/): Matrix pin bindings, bitmap, column scanning, and bitmap rotation.
  - [`software_timer/`](lib/software_timer/): Ten software timers driven by the 10 ms TIM2 interrupt; expiration flags are consumed by the application loop.
  - [`traffic_light/`](lib/traffic_light/): Traffic light state and LED pin definitions; `traffic_light.c` currently contains only its header include.
- **`CMakeLists.txt`**: Project definition. Register new library `.c` files in `add_executable(...)` and their header directories in `target_include_directories(...)`.
- **`cmake/stm32cubemx/`**: Auto-generated HAL library files and toolchain definitions.
- **`protues/`**: Proteus simulation schematic files (`lab1.pdsprj`, `lab2.pdsprj`).

CubeMX paths in `CO3010.ioc`, `.mxproject`, and `cmake/stm32cubemx/CMakeLists.txt` use `src/`. After regenerating with CubeMX, check that generated source and include paths still match this layout before building.

---

## Coding Guidelines & Conventions

1. **User Code Blocks in Generated Files**:
   - In files generated or touched by STM32CubeMX (such as [`src/main.c`](src/main.c)), place custom code strictly between `/* USER CODE BEGIN ... */` and `/* USER CODE END ... */` tags to prevent inadvertent overwrites.
2. **Peripheral Struct Patterns**:
   - Follow existing project conventions for GPIO descriptors: use `GPIO_TypeDef *port` and `uint32_t gpio` (or an array of `uint32_t` pin constants).
3. **C Standards**:
   - C11 with GNU extensions (`-std=gnu11`).
4. **Verification**:
   - Always run `ninja -C build/Debug` to ensure all headers, source files, and link steps succeed cleanly before finalizing changes.
