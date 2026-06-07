@REM UpdateArtistTools.bat

@SETLOCAL

@REM ------------------------------------------------------------------------
@REM 
@REM These scripts allow an artist to pull the most recent version of the
@REM Gamebryo libraries, executables, shaders, and plugins onto his
@REM machine from a built version, possibly made available on the network
@REM by a programmer who performed the build. 
@REM
@REM These scripts are intended to update an existing installation, not
@REM necessarily to perform a new installation. The environment variables
@REM and paths that the art tools require will not be set up - those will
@REM still require the original Gamebryo installer to be run. But these
@REM scripts will make it easier for artists to use the rebuilt version 
@REM of the engine after programmers make changes to the code.
@REM
@REM Before these scripts can be run, a rebuilt version of the Gamebryo tree
@REM must be visible to the artist, usually via a network share. The artist
@REM will also need to have the environment variables in the accompanying 
@REM "ArtistToolOptions.bat" script set up correctly to point to the
@REM location of the built version of Gamebryo, the local installation of
@REM Gamebryo that needs to be updated, and the location of any 3ds max or
@REM Maya installations that will need their exporters updated.
@REM
@REM Note that these scripts currently assume that VC7.1 was used to 
@REM initially install the art tool plugins, and to rebuild them. 
@REM To change the version of VC that this script will use to look for
@REM builds of the art tool plugins, change this variable accordingly:

@IF NOT DEFINED EGB_VCVERS (
@SET EGB_VCVERS=VC80
)

@REM ------------------------------------------------------------------------
@REM 
@REM Location of Gamebryo Tree
@REM 
@REM This variable describes the location of a built version of Gamebryo.
@REM It can be located locally or on a network. The scripts will update
@REM the local machine using the built libraries, executables, and shaders
@REM from this Gamebryo tree.

@IF NOT DEFINED GAMEBRYO_DEPOT (
@SET GAMEBRYO_DEPOT=\\NetworkShare\GamebryoDirectory\
)

@REM ------------------------------------------------------------------------

@IF NOT DEFINED GAMEBRYO_DEPOT (
@ECHO **Location of built Gamebryo tree not defined!
) & GOTO :DepotVarError

@IF NOT EXIST "%GAMEBRYO_DEPOT%" (
@ECHO **Cannot find location of built Gamebryo tree: "%GAMEBRYO_DEPOT%"
) & GOTO :DepotVarError
@ECHO Location of built Gamebryo tree: %GAMEBRYO_DEPOT%

@IF NOT DEFINED EGB_PATH (
@ECHO **Location of local Gamebryo installation not defined!
) & GOTO :LocalVarError

@IF NOT EXIST "%EGB_PATH%" (
@ECHO **Cannot find location of local Gamebryo installation: "%EGB_PATH%"
) & @GOTO :LocalVarError
@ECHO Location of local Gamebryo installation to be updated: %EGB_PATH%

@IF NOT DEFINED MAXINSTALLPATH90 GOTO :3dsmax90CheckDone
@IF NOT EXIST "%MAXINSTALLPATH90%" (
@ECHO **Cannot find location of 3ds max 9 installation: %MAXINSTALLPATH90%
) & (
@ECHO If no 3ds Max 9 installation exists, do not set the MAXINSTALLPATH90 variable.
) & (
@GOTO :EXIT
) ELSE (
@ECHO Location of 3ds Max 9 installation to be updated: %MAXINSTALLPATH90%
)
:3dsmax90CheckDone

@IF NOT DEFINED MAXINSTALLPATH2008 GOTO :3dsmax2008CheckDone
@IF NOT EXIST "%MAXINSTALLPATH2008%" (
@ECHO **Cannot find location of 3ds max 2008 installation: %MAXINSTALLPATH2008%
) & (
@ECHO If no 3ds Max 2008 installation exists, do not set the MAXINSTALLPATH2008 variable.
) & (
@GOTO :EXIT
) ELSE (
@ECHO Location of 3ds Max 2008 installation to be updated: %MAXINSTALLPATH2008%
)
:3dsmax2008CheckDone

