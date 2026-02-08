#!/usr/bin/env bash

# -----------------------------------------------------------------------------
# build.sh — Build and run kernel in Docker + QEMU, with clean, colored output
# -----------------------------------------------------------------------------

# Color codes for pretty output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Helper to print a section header
header() {
    echo -e "\n${CYAN}▶▶ $1${NC}"
}

# Helper to print status messages
success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
info()    { echo -e "${YELLOW}[INFO]${NC}    $1"; }
error()   { echo -e "${RED}[ERROR]${NC}   $1"; }

# Exit on error, undefined variable, or pipefail
set -euo pipefail
trap 'error "Unexpected failure at line $LINENO."; exit 1' ERR

clear

header "Starting Docker build"
# Check if Docker image exists
if ! sudo docker image inspect myos-buildenv >/dev/null 2>&1; then
    error "Docker image 'myos-buildenv' not found."
    echo "Please build it using:"
    echo "  docker build -t myos-buildenv docker/"
    exit 1
fi

# Run the build inside Docker
if sudo docker run --rm -it \
    -v "$(pwd)":/root/env \
    myos-buildenv bash -lc "cd /root/env && make"; then
    success "Kernel compiled inside Docker."
else
    error "Build failed inside Docker."
    exit 1
fi

header "Verifying ISO"
ISO_PATH="dist/x86_64/kernel.iso"
if [[ -f "$ISO_PATH" ]]; then
    success "ISO found at $ISO_PATH"
else
    error "ISO not found; build incomplete."
    exit 1
fi

header "Launching QEMU"
# Ensure debug log directory exists
mkdir -p debug_log

# Launch QEMU
qemu-system-x86_64 \
    -cdrom "$ISO_PATH" \
    -serial file:debug_log/.debug_log.txt \
    -monitor stdio

# On QEMU exit
clear
success "QEMU session ended."

exit 0
