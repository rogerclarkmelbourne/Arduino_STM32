@echo off
setlocal enabledelayedexpansion

if [%4] == "" (
    echo Usage: %0 ^<dummy_port^> ^<altID^> ^<usbID^> ^<binfile^>
    exit /b 1
)

set dummy_port=%1
set altID=%2
set usbID=%3
set "binfile=%4"


:: Get the directory where the script is running
set DIR=%~dp0
set DIR=%DIR:~0,-1%

:: DFU utility path
set "DFU_UTIL=%DIR%\dfu-util\dfu-util.exe"
if not exist "%DFU_UTIL%" (
    echo %0: error: cannot find %DFU_UTIL%
    exit /b 2
)

::  ----------------- IMPORTANT -----------------
:: The 2nd parameter to upload-reset is the delay after resetting before it exits
:: This value is in milliseonds
:: You may need to tune this to your system

"%DIR%\upload_reset.exe" %dummy_port% 750 || (
    echo.
    echo *** autoreset failed; you need to use RESET button
)


%DFU_UTIL% -d %usbID% -a %altID% -D %binfile% -R
if errorlevel 1 (
    echo Upload failed :(
    exit /b 1
)

:: Wait for serial port to reappear
echo Waiting for %dummy_port% serial...
for /l %%x in (1, 1, 40) do (
    mode %dummy_port% > nul 2>&1
    if not errorlevel 1 goto portFound
    ping -w 50 -n 1 192.0.2.1 > nul
)
echo  TIMEOUT :(
goto :eof

:portFound
echo  DONE :)

endlocal