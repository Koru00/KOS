#!/bin/bash

clear

# Run Docker container and build inside it
docker run --rm -it -v "$(pwd)":/root/env myos-buildenv bash -c "cd /root/env && make build-x86_64"
exit_code=$?

# Capture Docker's exit code
if [ $exit_code -ne 0 ]; then
    echo "[ERROR] Build failed inside Docker."
    exit 1
else
    echo "[SUCCESS] Build complete!"
fi

# Check if ISO exists
if [ -f dist/x86_64/kernel.iso ]; then
    echo "[INFO] Build successful. Launching QEMU..."
    qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso \
        -serial file:debug_log/.debug_log.txt \
        -monitor stdio
    clear
else
    echo "[ERROR] Build finished but ISO not found."
    exit 1
fi

