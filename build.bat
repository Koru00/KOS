@echo off
:: Run Docker container and build inside it
docker run --rm -it -v "%cd%":/root/env myos-buildenv bash -c "cd /root/env && make build-x86_64"

Set _fRed=[31m
Set _fGreen=[32m

Set _RESET=[0m

:: Capture Docker's exit code
if errorlevel 1 (
    echo %_fRed%[ERROR]%_RESET% Build failed inside Docker.
    exit /b 1
) else (
    echo %_fGreen%[SUCCESS]%_RESET% Build complete!
)

:: Check if ISO exists
if exist dist\x86_64\kernel.iso (
    echo %_fGreen%[INFO]%_RESET% Build successful. Launching QEMU...
    qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -serial file:debug_log/.debug_log.txt -monitor stdio
) else (
    echo %_fRed%[ERROR]%_RESET% Build finished but ISO not found.
    exit /b 1
)
