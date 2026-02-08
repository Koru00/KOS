#!/bin/bash

# Check if the ISO file exists
if [ -f dist/x86_64/kernel.iso ]; then
    echo "Running QEMU..."
    qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso \
        -serial file:debug_log/.debug_log.txt \
        -monitor stdio
else
    echo "Build failed or ISO not found."
fi

