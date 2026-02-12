@echo off
:: KOS Run System
setlocal enabledelayedexpansion

echo.
echo ========================================
echo   KOS Run System
echo ========================================
echo.

:: Check if ISO exists
if not exist "dist\x86_64\kernel.iso" (
    echo [ERROR] Kernel ISO not found.
    echo [INFO] Run build.bat first.
    pause
    exit /b 1
)

:: Check QEMU
where qemu-system-x86_64 >nul 2>&1
if errorlevel 1 (
    echo [ERROR] QEMU not found.
    echo [INFO] Install QEMU from: https://qemu.weilnetz.de/w64/
    pause
    exit /b 1
)

:: Create debug directory
if not exist "debug_log" mkdir debug_log

:: Run options
echo [1] Standard Mode
echo [2] Debug Mode
echo [3] Benchmark Mode
echo.
set /p choice="Select mode (1-3): "

if "%choice%"=="1" goto standard
if "%choice%"=="2" goto debug
if "%choice%"=="3" goto benchmark
echo [ERROR] Invalid choice. Defaulting to standard mode.
goto standard

:standard
echo [INFO] Launching KOS in standard mode...
qemu-system-x86_64 -cpu qemu64 -smp 4 -m 2048 -machine q35 -vga std -drive file=dist/x86_64/kernel.iso,if=ide,index=0,media=cdrom -serial file:debug_log/debug_log.txt -monitor telnet:localhost:1234,server,nowait -boot d
goto end

:debug
echo [INFO] Launching KOS in debug mode...
qemu-system-x86_64 -cpu qemu64 -smp 4 -m 2048 -machine q35 -vga std -drive file=dist/x86_64/kernel.iso,if=ide,index=0,media=cdrom -serial file:debug_log/debug_log.txt -monitor telnet:localhost:1234,server,nowait -s -S -d int,cpu_reset -D debug_log/qemu_trace.log -boot d
goto end

:benchmark
echo [INFO] Launching KOS in benchmark mode...
qemu-system-x86_64 -cpu qemu64 -smp 4 -m 2048 -machine q35 -display none -vga none -drive file=dist/x86_64/kernel.iso,if=ide,index=0,media=cdrom -serial file:debug_log/debug_log.txt -monitor telnet:localhost:1234,server,nowait -boot d
goto end

:end
pause
