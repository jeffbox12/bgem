#!/bin/sh -e

if command -v sudo >/dev/null 2>&1; then
    SUDO="sudo"
else
    SUDO=""
fi

if [ "$(id -u)" -eq 0 ]; then
    # Check if we are in a known CI environment, or if the developer explicitly allowed it.
    if [ "$CI" != "true" ] && [ "$ALLOW_ROOT_BUILD" != "1" ]; then
        echo "==================================================================="
        echo " Error: You are running this script as root (or via sudo)."
        echo " This will cause permission issues with git and vcpkg files."
        echo ""
        echo " Please run this script as a normal user:"
        echo "   ./bootstrap-bgem.sh"
        echo ""
        echo " If you are in a container/CI and MUST run as root, prefix it:"
        echo "   ALLOW_ROOT_BUILD=1 ./bootstrap-bgem.sh"
        echo "==================================================================="
        exit 1
    fi
fi

install_linux_dependencies() {
    if command -v apt-get &> /dev/null; then
        echo "Detected: Debian/Ubuntu (APT)"
        $SUDO apt-get update
        $SUDO apt-get install -y \
            `# Build tools` \
            build-essential cmake ninja-build make pkg-config git \
            autoconf autoconf-archive automake libtool \
            `# Scripting runtimes` \
            python3-venv perl perl-modules \
            `# Network & archives` \
            curl zip unzip tar ca-certificates \
            `# SDL3 — Audio` \
            libasound2-dev libpulse-dev libaudio-dev libjack-dev libsndio-dev libpipewire-0.3-dev \
            `# SDL3 — Video/Display` \
            libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev \
            libxi-dev libxss-dev libxtst-dev libxkbcommon-dev \
            libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev libegl1-mesa-dev \
            libvulkan-dev \
            `# SDL3 — Wayland` \
            libwayland-dev wayland-protocols libdecor-0-dev \
            `# SDL3 — Input & system` \
            libdbus-1-dev libibus-1.0-dev libudev-dev \
            libhidapi-dev libusb-1.0-0-dev liburing-dev \
            `# Freetype & HarfBuzz (text rendering)` \
            libfribidi-dev libthai-dev \
            `# Desktop integration` \
            gnome-desktop-testing \
            `# This solves an issue with finding decorations plugins in Linux Mint. Research more into it` \
            libdecor-0-plugin-1-gtk \
            `# libxcrypt depends on ltdl development files` \
            libltdl-dev

    elif command -v dnf &> /dev/null; then
        echo "Detected: Fedora/RHEL (DNF)"
        $SUDO dnf install -y \
            `# Build tools` \
            @development-tools @c-development cmake ninja-build make pkgconfig git-core \
            autoconf autoconf-archive automake libtool \
            `# Scripting runtimes` \
            python3 perl perl-core \
            `# Network & archives` \
            curl zip unzip tar ca-certificates \
            `# SDL3 — Audio` \
            alsa-lib-devel pulseaudio-libs-devel pipewire-devel \
            pipewire-jack-audio-connection-kit-devel \
            `# SDL3 — Video/Display` \
            libX11-devel libXext-devel libXrandr-devel libXcursor-devel libXfixes-devel \
            libXi-devel libXScrnSaver-devel libXtst-devel libxkbcommon-devel \
            libdrm-devel mesa-libgbm-devel mesa-libGL-devel mesa-libGLES-devel mesa-libEGL-devel \
            vulkan-devel \
            `# SDL3 — Wayland` \
            wayland-devel wayland-protocols-devel libdecor-devel \
            `# SDL3 — Input & system` \
            dbus-devel ibus-devel systemd-devel \
            libusb1-devel liburing-devel \
            `# Freetype & HarfBuzz (text rendering)` \
            fribidi-devel libthai-devel \
            `# Static libs required by vcpkg builds` \
            zlib-ng-compat-static \
            `# libxcrypt depends on ltdl development files` \
            libtool-ltdl-devel

    elif command -v pacman &> /dev/null; then
        echo "Detected: Arch Linux (Pacman)"
        $SUDO pacman -Syu --noconfirm --needed \
            `# Build tools` \
            base-devel cmake ninja make pkgconf git \
            autoconf autoconf-archive automake libtool \
            `# Scripting runtimes` \
            python perl \
            `# Network & archives` \
            curl zip unzip tar ca-certificates \
            `# SDL3 — Audio` \
            alsa-lib libpulse jack pipewire sndio \
            `# SDL3 — Video/Display` \
            libx11 libxext libxrandr libxcursor libxfixes libxi libxinerama \
            libxkbcommon libxrender libxss libxtst \
            libgl mesa vulkan-headers vulkan-driver \
            `# SDL3 — Wayland` \
            wayland wayland-protocols libdecor \
            `# SDL3 — Input & system` \
            ibus hidapi libusb liburing \
            `# Freetype & HarfBuzz (text rendering)` \
            fribidi libthai

    elif command -v zypper &> /dev/null; then
        echo "Detected: openSUSE (Zypper)"
        $SUDO zypper install -y -t pattern devel_basis
        $SUDO zypper in -y \
            `# Build tools` \
            cmake ninja make pkg-config git \
            autoconf autoconf-archive automake libtool \
            gcc gcc-c++ \
            `# Scripting runtimes` \
            python3-devel perl perl-base \
            `# Network & archives` \
            curl zip unzip tar ca-certificates \
            `# SDL3 — Audio` \
            alsa-devel libpulse-devel pipewire-devel sndio-devel \
            `# SDL3 — Video/Display` \
            libX11-devel libXext-devel libXrandr-devel libXcursor-devel libXfixes-devel \
            libXi-devel libXScrnSaver-devel libXtst-devel libxkbcommon-devel \
            libdrm-devel libgbm-devel Mesa-libGL-devel Mesa-libEGL-devel \
            vulkan-devel \
            `# SDL3 — Wayland` \
            wayland-devel wayland-protocols-devel xwayland-devel libdecor-devel \
            `# SDL3 — Input & system` \
            dbus-1-devel ibus-devel libudev-devel \
            libusb-1_0-devel liburing-devel \
            `# Freetype & HarfBuzz (text rendering)` \
            fribidi-devel libthai-devel \
            `# Misc` \
            libunwind-devel

    else
        echo "Error: Could not identify a compatible package manager."
        exit 1
    fi
}

