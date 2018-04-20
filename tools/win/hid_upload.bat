@echo off
rem: Note %~dp0 get path of this batch file
rem: Need to change drive if My Documents is on a drive other than C:
set driverLetter=%~dp0
set driverLetter=%driverLetter:~0,2%
%driverLetter%
cd %~dp0
rem: the two line below are needed to fix path issues with incorrect slashes before the bin file name
set str=%4
set str=%str:/=\%
set commport=%1
set commportnum=%commport:COM=%

rem: ------------- use hid_flash CLI
hid_flash\hid-flash.exe %str% %commportnum%

for /l %%x in (1, 1, 40) do (
  ping -w 50 -n 1 192.0.2.1 > nul
  mode %commportnum > nul
  if ERRORLEVEL 0 goto comPortFound
)

echo timeout waiting for %1 serial