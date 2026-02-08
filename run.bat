@echo off

:: Check if build succeeded
if exist dist\x86_64\kernel.iso (
  echo %_fGreen%[INFO]%_RESET% Running QEMU...
  qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso 
    -serial file:debug_log/.debug_log.txt 
    -monitor stdio
  ) else (
  echo %_fRed%[ERROR]%_RESET% Build failed or ISO not found.
)
