# Particle Photon 2: RTOS Hybrid Architecture Explorer

This project demonstrates an implementation of a Hybrid RTOS Architecture on the Particle Photon 2 (Realtek RTL872x). 

## Key Features

* **True Multithreading:** Utilization of native `os_thread_create` to spawn parallel tasks for Sensor Acquisition (High Priority) and UI Rendering (Low Priority).
* **Inter-Process Communication (IPC):** Uses `os_queue` for thread-safe data passing and `os_mutex` for shared resource protection (I2C Bus).
* **Edge Computing:** Implements an EMA (Exponential Moving Average) filter running at 20Hz on the edge, decoupled from cloud latency.
* **Reactive UI Pattern:**
    * **Idle:** OLED updates every 5 seconds to save resources.
    * **Active:** OLED updates *immediately* upon user interaction (Interrupts/Button Press).
* **Cloud Synchronization:** Asynchronous "Shadow Variable" pattern to keep Particle Cloud in sync without blocking critical real-time tasks.

## Hardware Requirements

* **MCU:** Particle Photon 2 (Device OS 6.x+)
* **Display:** Adafruit FeatherWing OLED - (I2C: 0x3C)
* **Input:** FeatherWing Button's Push connected to D2, D3, D4.
* **Output:** Status LED (D7) and RGB System LED.

## Project Structure

```text
src/
├── algorithms/       # Pure C logic (EMA Filter)
├── drivers/          # Hardware Abstraction Layer (GPIO, Storage)
├── modules/          # RTOS Tasks (Sensor, UI, Cloud)
├── system/           # Global definitions, Structs, RTOS Handles
└── main.cpp          # Entry point & Thread Orchestrator
