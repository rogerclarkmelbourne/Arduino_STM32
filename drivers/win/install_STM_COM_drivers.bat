@echo off

echo Installing STM Serial driver...
"%~dp0wdi-simple" --vid 0x0483 --pid 0x5740 --type 3 --name "STM Serial" --dest "%~dp0maple-serial"
echo.

pause

