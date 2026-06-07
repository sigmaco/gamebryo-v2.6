@SETLOCAL 
@SET PROJECTDIR=%~1
@SET MAXPATH=%~2
@SET RADIOSITY=%~3

@IF NOT EXIST "%MAXPATH%\plugins\MaxImmerse" mkdir "%MAXPATH%\plugins\MaxImmerse"
@COPY "%PROJECTDIR%..\..\Data\*.*" "%MAXPATH%\plugins\MaxImmerse"
@IF NOT EXIST "%MAXPATH%\plugins\MaxImmerse\Scripts" mkdir "%MAXPATH%\plugins\MaxImmerse\Scripts"
@COPY "%PROJECTDIR%..\..\Scripts\*.*" "%MAXPATH%\plugins\MaxImmerse\Scripts"
@IF NOT EXIST "%MAXPATH%\scripts\Gamebryo" mkdir "%MAXPATH%\scripts\Gamebryo"
@COPY "%PROJECTDIR%..\..\MaxScripts-Helper\*.*" "%MAXPATH%\scripts\Gamebryo"
@COPY "%PROJECTDIR%..\..\MaxScripts-Startup\*.*" "%MAXPATH%\scripts\startup"
@COPY "%PROJECTDIR%..\..\MaxScripts-MacroScript Src\*.*" "%MAXPATH%\ui\macroscripts"
@COPY "%PROJECTDIR%..\..\MaxScripts-MacroScript Images\*.*" "%MAXPATH%\ui\Icons"
@COPY "%PROJECTDIR%..\..\Toolbar Definition\*.*" "%MAXPATH%\ui"
@IF NOT (%RADIOSITY%) == () COPY "%PROJECTDIR%..\..\IRadiosityMesh\*.gup" "%MAXPATH%\plugins"
		         
@ENDLOCAL
