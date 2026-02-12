@echo off
:: KOS Debug Tools
setlocal enabledelayedexpansion

echo.
echo ========================================
echo   KOS Debug Tools
echo ========================================
echo.

:: Options
echo [1] GDB Debug Session
echo [2] QEMU Monitor
echo [3] View Debug Log
echo.
set /p choice="Select option (1-3): "

if "%choice%"=="1" goto gdb
if "%choice%"=="2" goto monitor
if "%choice%"=="3" goto logs
echo [ERROR] Invalid choice. Defaulting to GDB.
goto gdb

:gdb
echo.
echo Starting GDB debug session...
echo Make sure KOS is running in debug mode.

:: Check GDB
where gdb >nul 2>&1
if errorlevel 1 (
    where gdb-multiarch >nul 2>&1
    if errorlevel 1 (
        echo [ERROR] GDB not found.
        pause
        exit /b 1
    ) else (
        set "GDB_CMD=gdb-multiarch"
    )
) else (
    set "GDB_CMD=gdb"
)

:: Create GDB script
(
echo target remote localhost:1234
echo set architecture i386:x86-64
echo set print pretty on
echo break kernel_main
echo continue
) > debug_log\gdb_script.txt

echo Starting GDB...
%GDB_CMD% -x debug_log\gdb_script.txt
goto end

:monitor
echo.
echo Connecting to QEMU monitor...
echo Type 'quit' to exit monitor.

where telnet >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Telnet not available.
    pause
    exit /b 1
)

telnet localhost 1234
goto end

:logs
echo.
echo Debug Log Analysis:
echo ------------------
if exist "debug_log\debug_log.txt" (
    type "debug_log\debug_log.txt"
) else (
    echo [ERROR] No debug log found.
)
pause
goto end

:end
