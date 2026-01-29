# Zephyr RTOS Data Logger

A multi-threaded sensor data logger built with Zephyr RTOS, demonstrating real-time operating system concepts and embedded development practices. Designed to run on QEMU for development and testing.

## Overview

This project implements a simulated sensor data aggregation system showcasing:

- Multi-threaded task management with priorities
- Inter-thread communication using message queues
- Periodic data sampling with kernel timers
- Power management state machine
- Shell interface for runtime configuration

## Project Structure

```
zephyr_datalogger/
├── src/
│   └── main.c              # Application entry point
├── boards/                  # Custom board overlays (if needed)
├── docs/                    # Additional documentation
├── prj.conf                 # Zephyr kernel configuration
├── CMakeLists.txt          # Build configuration
└── README.md
```

## Requirements

- Zephyr SDK 0.16.x or later
- CMake 3.20+
- Python 3.8+
- QEMU (installed via Zephyr SDK)

## Environment Setup

### 1. Install Dependencies (macOS)

```bash
brew install cmake ninja gperf python3 ccache qemu dtc wget
pip3 install west
```

### 2. Initialize Zephyr Workspace

```bash
west init -m https://github.com/zephyrproject-rtos/zephyr --mr main ~/zephyrproject
cd ~/zephyrproject
west update
west zephyr-export
pip3 install -r ~/zephyrproject/zephyr/scripts/requirements.txt
```

### 3. Install Zephyr SDK

```bash
cd ~
wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.8/zephyr-sdk-0.16.8_macos-aarch64.tar.xz
tar xvf zephyr-sdk-0.16.8_macos-aarch64.tar.xz
cd zephyr-sdk-0.16.8
./setup.sh
```

### 4. Set Environment Variables

Add to your `~/.zshrc`:

```bash
export ZEPHYR_BASE=~/zephyrproject/zephyr
source ~/zephyrproject/zephyr/zephyr-env.sh
```

## Building

```bash
cd ~/zephyrproject
west build -b qemu_cortex_m3 path/to/zephyr_datalogger
```

## Running

```bash
west build -t run
```

To exit QEMU: `Ctrl+A` then `X`

## Key Concepts Demonstrated

### Threading Model

The application uses multiple threads with different priorities:

| Thread          | Priority | Purpose                          |
|-----------------|----------|----------------------------------|
| sensor_thread   | 7        | Simulates sensor data sampling   |
| logger_thread   | 8        | Processes and logs data          |
| main            | 10       | Initialization and shell         |

### Configuration (Kconfig)

Project-specific settings in `prj.conf`:

- `CONFIG_PRINTK=y` - Enable kernel print
- `CONFIG_LOG=y` - Enable logging subsystem
- `CONFIG_SHELL=y` - Enable interactive shell

### Devicetree

Board-specific hardware descriptions. QEMU Cortex-M3 provides a standard ARM platform for development without physical hardware.

## Development Notes

- Built and tested with Zephyr v3.6.x
- Target board: `qemu_cortex_m3` (emulated ARM Cortex-M3)
- No physical hardware required for basic development

## Resources

- [Zephyr Documentation](https://docs.zephyrproject.org/)
- [Zephyr API Reference](https://docs.zephyrproject.org/latest/doxygen/html/index.html)
- [QEMU ARM System Emulator](https://www.qemu.org/docs/master/system/target-arm.html)

## License

MIT License
