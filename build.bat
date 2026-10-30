@echo off
setlocal enabledelayedexpansion

echo =========================================================================
echo   COMPILING ADAPTIVE AI MOCK INTERVIEW BOT (C++17)
echo =========================================================================

:: Check if cl is available
where cl >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [*] Initializing MSVC x64 build environment...
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" >nul
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" >nul
    )
)

where cl >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [ERROR] cl.exe compiler could not be located. Please run from Visual Studio Developer Command Prompt.
    exit /b 1
)

echo [*] Compiling with MSVC Optimization flags (/std:c++17 /EHsc /O2 /utf-8 /DNOMINMAX)...
cl /nologo /std:c++17 /EHsc /O2 /W4 /utf-8 /DNOMINMAX /DWIN32_LEAN_AND_MEAN /Iinclude src\main.cpp /Fe:MockInterviewBot.exe

if %ERRORLEVEL% equ 0 (
    echo.
    echo =========================================================================
    echo   BUILD SUCCESSFUL! Created MockInterviewBot.exe
    echo =========================================================================
    echo   To run the interactive interview bot:
    echo     .\MockInterviewBot.exe
    echo.
    echo   To run an automated benchmark simulation:
    echo     .\MockInterviewBot.exe --demo
    echo =========================================================================
    exit /b 0
) else (
    echo.
    echo [ERROR] Build failed with exit code %ERRORLEVEL%.
    exit /b %ERRORLEVEL%
)
