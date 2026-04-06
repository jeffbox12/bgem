# Project Bluegem

🚧 UNDER CONSTRUCTION 🚧

Multimedia center software for big screens.

Currently under heavy architectural development. Due to the scope and complexity of the project, there will be a lot of under-the-hood works, so don't expect major visual differences.

## How to compile
### Windows (x86_64/ARM64)
Prerequisites:
- Install Git for Windows
- Install Python
- Install Visual Studio 2026
- Check **Desktop Development with C++** Workload and install it
  - If you never installed **Desktop Development with C++**, leave the default options.
  - If you did install **Desktop Development with C++** before, ensure MSVC, Windows SDK, and CMake tools are included

To compile:
- If you are compiling for x86 (64-bits), open **x64 Native Tools Command Line for VS**
- If you are compiling for ARM, open **ARM64 Native Tools Command Line for VS**

- Go to the project path where you Git cloned:

**Important**: Make sure the path has no spaces.
```
cd C:\path\to\bgem
```
- In the VS Command Line, run the bootstrap script:
```
bootstrap-bgem.cmd
```
- Configure the project:
```
cmake --preset debug
```
- Build the project:
```
cd build\debug
ninja
```
- Run the executable
```
.\source\bgem
```
### macOS
**Note:** Can only be compiled for Apple Silicon based Macs as Intel-based Mac support is being deprecated.

Prerequisites:
- Install Xcode Command Line Tools:
```
xcode-select --install
```
- Install Homebrew

To compile:
- Open Terminal
- Go to the project path where you Git cloned:
```
cd /path/to/bgem
```
- Run the bootstrap script
```
./bootstrap-bgem.sh
```
- Configure the project:
```
cmake --preset debug
```
- Build the project:
```
cd build/debug
ninja
```
- Run the executable
```
./source/bgem
```
### Linux (x86_64/ARM64)
To compile:
- Open Terminal
- Go to the project path where you Git cloned:
```
cd /path/to/bgem
```
- Run the bootstrap script:
```
./bootstrap-bgem.sh
```
- Configure the project:
```
cmake --preset debug
```
- Build the project:
```
cd build/debug
ninja
```
- Run the executable:
```
./source/bgem
```

## Tested configurations
macOS Tahoe 26.3 (Apple Silicon)
- Xcode 26.2
- CMake 4.2.1
- Ninja 1.13.2

Windows 11 Pro (ARM64)
- Visual Studio 2026
- CMake 4.1.2-msvc8 (Bundled with Desktop Development with C++)
- Ninja 1.12.1 (Bundled with Desktop Development with C++)

Windows 11 Pro (x86_64)
- Visual Studio 2022
- CMake 4.2.3
- Ninja 1.11.1

Debian 13.2 (ARM64)
- GCC 14.2.0
- CMake 3.31.6
- Ninja 1.12.1

Llnux Mint (x86_64)
- GCC 13.3.0
- CMake 3.28
- Ninja 1.11.1

Debian 13.4 (x86_64)
- GCC 14.2.0
- CMake 3.31.6
- Ninja 1.12.1
