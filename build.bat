@echo Starting CMake...
@echo Note: Fetching dependencies the first time can take a while.

@cd build

@if "%1"=="/fast" goto :fast
@if "%2"=="/fast" goto :fast

@echo Fetching dependencies...
cmake ..

:fast
@echo Building...
@if "%1"=="/release" (
    cmake --build . -j 8 --config Release
) else (
    cmake --build . -j 8 --config Debug
)

@cd ..

del .\build\target\Client\binary\*.pdb
del .\build\target\Server\binary\*.pdb
powershell Compress-Archive -Force -Path ".\build\target\Client", ".\build\target\Server" -DestinationPath .\r-type_win32.zip

@echo Generating installer...
.\setup\createsetup.bat /nopause

@if "%1"=="/nopause" goto end
pause
:end