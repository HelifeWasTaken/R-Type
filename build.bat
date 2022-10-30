@echo Starting CMake...
@echo Note: Fetching dependencies the first time can take a while.

@cd build

@echo Fetching dependencies...
cmake ..

@echo Building...
cmake --build . -j 8

@cd ..

del .\build\target\Client\binary\*.pdb
del .\build\target\Server\binary\*.pdb
powershell Compress-Archive -Force -Path ".\build\target\Client", ".\build\target\Server" -DestinationPath .\r-type_win32.zip

@echo Generating installer...
.\setup\createsetup.bat /nopause

copy .\setup\Output\r-type_setup.exe .

@if "%1"=="/nopause" goto end
pause
:end