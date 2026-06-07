@SETLOCAL 

@SET PROJECTDIR=%~1
@SET PROJECTNAME=%~2
@SET TARGETPATH=%~3
@SET XSIWORKGROUPPATH=%~4

@SET DEST=%XSIWORKGROUPPATH%\Addons\GamebryoExporter

@if not exist "%DEST%\Data\Scripts" mkdir "%DEST%\Data\Scripts"
copy "%PROJECTDIR%..\..\Scripts\*.script" "%DEST%\Data\Scripts\"

@if not exist "%DEST%\Application\Plugins" mkdir "%DEST%\Application\Plugins"
copy "%TARGETPATH%" "%DEST%\Application\Plugins\"

copy "%PROJECTDIR%..\..\%PROJECTNAME%\add-on resources\*.*" "%DEST%\"

@if not exist "%DEST%\Application\toolbars" mkdir "%DEST%\Application\toolbars"
copy "%PROJECTDIR%..\..\%PROJECTNAME%\src\*.xsitb" "%DEST%\Application\toolbars"

@ENDLOCAL
