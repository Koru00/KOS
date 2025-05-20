@echo off
:: Run Docker container and build inside it
docker run --rm -it -v "%cd%":/root/env myos-buildenv bash -c "cd /root/env && make build-x86_64"

:: Capture Docker's exit code
if errorlevel 1 (
    echo [ERROR] Build failed inside Docker.
    exit /b 1
) else (
    echo [SUCCESS] Build complete!
)

:: Check if ISO exists
if exist dist\x86_64\kernel.iso (
    echo [INFO] Build successful. Launching QEMU...
    qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -serial file:.debug_log -monitor stdio
) else (
    echo [ERROR] Build finished but ISO not found.
    exit /b 1
)
