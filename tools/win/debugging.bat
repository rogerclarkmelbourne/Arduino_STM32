@Echo off
set fileName=%~n2
set BaseDir=C:\Debugging
set OutputDir=%BaseDir%\%fileName%
IF NOT EXIST %BaseDir%\*.* (echo Need to create directory %BaseDir%)
IF NOT EXIST %OutputDir%\*.* (md %OutputDir%)
set sizeFile=%fileName%.size.txt
set nmFile=%fileName%.nm.txt
set dasmFile=%fileName%.dasm.txt
set pcompFile=%fileName%.pcomp.txt
cd %1

arm-none-eabi-objdump.exe -d -S -l %2.elf > %OutputDir%\%dasmFile%

arm-none-eabi-nm.exe --format="sysv" -C -l  %2.elf > %OutputDir%\%nmFile%

arm-none-eabi-size.exe -t -A %2.elf > %OutputDir%\%sizeFile%
arm-none-eabi-size.exe -t -A -x %2.elf >> %OutputDir%\%sizeFile%
start %OutputDir%

echo Done!