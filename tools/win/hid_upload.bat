@echo off
rem: Note %~dp0 get path of this batch file
rem: Need to change drive if My Documents is on a drive other than C:
set driverLetter=%~dp0
set driverLetter=%driverLetter:~0,2%
%driverLetter%
cd %~dp0
rem: the two line below are needed to fix path issues with incorrect slashes before the bin file name
set str=%1
set str=%str:/=\%
set comport=%2

rem: ------------- use hid_flash CLI

hid_flash\hid-flash.exe %str% %comport%

for /l %%x in (1, 1, 10) do (
	ping -w 50 -n 1 192.0.2.1 > nul
)

mode %comport% | find "RTS" > nul
if errorlevel 1 (
echo Error: %comport% not found
) else (
echo %comport% is Ready !
)




