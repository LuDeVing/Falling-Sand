# 🏖️ Sand Simulation in C++

This is a simple particle-based sand simulation written in C++ using the Win32 API. It simulates sand and water behavior with real-time interaction and visualization.

> ⚠️ **Note:** This project uses the Windows API, so it's **Windows-only** and must be compiled with the subsystem set to `Windows`.

---

## 📸 Features

- Sand and water block simulation
- Simple fluid and granular physics
- User interaction (drawing particles)
- Real-time updates with pixel rendering
- Solid blocks for building terrain
- Randomized update order for natural motion

---

## 🛠️ Setup & Compilation

### Requirements

- Windows OS
- C++17 or later compiler (e.g., MSVC)
- Win32 development environment (e.g., Visual Studio)

### Build Instructions

1. **Open in Visual Studio**
   - Create a new empty C++ Windows project
   - Paste the code into a `.cpp` file
   - Set **Linker > System > Subsystem** to `Windows` (not `Console`)

2. **Compile and run**

---

## 🕹️ Controls

| Action             | Input             |
|--------------------|------------------|
| Draw blocks        | Left Mouse Button |
| Select SAND        | Press `1`        |
| Select WATER       | Press `2`        |
| Select SOLID block | Press `3`        |

---

## 🧪 How It Works

- The screen is treated as a 2D grid of `Block` objects.
- Each block has a velocity and a type (`NONE`, `SAND`, `WATER`, `SOLID`).
- `updateSand()` and `updateWater()` handle particle behavior.
- Simulation updates happen per frame in a randomized order.
- Rendering is done directly to a memory buffer and painted to the screen using `StretchDIBits`.

---

## 🧠 Concepts Used

- Basic physics for gravity and flow
- Frame-rate independent motion using `deltaTime`
- Pixel-level manipulation with `StretchDIBits`
- Win32 API for window management and input
- Double buffering using `VirtualAlloc` for performance

---

## 📝 License

This project is released under the [MIT License](https://opensource.org/licenses/MIT) – feel free to use and modify it for any purpose.

---

Enjoy playing with falling pixels! 🌊⛱️🔥
