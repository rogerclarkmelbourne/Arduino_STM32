@echo off
rem
rem 
rem
rem parameters are: 

rem Note: %~dp0 get path of this batch file, so path parameter is no longer required

cd %~dp0
java -jar maple_loader.jar %1 %2 %3 %4 %5 %6 %7 %8 %9
