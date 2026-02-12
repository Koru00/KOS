@echo off
:: KOS Performance Monitor
setlocal enabledelayedexpansion

echo.
echo ========================================
echo   KOS Performance Monitor
echo ========================================
echo.

:: Check if debug log exists
if not exist "debug_log\debug_log.txt" (
    echo [ERROR] No performance data found.
    echo [INFO] Run KOS first to generate data.
    pause
    exit /b 1
)

:: Options
echo [1] View Performance Data
echo [2] Performance Report
echo [3] Real-time Monitor
echo.
set /p choice="Select option (1-3): "

if "%choice%"=="1" goto view_data
if "%choice%"=="2" goto report
if "%choice%"=="3" goto monitor
echo [ERROR] Invalid choice. Defaulting to view data.
goto view_data

:view_data
echo.
echo Performance Data:
echo ----------------
type "debug_log\debug_log.txt"
pause
goto end

:report
echo.
echo Generating performance report...
set "report_file=perf_report_%date:~-4%%date:~4,2%%date:~7,2%.txt"
set "report_file=%report_file: =0%"

(
echo KOS Performance Report
echo Generated: %date% %time%
echo ========================================
echo.
echo Performance Data:
echo --------------------
) > "%report_file%"

findstr /i "cpu\|memory\|timing\|alert" "debug_log\debug_log.txt" >> "%report_file%" 2>nul

echo [SUCCESS] Report generated: %report_file%
set /p view="View report? (y/n): "
if /i "%view%"=="y" type "%report_file%"
pause
goto end

:monitor
echo.
echo Real-time Performance Monitor
echo Press Ctrl+C to stop
echo.

:monitor_loop
cls
echo ========================================
echo KOS Real-Time Performance Monitor
echo ========================================
echo Time: %time%
echo.
echo Current Performance Data:
echo ------------------------
type "debug_log\debug_log.txt" 2>nul
echo.
echo Updating in 3 seconds...
ping -n 4 127.0.0.1 >nul 2>&1
goto monitor_loop

:end
