@echo off
if not exist "MockInterviewBot.exe" (
    echo [*] Executable not found. Running build.bat first...
    call build.bat
    if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
)

echo [*] Launching Adaptive AI Mock Interview Simulation Bot...
MockInterviewBot.exe %*
