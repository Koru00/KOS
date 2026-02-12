@echo off
:: Professional QEMU launcher with enhanced emulation
echo.
echo ========================================
echo   KOS Professional QEMU Launcher
echo ========================================
echo.

:: Check if ISO exists
if not exist "dist\x86_64\kernel.iso" (
    echo [ERROR] Kernel ISO not found. Building first...
    call .\build-only.bat
    if errorlevel 1 (
        echo [ERROR] Build failed. Cannot launch QEMU.
        pause
        exit /b 1
    )
)

:: Create debug directory
if not exist "debug_log" mkdir debug_log

:: Check for QEMU installation
where qemu-system-x86_64 >nul 2>&1
if errorlevel 1 (
    echo [ERROR] QEMU not found. Please install QEMU for Windows.
    echo Download from: https://qemu.weilnetz.de/w64/
    pause
        exit /b 1
)

:: Display launch options
echo [1] Standard Mode (with display)
echo [2] Debug Mode (with GDB support)
echo [3] Benchmark Mode (headless for performance testing)
echo [4] Custom Mode (advanced options)
echo.
set /p choice="Select launch mode (1-4): "

if "%choice%"=="1" goto standard
if "%choice%"=="2" goto debug
if "%choice%"=="3" goto benchmark
if "%choice%"=="4" goto custom
echo [ERROR] Invalid choice. Defaulting to standard mode.
goto standard

:standard
echo.
echo [INFO] Launching KOS in Standard Mode...
echo [INFO] Monitor available on telnet localhost:1234
echo [INFO] Serial output logged to debug_log\debug_log.txt
echo.
qemu-system-x86_64 ^
    -cpu qemu64 ^
    -smp 4 ^
    -m 2048 ^
    -machine q35 ^
    -vga std ^
    -drive file=dist/x86_64/kernel.iso,if=ide,index=0,media=cdrom ^
    -serial file:debug_log/debug_log.txt ^
    -monitor telnet:localhost:1234,server,nowait ^
    -boot d
goto end

:debug
echo.
echo [INFO] Launching KOS in Debug Mode...
echo [INFO] GDB server listening on localhost:1234
echo [INFO] Connect with: gdb-multiarch -ex "target remote localhost:1234"
echo [INFO] Trace log: debug_log\qemu_trace.log
echo.
qemu-system-x86_64 ^
    -enable-kvm ^
    -cpu host ^
    -smp 4 ^
    -m 2048 ^
    -machine q35 ^
    -vga virtio ^
    -drive id=hd0,file=dist/x86_64/kernel.iso,if=none,format=raw,media=cdrom ^
    -device ide-cd,drive=hd0 ^
    -serial file:debug_log/debug_log.txt ^
    -monitor telnet:localhost:1234,server,nowait ^
    -s -S ^
    -d int,cpu_reset ^
    -D debug_log/qemu_trace.log ^
    -boot d
goto end

:benchmark
echo.
echo [INFO] Launching KOS in Benchmark Mode (headless)...
echo [INFO] Performance testing - no display output
echo [INFO] Results will be logged to debug_log\debug_log.txt
echo.
qemu-system-x86_64 ^
    -enable-kvm ^
    -cpu host ^
    -smp 4 ^
    -m 2048 ^
    -machine q35,accel=kvm ^
    -display none ^
    -vga none ^
    -drive id=hd0,file=dist/x86_64/kernel.iso,if=none,format=raw,media=cdrom ^
    -device ide-cd,drive=hd0 ^
    -serial file:debug_log/debug_log.txt ^
    -monitor telnet:localhost:1234,server,nowait ^
    -boot d
goto end

:custom
echo.
echo [INFO] Custom QEMU launch - enter your parameters below
echo [INFO] Base parameters: -enable-kvm -cpu host -smp 4 -m 2048
echo.
set /p custom_params="Enter additional QEMU parameters: "
echo.
echo [INFO] Launching with custom parameters...
qemu-system-x86_64 ^
    -enable-kvm ^
    -cpu host ^
    -smp 4 ^
    -m 2048 ^
    -machine q35 ^
    %custom_params% ^
    -drive file=dist/x86_64/kernel.iso,if=ide,index=0,media=cdrom ^
    -serial file:debug_log/debug_log.txt ^
    -boot d

:end
echo.
echo [INFO] QEMU session ended.
pause
