@echo off
:: KOS Build System (modalità semplificata)
setlocal enabledelayedexpansion

echo.
echo ========================================
echo   KOS Build System - Simple Build Only
echo ========================================
echo.

:: Check Docker
docker --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Docker not found. Please install Docker Desktop.
    pause
    exit /b 1
)

echo [INFO] Building KOS...
docker run --rm -it -v "%cd%":/root/env myos-buildenv bash -c "cd /root/env && make"

if errorlevel 1 (
    echo [ERROR] Build failed.
    exit /b 1
)

echo [SUCCESS] Build completed.
exit /b


:: ============================================================
::   CODICE VECCHIO (NON ESEGUITO) — VIENE SALTATO SEMPRE
:: ============================================================
goto :EOF


:: --- Tutto ciò che segue è il tuo script originale ---
:: (rimane qui come archivio, ma non verrà mai eseguito)

echo [1] Standard Build
echo [2] Debug Build  
echo [3] Clean Build
echo [4] Build and Run
echo.
set /p choice="Select option (1-4): "

if "%choice%"=="1" goto standard
if "%choice%"=="2" goto debug
if "%choice%"=="3" goto clean
if "%choice%"=="4" goto build_run
echo [ERROR] Invalid choice. Defaulting to standard build.
goto standard

:standard
echo [INFO] Building KOS...
docker run --rm -it -v "%cd%":/root/env myos-buildenv bash -c "cd /root/env && make"
goto check_result

:debug
echo [INFO] Building KOS (Debug)...
docker run --rm -it -v "%cd%":/root/env myos-buildenv bash -c "cd /root/env && make DEBUG=1"
goto check_result

:clean
echo [INFO] Cleaning build artifacts...
docker run --rm -it -v "%cd%":/root/env myos-buildenv bash -c "cd /root/env && make clean"
echo [SUCCESS] Build artifacts cleaned.
goto end

:build_run
call :standard
if errorlevel 1 goto end
echo [INFO] Launching KOS...
call run.bat
goto end

:check_result
if errorlevel 1 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
) else (
    echo [SUCCESS] Build completed.
    if exist "dist\x86_64\kernel.iso" (
        echo [INFO] Kernel ISO: dist\x86_64\kernel.iso
        for %%A in ("dist\x86_64\kernel.iso") do echo [INFO] Size: %%~zA bytes
    )
)

:end
pause
