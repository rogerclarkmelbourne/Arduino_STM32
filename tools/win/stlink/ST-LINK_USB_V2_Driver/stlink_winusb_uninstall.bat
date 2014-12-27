@echo off
if "%PROCESSOR_ARCHITECTURE%"=="x86" goto X86
dpinst_amd64.exe /u stlink_dbg_winusb.inf
dpinst_amd64.exe /u stlink_VCP.inf
goto END
:X86
dpinst_x86.exe /u stlink_dbg_winusb.inf
dpinst_x86.exe /u stlink_VCP.inf
:END
