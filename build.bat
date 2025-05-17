@echo off
:: Run Docker container and build inside it
docker run --rm -it -v "%cd%":/root/env myos-buildenv bash -c "cd /root/env && make build-x86_64"

:: Check if build succeeded
if exist dist\x86_64\kernel.iso (
    echo Build successful, running QEMU...
    qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso
) else (
    echo Build failed or ISO not found.
)
