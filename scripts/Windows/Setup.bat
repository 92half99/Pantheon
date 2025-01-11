@echo off

pushd ..\..
Utopia\vendor\bin\premake\Windows\premake5.exe --file=Build-Client.lua vs2022
Utopia\vendor\bin\premake\Windows\premake5.exe --file=Build-Server.lua vs2022
popd
pause