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
stm32flash -g 0x8000000 -b 115200 -w %str% %1 
rem: C:\Python27\python.exe stm32loader.py -e -w -p %1 -g -b 115200  %str%

rem: ------------- use STM's own uploader 
rem: ---- Need to remove the COM bit from the comm port as the STM prog just wants the number
set commport=%1
set commportnum=%commport:COM=%
rem: --- The maple board may nee the -i setting to be -i STM32_Med-density_128K or STM32_Med-density_64K
rem: ---- 64 bit version
rem: "%ProgramFiles(x86)%\STMicroelectronics\Software\Flash Loader Demonstrator\STMFlashLoader.exe"  -c --pn %commportnum% --br 230400  -i STM32_High-density_256K -e --all -d --fn %str% --a 0x8000000 -r --a 0x8000000 

rem:  -- 32 bit version
rem: "%ProgramFiles%\STMicroelectronics\Software\Flash Loader Demonstrator\STMFlashLoader.exe"  -c --pn %commportnum% --br 230400  -i STM32_Med-density_64K -e --all -d --fn %str% --a 0x8000000 -r --a 0x8000000 
