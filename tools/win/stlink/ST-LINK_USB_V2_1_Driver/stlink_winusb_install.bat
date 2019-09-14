@echo off
if "%PROCESSOR_ARCHITEW6432%" == "AMD64" goto X64
if "%PROCESSOR_ARCHITECTURE%" == "AMD64" goto X64
start "" dpinst_x86.exe 
goto END
:X64
start "" dpinst_amd64.exe
:END