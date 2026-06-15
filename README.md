# 🖥️ Tiny Operating System (Tiny-OS)

A minimal hobby operating system built **from scratch** — including a custom bootloader, kernel, basic task scheduling, and hardware interfacing — designed to run on a **Raspberry Pi Pico** or similar microcontroller.

---

## 🎯 Project Goal

Problem: To gain a deep understanding of low-level computer architecture and operating system fundamentals by building a functional core OS.

Solution: Developed a simple hobby operating system from scratch, including a custom bootloader, a basic kernel, and task scheduling. The project also aims to run this system on a minimal hardware platform, such as a microcontroller.

Result: A working, minimal operating system demonstrating core OS concepts and providing practical experience in hardware-level programming, memory management, and process control.

---

## 🚀 Core Concepts

* **Custom Bootloader** – Initializes hardware and loads the kernel into memory.
* **Minimal Kernel** – Provides task scheduling, memory management, and hardware abstraction.
* **Multitasking Scheduler** – Supports cooperative or preemptive task switching.
* **Hardware Interaction** – Low-level drivers for peripherals (UART, SPI, I²C, GPIO, PWM, etc.).
* **Microcontroller Target** – Built for ARM Cortex-M0+ (Raspberry Pi Pico).

---

## 🛠 Tech Stack

| Component           | Details                    |
| ------------------- | -------------------------- |
| **Language**        | C, Assembly                |
| **Build System**    | CMake                      |
| **Version Control** | Git + GitHub               |
| **Target MCU**      | Raspberry Pi Pico (RP2040) |
| **Toolchain**       | ARM GCC (arm-none-eabi)    |

---

## 📦 Features & Progress

* [ ] **Bootloader** – Minimal bare-metal boot sequence
* [ ] **Kernel** – Startup code, hardware abstraction
* [ ] **Memory Management** – Simple allocator or static allocation
* [ ] **Scheduler** – Round-robin or preemptive task scheduling
* [ ] **IPC** – Basic inter-process communication (mailbox, pipes)
* [ ] **I/O Support** – UART console, GPIO, LED, SPI, I²C
* [ ] **RTC Support** – Real-time clock driver
* [ ] **Microcontroller Port** – Optimized for Raspberry Pi Pico

---

## 📂 Repository Structure

```
Tiny-Operating-System/
├── boot.S            # Bootloader assembly code
├── kernel.c          # Core kernel logic
├── memory.c/h        # Memory allocation and management
├── scheduler.c/h     # Task scheduling logic
├── uart.c/h          # UART driver
├── spi.c/h           # SPI driver
├── i2c.c/h           # I²C driver
├── rtc.c/h           # RTC driver
├── fs.c/h            # Basic file system logic
├── shell.c/h         # Simple command-line shell
├── CMakeLists.txt    # Build configuration
└── pico-sdk/         # Pico SDK dependency
```

---

## ⚙️ Building & Running

### 🧪 Run it in QEMU (no hardware needed)

The easiest way to see Tiny-OS actually boot and multitask is the emulated
`lm3s6965evb` (Cortex-M3) target. Install `arm-none-eabi-gcc`, `cmake`, and
`qemu-system-arm`, then:

```bash
./run.sh
```

This builds the QEMU image and boots it, printing to the serial console. You
should see the bump allocator hand out RAM and four tasks (heartbeat, producer,
consumer, IPC sink) being preemptively time-sliced by the round-robin
scheduler, with the mailbox and IPC queue carrying values between them:

```
=== Tiny-OS booting on QEMU (lm3s6965evb / Cortex-M3) ===
[INFO] kernel initialised
[mem] simple_malloc(64)  -> 0x20004008
=== starting scheduler with 4 tasks ===
[heartbeat] tick 0
[producer] sent  value 0
[consumer] recv  value 0
[sink]     logged item 0
...
```

Exit QEMU with `Ctrl-A` then `X`. Under the hood this is
`cmake -B build-qemu -DQEMU=ON -DCMAKE_TOOLCHAIN_FILE=arm-toolchain.cmake`
followed by `qemu-system-arm -M lm3s6965evb -nographic -kernel build-qemu/kernel.elf`.

### 🔌 Build for a real Raspberry Pi Pico

The default build (`QEMU=OFF`) targets the RP2040 and produces a flashable
`kernel.uf2`. It embeds a proper RP2040 second-stage bootloader (`boot2.S`,
the universal "03h" variant) at the start of flash — padded to 256 bytes with
the CRC32 the boot ROM checks — so the chip actually boots into the kernel.

```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=arm-toolchain.cmake
cmake --build build
# Hold BOOTSEL, plug in the Pico, then:
cp build/kernel.uf2 /Volumes/RPI-RP2/
```

What you should see: the **onboard LED (GPIO25) blinking**. That confirms the
whole chain works on real silicon — boot2 → vector table → `kernel_main` → the
preemptive round-robin scheduler context-switching between two tasks. Wire an
LED to GPIO16 to watch the second task blink at its own independent rate.

> **Tested in QEMU, not on physical hardware** (none was available while this
> was written). The image layout and boot2 checksum are verified correct, and
> the scheduler/context-switch logic is the same algorithm proven under QEMU,
> ported to Cortex-M0+. UART serial output on hardware is intentionally left out
> of this demo because it needs PLL/`clk_peri` clock configuration; the LED demo
> avoids that dependency. If you have a Pico to test on, I can add clock + UART
> setup next.

If you edit `boot2.S`, regenerate the checksummed blob with
`./tools/build_boot2.sh`.

### 1️⃣ Install Dependencies

* [ARM GCC Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
* [CMake](https://cmake.org/download/)
* [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)

### 2️⃣ Clone Repository

```bash
git clone https://github.com/yourusername/Tiny-Operating-System.git
cd Tiny-Operating-System
```

### 3️⃣ Build

```bash
mkdir build && cd build
cmake ..
make
```

### 4️⃣ Flash to Pico

```bash
# Connect your Raspberry Pi Pico in bootloader mode
cp TinyOS.uf2 /Volumes/RPI-RP2/
```

---

## 🎓 Learning Objectives

* Understand the **boot process** on ARM Cortex-M microcontrollers.
* Learn how to **write a minimal kernel**.
* Implement **basic task scheduling** and context switching.
* Develop **hardware drivers** for embedded peripherals.
* Explore **memory management** in a constrained environment.

---

## 📜 License

This project is licensed under the **MIT License** — see [LICENSE](LICENSE) for details.
