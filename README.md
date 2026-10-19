# 🌌 OpenGL Spacetime Galaxy

An interactive 3D visualization of the Solar System and a procedurally generated galaxy, rendered natively in C++ using OpenGL. 

This project visualizes Einstein's General Relativity by rendering planets as massive bodies that warp a flexible "spacetime grid," and features a seamless camera transition from planetary scale out to a full galactic scale.

![Spacetime Grid Preview](https://img.shields.io/badge/OpenGL-3.3-blue) ![C++](https://img.shields.io/badge/C++-17-orange) ![GLM](https://img.shields.io/badge/GLM-0.9.9-green) ![GLAD](https://img.shields.io/badge/GLAD-Core-red)

---

## ✨ Features

- **General Relativity Spacetime Grid**: A procedurally warped mesh that dips into gravity wells based on planetary mass. Deep wells fade to black, simulating visual gravity wells.
- **Complete Solar System**: The Sun, all 8 planets (with accurate color profiles), and Earth's Moon, complete with hierarchical orbital mechanics.
- **Infinite Zooming**: Seamless transition between scales. Zoom in to inspect planetary gravity wells, or zoom out to see a 100,000-star spiral galaxy fade into view.
- **Procedural Galaxy**: A custom-shader driven galaxy using logarithmic spiral math, complete with a rotating core and halo stars.
- **Smooth Camera System**: Custom orbit-camera with exponential zoom, smooth interpolation (slack/lerp), and right-click panning.
- **High Performance**: Planet data is shared across single high-poly meshes. Gravity calculations are done entirely on the GPU using vertex shaders.

---

## 🎮 Controls

| Input | Action |
|-------|--------|
| **Scroll Wheel** | Zoom In / Out (Fades between Spacetime Grid and Galaxy) |
| **Left Click + Drag** | Orbit the camera around the center (Sun) |
| **Right Click + Drag** | Pan the camera (changes the look-at target) |
| **R** | Reset camera to default view |
| **Space** | Pause / Resume the simulation |
| **+ / -** | Increase / Decrease simulation speed |

---

## 🛠️ Prerequisites

You will need a C++ compiler (supporting C++17), CMake, and the following libraries:
- **GLFW** (Windowing)
- **GLM** (Mathematics)
- **GLAD** (OpenGL Loader)
- **Dear ImGui** (Optional, if using the UI branch)

### Ubuntu / Debian setup:
```bash
sudo apt update
sudo apt install build-essential cmake libglfw3-dev libglm-dev
