if exist "C:\Program Files (x86)\Inno Setup 6\iscc.exe" (
    goto :create
) else (
    @echo "Did not find a global install of InnoSetup"
)

if exist "%LocalAppData%\Programs\Inno Setup 6\iscc.exe" (
    goto :create
) else (
    @echo "InnoSetup not found"
)

if exist "%TEMP%/innosetupinstaller.exe" (
    @echo "InnoSetup installer is already downloaded."
) else (
    @echo Downloading InnoSetup...
    powershell -Command "Invoke-WebRequest -Uri 'https://jrsoftware.org/download.php/is.exe' -OutFile '%TEMP%/innosetupinstaller.exe'"
)

@echo Installing InnoSetup...
"%TEMP%/innosetupinstaller.exe" /CURRENTUSER /SP /VERYSILENT

:create

@echo Creating installer...

if exist "C:\Program Files (x86)\Inno Setup 6" (
    "C:\Program Files (x86)\Inno Setup 6\iscc.exe" "%~dp0\config.iss"
) else (
    "%LocalAppData%\Programs\Inno Setup 6\iscc.exe" "%~dp0\config.iss"
)


pause