@IF NOT DEFINED MAXINSTALLPATH2009 GOTO :3dsmax2009CheckDone
@IF NOT EXIST "%MAXINSTALLPATH2009%" (
@ECHO **Cannot find location of 3ds max 2009 installation: %MAXINSTALLPATH2009%
) & (
@ECHO If no 3ds Max 2009 installation exists, do not set the MAXINSTALLPATH2009 variable.
) & (
@GOTO :EXIT
) ELSE (
@ECHO Location of 3ds Max 2009 installation to be updated: %MAXINSTALLPATH2009%
)
:3dsmax2009CheckDone

@IF NOT DEFINED MAYAINSTALLPATH70 GOTO :MAYA70CheckDone
@IF NOT EXIST "%MAYAINSTALLPATH70%" (
@ECHO **Cannot find location of Maya 7.0 installation: %MAYAINSTALLPATH70%
) & (
@ECHO If no Maya 7.0 installation exists, do not set the MAYAINSTALLPATH70 variable in ArtistToolOptions.bat.
) & (
@GOTO :EXIT
) ELSE (
@ECHO Location of Maya 7.0 installation to be updated: %MAYAINSTALLPATH70%
)
:Maya70CheckDone

@IF NOT DEFINED MAYAINSTALLPATH80 GOTO :MAYA80CheckDone
@IF NOT EXIST "%MAYAINSTALLPATH80%" (
@ECHO **Cannot find location of Maya 8.0 installation: %MAYAINSTALLPATH80%
) & (
@ECHO If no Maya 8.0 installation exists, do not set the MAYAINSTALLPATH80 variable in ArtistToolOptions.bat.
) & (
@GOTO :EXIT
) ELSE (
@ECHO Location of Maya 8.0 installation to be updated: %MAYAINSTALLPATH80%
)
:Maya80CheckDone

@IF NOT DEFINED MAYAINSTALLPATH85 GOTO :MAYA85CheckDone
@IF NOT EXIST "%MAYAINSTALLPATH85%" (
@ECHO **Cannot find location of Maya 8.5 installation: %MAYAINSTALLPATH85%
) & (
@ECHO If no Maya 8.5 installation exists, do not set the MAYAINSTALLPATH85 variable in ArtistToolOptions.bat.
) & (
@GOTO :EXIT
) ELSE (
@ECHO Location of Maya 8.5 installation to be updated: %MAYAINSTALLPATH85%
)
:Maya85CheckDone

@IF NOT DEFINED MAYAINSTALLPATH2008 GOTO :Maya2008CheckDone
@IF NOT EXIST "%MAYAINSTALLPATH2008%" (
@ECHO **Cannot find location of Maya 2008 installation: %MAYAINSTALLPATH2008%
) & (
@ECHO If no Maya 2008 installation exists, do not set the MAYAINSTALLPATH2008 variable in ArtistToolOptions.bat.
) & (
@GOTO :EXIT
) ELSE (
@ECHO Location of Maya 2008 installation to be updated: %MAYAINSTALLPATH2008%
)
:Maya2008CheckDone

@REM ------------------------------------------------------------------------
@REM Update the local Gamebryo SDK installation
@ECHO -----

@IF NOT EXIST "%GAMEBRYO_DEPOT%\SDK\Win32\DLL\" (
@ECHO **Location %GAMEBRYO_DEPOT%\SDK\Win32\DLL\ not found!
) & @GOTO :DLLCopyDone
@ECHO Copying DLL Directory...
@XCOPY "%GAMEBRYO_DEPOT%\SDK\Win32\DLL" "%EGB_PATH%\SDK\Win32\DLL" /c /q /i /s /y
:DLLCopyDone

