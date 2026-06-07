@SETLOCAL 

@SET PROJECTDIR=%~1
@SET PROJECTNAME=%~2
@SET TARGETPATH=%~3
@SET XSIWORKGROUPPATH=%~4

@SET DEST=%XSIWORKGROUPPATH%\Addons\GamebryoMaterial

if not exist "%DEST%\Application\bin\nt-x86" mkdir "%DEST%\Application\bin\nt-x86"
copy "%TARGETPATH%" "%DEST%\Application\bin\nt-x86"

if not exist "%DEST%\Application\spdl" mkdir "%DEST%\Application\spdl"
copy "%PROJECTDIR%..\..\%PROJECTNAME%\src\*.spdl" "%DEST%\Application\spdl"

if not exist "%DEST%\Data\DSPresets\Shaders\Realtime" mkdir "%DEST%\Data\DSPresets\Shaders\Realtime"
copy "%PROJECTDIR%..\..\%PROJECTNAME%\src\*.Preset" "%DEST%\Data\DSPresets\Shaders\Realtime"

copy "%PROJECTDIR%..\..\%PROJECTNAME%\add-on resources\*.*" "%DEST%\"

@if not exist "%DEST%\Data\Scripts" mkdir "%DEST%\Data\Scripts"
copy "%PROJECTDIR%..\..\%PROJECTNAME%\src\*.vbs" "%DEST%\Data\Scripts"

@if not exist "%DEST%\Application\toolbars" mkdir "%DEST%\Application\toolbars"
copy "%PROJECTDIR%..\..\%PROJECTNAME%\src\*.xsitb" "%DEST%\Application\toolbars"

@ENDLOCAL
