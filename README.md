# 🖥️ Tiny Operating System (Tiny-OS)

A minimal hobby operating system built **from scratch** — including a custom bootloader, kernel, basic task scheduling, and hardware interfacing — designed to run on a **Raspberry Pi Pico** or similar microcontroller.

---

## 🎯 Project Goal

The aim of this project is to **learn and explore low-level systems programming** by building a small but functional OS from the ground up.
It covers **boot process**, **kernel design**, **memory management**, and **multitasking** — all while working directly with microcontroller hardware.

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