@IF NOT EXIST "%GAMEBRYO_DEPOT%\SDK\Win32\ToolPlugins\" (
@ECHO **Location %GAMEBRYO_DEPOT%\SDK\Win32\ToolPlugins\ not found!
) & @GOTO :ToolPluginsCopyDone
@ECHO Copying ToolPlugins Directory...
@XCOPY "%GAMEBRYO_DEPOT%\SDK\Win32\ToolPlugins" "%EGB_PATH%\SDK\Win32\ToolPlugins" /c /q /i /s /y
:ToolPluginsCopyDone

@IF NOT EXIST "%GAMEBRYO_DEPOT%\SDK\Win32\Shaders\" (
@ECHO **Location %GAMEBRYO_DEPOT%\SDK\Win32\Shaders\ not found!
) & @GOTO :ShadersCopyDone
@ECHO Copying Shaders Directory...
@XCOPY "%GAMEBRYO_DEPOT%\SDK\Win32\Shaders" "%EGB_PATH%\SDK\Win32\Shaders" /c /q /i /s /y
:ShadersCopyDone

@IF NOT EXIST "%GAMEBRYO_DEPOT%\Tools\AnimationTool\Application\" (
@ECHO **Location %GAMEBRYO_DEPOT%\Tools\AnimationTool\Application\ not found!
) & @GOTO :AnimationToolCopyDone
@ECHO Copying AnimationTool Application Directory...
@XCOPY "%GAMEBRYO_DEPOT%\Tools\AnimationTool\Application" "%EGB_PATH%\Tools\AnimationTool\Application" /c /q /i /s /y
:AnimationToolCopyDone

@IF NOT EXIST "%GAMEBRYO_DEPOT%\Tools\AssetViewer\Application\" (
@ECHO **Location %GAMEBRYO_DEPOT%\Tools\AssetViewer\Application\ not found!
) & @GOTO :AssetViewerCopyDone
@ECHO Copying AssetViewer Application Directory...
@XCOPY "%GAMEBRYO_DEPOT%\Tools\AssetViewer\Application" "%EGB_PATH%\Tools\AssetViewer\Application" /c /q /i /s /y
:AssetViewerCopyDone

@IF NOT EXIST "%GAMEBRYO_DEPOT%\Tools\SceneDesigner\Application\" (
@ECHO **Location %GAMEBRYO_DEPOT%\Tools\SceneDesigner\Application\ not found!
) & GOTO :SceneDesignerCopyDone
@ECHO Copying SceneDesigner Application Directory...
@XCOPY "%GAMEBRYO_DEPOT%\Tools\SceneDesigner\Application" "%EGB_PATH%\Tools\SceneDesigner\Application" /c /q /i /s /y
:SceneDesignerCopyDone

@REM ------------------------------------------------------------------------
@REM Copy over the PhysXNifViewer
@ECHO -----
@IF NOT EXIST "%GAMEBRYO_DEPOT%\Tools\DeveloperTools\PhysXNifViewer\Win32\" (
@ECHO **Location %GAMEBRYO_DEPOT%\Tools\DeveloperTools\PhysXNifViewer\Win32\ not found!
) & GOTO :PhysXNifViewerCopyDone
@ECHO Copying PhysXNifViewer...
@XCOPY "%GAMEBRYO_DEPOT%\Tools\DeveloperTools\PhysXNifViewer\Win32\PhysXNifViewer.exe" "%EGB_PATH%\Tools\DeveloperTools\PhysXNifViewer\Win32\" /c /q /i /s /y
:PhysXNifViewerCopyDone

@REM ------------------------------------------------------------------------
@REM Update the local exporters
@ECHO -----

@IF NOT DEFINED MAXINSTALLPATH90 GOTO :3dsmax90CopyDone
@IF NOT EXIST "%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MaxPlugin\" (
@ECHO **Location %GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MaxPlugin\ not found!
) & @GOTO :3dsmax90CopyDone

