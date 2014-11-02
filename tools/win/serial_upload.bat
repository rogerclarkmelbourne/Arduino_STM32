@echo off
rem
rem parameters are: 

rem Note: %~dp0 get path of this batch file, so path parameter is no longer required

cd %~dp0
rem: the two line below are needed to fix path issues with incorrect slashes before the bin file name
set str=%4
set str=%str:/=\%
stm32flash -g 0x8000000 -b 230400 -w %str% %1 





