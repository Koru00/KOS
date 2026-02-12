@echo off
:: KOS Cleanup Utility

echo.
echo ========================================
echo   KOS Cleanup Utility
echo ========================================
echo.

:: Options
echo [1] Clean Build Artifacts
echo [2] Clean Debug Logs
echo [3] Clean Everything
echo.
set /p choice="Select option (1-3): "

if "%choice%"=="1" goto clean_build
if "%choice%"=="2" goto clean_logs
if "%choice%"=="3" goto clean_all
echo [ERROR] Invalid choice. Defaulting to build cleanup.
goto clean_build

:clean_build
echo Cleaning build artifacts...
docker run --rm -it -v "%cd%":/root/env myos-buildenv bash -c "cd /root/env && make clean" 2>nul
if exist "build" rmdir /s /q "build" 2>nul
if exist "dist" rmdir /s /q "dist" 2>nul
echo [SUCCESS] Build artifacts cleaned.
goto end

:clean_logs
echo Cleaning debug logs...
if exist "debug_log" rmdir /s /q "debug_log" 2>nul
echo [SUCCESS] Debug logs cleaned.
goto end

:clean_all
echo Cleaning everything...
call :clean_build
call :clean_logs
echo [SUCCESS] All files cleaned.
goto end

:end
pause
