@ECHO OFF


ECHO ************************************************************
ECHO *
ECHO * 1) Uninstall from \user\ directories
ECHO *
ECHO ************************************************************

"%XSISDK_ROOT%\..\Application\bin\xsibatch" -u GamebryoMaterial.spdl  

ECHO ************************************************************
ECHO *
ECHO * 2) Delete old files from XSI root
ECHO *
ECHO ************************************************************

del "%XSISDK_ROOT%\..\Application\bin\GamebryoShaders.dll"
del "%XSISDK_ROOT%\..\Data\DSPresets\Shaders\RealTime\GamebryoMaterial.Preset"
del "%XSISDK_ROOT%\..\Application\spdl\GamebryoMaterial.spdl" 

ECHO ************************************************************
ECHO *
ECHO * 3) Using XSIBATCH to create a preset from SPDL file...
ECHO *
ECHO ************************************************************

"%XSISDK_ROOT%\..\application\bin\xsibatch" -i GamebryoMaterial.spdl -overwrite

ECHO ************************************************************
ECHO *
ECHO * 4) Copying the files to Sofimage folders...
ECHO *
ECHO ************************************************************

rem copy "%EGB_PATH%\ToolLibs\ArtPlugins\XSIPlugin\Win32\VC80\DebugDLL\GamebryoShaders\GamebryoShaders.dll" "%XSISDK_ROOT%\..\Application\Plugins\"
copy "%EGB_PATH%\ToolLibs\ArtPlugins\XSIPlugin\Win32\VC80\DebugDLL\GamebryoShaders\GamebryoShaders.dll" "%XSISDK_ROOT%\..\Application\bin\"
copy "%USERPROFILE%\Softimage\XSI_6.5\Data\DSPresets\Shaders\RealTime\GamebryoMaterial.Preset" "%XSISDK_ROOT%\..\Data\DSPresets\Shaders\RealTime\"
copy "%EGB_PATH%\ToolLibs\ArtPlugins\XSIPlugin\GamebryoShaders\src\GamebryoMaterial.spdl" "%XSISDK_ROOT%\..\Application\spdl\"

rem For Vista, skip next section
ver | find "6." > nul
if %ERRORLEVEL% == 0 goto exit

REM *************************************************************
REM *
REM * On Windows XP 64bits, XSI is confused about user profile: sometimes it uses XP-like paths,
REM * sometimes Vista-like paths... oh well.
REM *
REM *************************************************************

set VISTA_LIKE_PROFILE=%SystemDrive%\users\%USERNAME%
copy "%USERPROFILE%\Softimage\XSI_6.5\Data\DSPresets\Shaders\RealTime\GamebryoMaterial.Preset" "%VISTA_LIKE_PROFILE%\Softimage\XSI_6.5\Data\DSPresets\Shaders\RealTime\" 
copy "%EGB_PATH%\ToolLibs\ArtPlugins\XSIPlugin\GamebryoShaders\src\GamebryoMaterial.spdl" "%VISTA_LIKE_PROFILE%\Softimage\XSI_6.5\Application\spdl\"


:exit