@ECHO Copying 3ds max 9.0 plugin...
@SET GBMAXDIR=%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MaxPlugin\
@XCOPY "%GBMAXDIR%\Win32\%EGB_VCVERS%\Shipping_Max90\GamebryoMaxPlugin.dlu" "%MAXINSTALLPATH90%\plugins\" /c /q /i /y
@XCOPY "%GBMAXDIR%\Data\*.*" "%MAXINSTALLPATH90%\plugins\MaxImmerse\" /c /q /i /y
@XCOPY "%GBMAXDIR%\Scripts\*.*" "%MAXINSTALLPATH90%\plugins\MaxImmerse\Scripts\" /c /q /i /y
@XCOPY "%GBMAXDIR%\MaxScripts-Helper\*.*" "%MAXINSTALLPATH90%\scripts\Gamebryo\" /c /q /i /y
@XCOPY "%GBMAXDIR%\MaxScripts-Startup\*.*" "%MAXINSTALLPATH90%\scripts\startup\" /c /q /i /y
@XCOPY "%GBMAXDIR%\MaxScripts-MacroScript Src\*.*" "%MAXINSTALLPATH90%\ui\macroscripts\" /c /q /i /y
@XCOPY "%GBMAXDIR%\MaxScripts-MacroScript Images\*.*" "%MAXINSTALLPATH90%\ui\Icons\" /c /q /i /y
@XCOPY "%GBMAXDIR%\Toolbar Definition\*.*" "%MAXINSTALLPATH90%\ui\" /c /q /i /y
:3dsmax90CopyDone

@IF NOT DEFINED MAXINSTALLPATH2008 GOTO :3dsmax2008CopyDone
@IF NOT EXIST "%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MaxPlugin\" (
@ECHO **Location %GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MaxPlugin\ not found!
) & @GOTO :3dsmax2008CopyDone

@ECHO Copying 3ds Max 2008 plugin...
@SET GBMAXDIR=%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MaxPlugin\
@XCOPY "%GBMAXDIR%\Win32\%EGB_VCVERS%\Shipping_Max2008\GamebryoMaxPlugin.dlu" "%MAXINSTALLPATH2008%\plugins\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\Data\*.*" "%MAXINSTALLPATH2008%\plugins\MaxImmerse\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\Scripts\*.*" "%MAXINSTALLPATH2008%\plugins\MaxImmerse\Scripts\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\MaxScripts-Helper\*.*" "%MAXINSTALLPATH2008%\scripts\Gamebryo\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\MaxScripts-Startup\*.*" "%MAXINSTALLPATH2008%\scripts\startup\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\MaxScripts-MacroScript Src\*.*" "%MAXINSTALLPATH2008%\ui\macroscripts\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\MaxScripts-MacroScript Images\*.*" "%MAXINSTALLPATH2008%\ui\Icons\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\Toolbar Definition\*.*" "%MAXINSTALLPATH2008%\ui\" /c /q /d /i /y
:3dsmax2008CopyDone

@IF NOT DEFINED MAXINSTALLPATH2009 GOTO :3dsmax2009CopyDone
@IF NOT EXIST "%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MaxPlugin\" (
@ECHO **Location %GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MaxPlugin\ not found!
) & @GOTO :3dsmax2009CopyDone

@ECHO Copying 3ds Max 2009 plugin...
@SET GBMAXDIR=%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MaxPlugin\
@XCOPY "%GBMAXDIR%\Win32\%EGB_VCVERS%\Shipping_Max2009\GamebryoMaxPlugin.dlu" "%MAXINSTALLPATH2009%\plugins\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\Data\*.*" "%MAXINSTALLPATH2009%\plugins\MaxImmerse\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\Scripts\*.*" "%MAXINSTALLPATH2009%\plugins\MaxImmerse\Scripts\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\MaxScripts-Helper\*.*" "%MAXINSTALLPATH2009%\scripts\Gamebryo\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\MaxScripts-Startup\*.*" "%MAXINSTALLPATH2009%\scripts\startup\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\MaxScripts-MacroScript Src\*.*" "%MAXINSTALLPATH2009%\ui\macroscripts\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\MaxScripts-MacroScript Images\*.*" "%MAXINSTALLPATH2009%\ui\Icons\" /c /q /d /i /y
@XCOPY "%GBMAXDIR%\Toolbar Definition\*.*" "%MAXINSTALLPATH2009%\ui\" /c /q /d /i /y
:3dsmax2009CopyDone

