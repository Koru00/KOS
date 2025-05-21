# KOS - Koru Operating System

![logo](logo.png)

A lightweight, experimental x86-64 operating system written in C and assembly.  
**KOS** is a personal learning project by [Koru](https://www.twitch.tv/koru_chan00), designed to explore low-level system architecture from the ground up.

---

## Status

> **Current Version**: `0.1`  
> **Stage**: Pre-Alpha  
> **Target Architecture**: x86-64 long mode
> **Goal**: Build a custom kernel with interrupts, PIT, memory management, and a shell interface.

---

## Requirements

To build and run **KOS**, you’ll need the following tools installed on your system:

- [Docker](https://www.docker.com/) – for building in a consistent environment  
- [QEMU](https://www.qemu.org/) – to emulate the OS

---

## Quick Start

### Build + Run

Builds the kernel inside a Docker container and launches it in QEMU:

```bash
./build
```

### Run Only

Runs the most recently built ISO image (assumes it already exists):

```bash
./run
```

---

## Problem
- PTI

---

## Contributing

While this is a solo project for now, contributions, suggestions, and questions are always welcome!

To contribute:

1. Fork the repo
2. Create a new branch
3. Make your changes
4. Open a Pull Request

Or you can follow me on:
- [Twitch](https://www.twitch.tv/koru_chan00)
- [Youtube](https://www.youtube.com/@Koru_chan00)
- [X](https://x.com/_Koru_chan_)

---

## License

Currently **unlicensed**. This project is for educational and experimental use only.  
A formal open-source license (likely MIT or GPL) will be added once the project is more complete.

---

## Support

If you like the project, consider leaving a ⭐ on GitHub to motivate future development!
