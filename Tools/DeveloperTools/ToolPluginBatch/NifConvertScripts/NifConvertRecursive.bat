@echo off

setlocal

:: Batch file for recursively upgrading older NIF files to new version.
::
:: Run this batch file and pass it an argument of the directory
:: you would like to convert, i.e. NifConvertRecursive.bat C:\ArtAssets

if "%1"=="" goto labelUsage

:: If directory passed, use that one:
set PROCESSDIR="%~1"
if not exist "%PROCESSDIR%" goto labelBadDirectory

:: Suffix to add to all converted nifs
set SUFFIX=_converted
set NIFCONVERT=%EGB_PATH%\Tools\DeveloperTools\ToolPluginBatch\NifConvertScripts\NifConvert.bat

pushd %PROCESSDIR%

for /R %%f in (*.nif) DO call :labelConvert "%%~ff"

popd

goto labelExit

:labelConvert
set ROOT=%~dpn1
set INFILE=%ROOT%.nif
SET OUTFILE=%ROOT%%SUFFIX%.nif
echo "%INFILE%" | find /I "%SUFFIX%" 1>NUL
if "%ERRORLEVEL%"=="1" %NIFCONVERT% "%INFILE%" "%OUTFILE%"

goto :EOF

:labelUsage
echo Usage: %~nx0 (directory name)
goto labelExit

:labelBadDirectory
echo %PROCESSDIR% is not a valid directory.
goto labelExit

:labelExit
echo.

endlocal
