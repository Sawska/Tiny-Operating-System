# Tiny-Operating-System
a simple hobby operating system from scratch including bootloader, kernel, basic task scheduling, and possibly run it on a microcontroller


# Hobby Tiny OS Project

**Goal:**
Create a simple hobby operating system from scratch including bootloader, kernel, basic task scheduling, and possibly run it on a microcontroller.

---

### 🚀 Concept

Build a minimal yet functional tiny OS that covers:

* A custom bootloader to initialize hardware and load the kernel
* A kernel with basic functionality: memory management, simple I/O, and task scheduling
* A basic multitasking scheduler supporting cooperative or preemptive multitasking
* Targeting a microcontroller platform for practical hardware experience
* Emphasis on learning low-level programming, OS fundamentals, and embedded systems



---

### 🛠 Tech Stack

* **Language:** C,ASM
* **Build System:** CMake 
* **Version Control:** Git + GitHub
* **Controller:** Raspberry Pi Pico


---

### 🧩 Key Features (in Scope)

* [ ] Bootloader development (bare-metal, minimal hardware init)
* [ ] Kernel startup and basic hardware abstraction
* [ ] Basic memory management (simple allocator or static allocation)
* [ ] Task scheduling (round-robin or simple preemptive scheduler)
* [ ] Basic inter-task communication (optional)
* [ ] Simple I/O (serial console or LED control)
* [ ] Porting to a microcontroller (e.g., ARM Cortex-M) with debugging support