@IF NOT DEFINED MAYAINSTALLPATH70 GOTO :MAYA70CopyDone
@IF NOT EXIST "%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MayaPlugin\" (
@ECHO **Location %GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MayaPlugin\ not found!
) & @GOTO :Maya70CopyDone

@ECHO Copying Maya 7.0 plugin...
@SET GBMAYADIR=%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MayaPlugin\
@CALL "%GBMAYADIR%\MayaPlugin\PostBuild\PostBuildScript.bat" "%GBMAYADIR%\MayaPlugin\Win32\%EGB_VCVERS%\" "%MAYAINSTALLPATH70%" MyImmerse70.mll
:Maya70CopyDone

@IF NOT DEFINED MAYAINSTALLPATH80 GOTO :MAYA80CopyDone
@IF NOT EXIST "%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MayaPlugin\" (
@ECHO **Location %GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MayaPlugin\ not found!
) & @GOTO :Maya80CopyDone

@ECHO Copying Maya 8.0 plugin...
@SET GBMAYADIR=%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MayaPlugin\
@CALL "%GBMAYADIR%\MayaPlugin\PostBuild\PostBuildScript.bat" "%GBMAYADIR%\MayaPlugin\Win32\%EGB_VCVERS%\" "%MAYAINSTALLPATH80%" MyImmerse80.mll
:Maya80CopyDone

@IF NOT DEFINED MAYAINSTALLPATH85 GOTO :MAYA85CopyDone
@IF NOT EXIST "%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MayaPlugin\" (
@ECHO **Location %GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MayaPlugin\ not found!
) & @GOTO :Maya85CopyDone

@ECHO Copying Maya 8.5 plugin...
@SET GBMAYADIR=%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MayaPlugin\
@CALL "%GBMAYADIR%\MayaPlugin\PostBuild\PostBuildScript.bat" "%GBMAYADIR%\MayaPlugin\Win32\%EGB_VCVERS%\" "%MAYAINSTALLPATH85%" MyImmerse85.mll
:Maya85CopyDone

@IF NOT DEFINED MAYAINSTALLPATH2008 GOTO :MAYA2008CopyDone
@IF NOT EXIST "%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MayaPlugin\" (
@ECHO **Location %GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MayaPlugin\ not found!
) & @GOTO :Maya2008CopyDone

@ECHO Copying Maya 2008 plugin...
@SET GBMAYADIR=%GAMEBRYO_DEPOT%\ToolLibs\ArtPlugins\MayaPlugin\
@CALL "%GBMAYADIR%\MayaPlugin\PostBuild\PostBuildScript.bat" "%GBMAYADIR%\MayaPlugin\Win32\%EGB_VCVERS%\" "%MAYAINSTALLPATH2008%" MyImmerse2008.mll
:Maya2008CopyDone

@REM ------------------------------------------------------------------------
@REM Done!
@ECHO Update complete.
@GOTO :Exit

@REM ------------------------------------------------------------------------

:DepotVarError
@ECHO Be sure to define GAMEBRYO_DEPOT correctly in ArtistToolOptions.bat.
@GOTO :Exit

:LocalVarError
@ECHO Be sure to define EGB_PATH correctly in ArtistToolOptions.bat.
@GOTO :Exit

:Exit
@PAUSE
@ENDLOCAL
