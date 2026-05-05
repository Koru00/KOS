#!/usr/bin/env bash

# Colors
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
YELLOW=$(tput setaf 3)
CYAN=$(tput setaf 6)
NC=$(tput sgr0)

header() { echo -e "\n${CYAN}▶▶ $1${NC}"; }
success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
info()    { echo -e "${YELLOW}[INFO]${NC}    $1"; }
error()   { echo -e "${RED}[ERROR]${NC}   $1"; }

set -euo pipefail
trap 'error "Unexpected failure at line $LINENO."; exit 1' ERR

clear

header "Checking Docker"
DOCKER="docker"
if ! docker ps >/dev/null 2>&1; then
    if command -v sudo >/dev/null; then
        DOCKER="sudo docker"
    else
        error "Docker needs root privileges, and sudo is missing."
        exit 1
    fi
fi

if ! $DOCKER image inspect myos-buildenv >/dev/null 2>&1; then
    error "Docker image 'myos-buildenv' not found."
    echo "Please build it with: docker build -t myos-buildenv docker/"
    exit 1
fi

header "Starting Docker build"
if $DOCKER run --rm -it -v "$(pwd)":/root/env myos-buildenv bash -lc "cd /root/env && make"; then
    success "Kernel compiled successfully."
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
mkdir -p debug_log

echo -e "${CYAN}--------------------------------------------------${NC}"
echo -e "${CYAN}                QEMU Monitor Start                ${NC}"
echo -e "${CYAN}--------------------------------------------------${NC}"

qemu-system-x86_64 \
    -cdrom "$ISO_PATH" \
    -serial file:debug_log/.debug_log.txt \
    -monitor stdio

clear
success "QEMU session ended."

exit 0
