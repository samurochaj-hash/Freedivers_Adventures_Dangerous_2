# Freediver's Adventures Dangerous 2 — Nintendo 64

Original homebrew prototype targeting a `.z64` ROM with Libdragon.

## Milestone 3: perspective scene and low-poly diver

This milestone replaces the earlier flat placeholder with a small CPU-projected 3D-style scene rendered as triangles through RDPQ:

- Perspective projection from world coordinates to screen coordinates.
- Low-poly diver assembled from shaded solid triangles.
- Simple swimming kick animation.
- Low-poly coral, rocks, and fish.
- Follow-camera movement: D-pad moves the diver through the scene.
- HP and blue air meters, plus a depth gauge.
- Underwater particles and layered seabed.

The scene uses painter-style draw ordering and is an early 3D prototype. It does not yet include a full z-buffered mesh pipeline, textures, skeletal animation, sound, or a finished level.

## Controls

- D-pad: swim left/right/up/down.

## Build

Install the official Libdragon toolchain and set `N64_INST` to its installation directory. Then run:

```sh
make
```

The Makefile uses Libdragon's `n64.mk` rules. If the build succeeds, the `.z64` ROM is written under `build/`.

This package contains source code only. The SDK/toolchain is not bundled and no `.z64` ROM is claimed to have been compiled or tested here.

## Project structure

- `src/main.c` — game loop, perspective projection, low-poly scene, controls and HUD.
- `Makefile` — Libdragon build entry point.
