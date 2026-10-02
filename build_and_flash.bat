@echo off
setlocal enabledelayedexpansion

echo STEP 1: CLEANING BUILD DIRECTORY
if exist build (
    rmdir /s /q build
)
echo.

echo STEP 2: CONFIGURING PROJECT WITH CMAKE
cmake --preset Debug
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b %errorlevel%
)
echo.

echo STEP 3: COMPILING FIRMWARE
cmake --build --preset Debug
if %errorlevel% neq 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b %errorlevel%
)

echo.
echo Build successful.
pause