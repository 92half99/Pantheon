@echo off
REM Move to the parent directory
pushd ..

REM Run Premake to generate Visual Studio 2022 solution
Utopia\vendor\bin\premake\Windows\premake5.exe --file=Build-App.lua vs2022

REM Return to the original directory
popd

REM Pause to view any output
pause