install_macos_dependencies() {
    if ! xcode-select -p &> /dev/null; then
        echo ""
        echo "Error: Missing Xcode Command Line Tools."
        echo "Run this command to install it:"
        echo "   xcode-select --install"
        echo "Note: GUI pop-ups will appear during the installation."
        echo ""
        exit 1
    else
        echo "Detected Xcode Command Line Tools is installed."
    fi

    if ! command -v brew &> /dev/null; then
        echo ""
        echo "Error: Missing Homebrew."
        echo "Run this command to install it:"
        echo "   /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
        echo ""
    else
        echo "Detected Homebrew. Installing packages..."
        brew install cmake ninja pkg-config
    fi
}

if [ "$(uname)" = "Linux" ]; then
    install_linux_dependencies
elif [ "$(uname)" = "Darwin" ]; then
    install_macos_dependencies
else
    echo "Error: Unsupported operating system"
    exit 1
fi

# Resolve paths
SCRIPT_DIR="$PROJECT_ROOT/scripts"

# Check if this is a git repo
if [ -d "$PROJECT_ROOT/.git" ]; then
    git -C "$PROJECT_ROOT" submodule update --init --recursive
else
    git clone https://github.com/microsoft/vcpkg "$PROJECT_ROOT/vcpkg"
fi

# Bootstrap vcpkg
echo "Bootstrapping vcpkg..."
"$PROJECT_ROOT/vcpkg/bootstrap-vcpkg.sh"
