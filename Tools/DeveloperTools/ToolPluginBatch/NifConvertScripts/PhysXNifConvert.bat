@echo off

setlocal

:: Batch file for upgrading older NIF files to the current version --


:: Sanity check on command line arguments --
set fileIn=%~1
set fileOut=%~2
if "%fileIn%"=="" goto labelUsage
if "%fileOut%"=="" goto labelUsage

set TPB_PATH=%EGB_PATH%\Tools\DeveloperTools\ToolPluginBatch
set SCRIPT_DIR="%TPB_PATH%\ExampleScripts"
set TPB_EXE=

if exist "%TPB_PATH%\Win32\VC71\DebugDLL\ToolPluginBatch.exe" set TPB_EXE="%TPB_PATH%\Win32\VC80\DebugDLLToolPluginBatch.exe"
if exist "%TPB_PATH%\Win32\VC80\DebugDLL\ToolPluginBatch.exe" set TPB_EXE="%TPB_PATH%\Win32\VC80\DebugDLL\ToolPluginBatch.exe"
if exist "%TPB_PATH%\Win32\VC90\DebugDLL\ToolPluginBatch.exe" set TPB_EXE="%TPB_PATH%\Win32\VC90\DebugDLL\ToolPluginBatch.exe"
if exist "%TPB_PATH%\Win32\VC71\ToolPluginBatch.exe" set TPB_EXE="%TPB_PATH%\Win32\VC71\ToolPluginBatch.exe"
if exist "%TPB_PATH%\Win32\VC80\ToolPluginBatch.exe" set TPB_EXE="%TPB_PATH%\Win32\VC80\ToolPluginBatch.exe"
if exist "%TPB_PATH%\Win32\VC90\ToolPluginBatch.exe" set TPB_EXE="%TPB_PATH%\Win32\VC90\ToolPluginBatch.exe"
if not exist %TPB_EXE% goto labelNotBuilt

echo.
echo ------------ start:  %TIME% ------------
echo.
echo File In: %fileIn%
echo.
echo File Out: %fileOut%
echo.

:: Run ToolPluginBatch to convert old files to new --
%TPB_EXE% -p "%SCRIPT_DIR%\PhysXMeshProcess.script" -e "%SCRIPT_DIR%\SimpleExport.script" "%fileIn%" "%fileOut%"

echo.
echo ------------- end:  %TIME% -------------
echo.

goto labelExit

:labelNotBuilt
echo Could not find built ToolPluginBatch.exe in "%TPB_PATH%".  Aborting.
goto labelExit

:labelUsage
echo Usage: %~nx0 (old filename).nif (new filename).nif
goto labelExit


:labelExit
echo.

endlocal
