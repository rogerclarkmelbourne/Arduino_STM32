rem: @echo off
rem: Note %~dp0 get path of this batch file
rem: Need to change drive if My Documents is on a drive other than C:
set driverLetter=%~dp0
set driverLetter=%driverLetter:~0,2%
%driverLetter%
cd %~dp0
rem: the two line below are needed to fix path issues with incorrect slashes before the bin file name
set str=%4
set str=%str:/=\%
%1arm-none-eabi-gdb.exe -b %2  -ex "set debug remote 0" -ex "set target-async off" -ex "set remotetimeout 60" -ex "set confirm off" -ex "set height 0" -ex %3 -ex "monitor swdp_scan" -ex "attach 1" -ex "x/wx 0x8000004" -ex "monitor erase_mass" -ex "echo 0x8000004 expect 0xffffffff after erase\n" -ex "x/wx 0x8000004" -ex "file %str%" -ex "load" -ex "x/i *0x8000004" -ex "kill" -ex "tbreak main" -ex "run" -ex "detach" -ex "echo \n\n\n%6 uploaded!\n" -ex "quit" 
