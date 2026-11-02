# Freediver's Adventures Dangerous 2 — Nintendo 64 starter

This is the first playable-code scaffold for an original N64 homebrew project.
It uses Libdragon and targets a `.z64` ROM build.

## Current milestone

- 320x240 video output
- controller polling
- basic underwater-colored scene
- diver placeholder that moves with the D-pad
- HP and air meter graphics
- simple seabed, coral silhouettes and bubbles

**Important:** this milestone is a 2D placeholder scene, not the finished 3D game. The next milestone is to add a 3D camera, low-poly diver/environment meshes, depth-tested geometry, collision and real gameplay systems.

## Build environment

Install the stable Libdragon toolchain and initialize the project environment using the official Libdragon instructions. Set `N64_INST` to the Libdragon installation directory, then run:

```sh
make
```

The Libdragon build rules produce a `.z64` ROM in the build output directory. This source package does not include the SDK/toolchain, so a ROM has not been compiled here.

## Controls in this prototype

- D-pad: move the diver placeholder
