::
:: %1 is the location of the project
:: %2 is the path to where the max plugin will be copied
::
@SETLOCAL 
@SET PROJECTDIR=%~1
@SET MAXPATH=%~2

@ECHO.**************************************************
@ECHO.Removing any previous existance of the Max Plugin
@ECHO.**************************************************

@IF EXIST "%MAXPATH%\plugins\MaxImmerse" (
    @ECHO.Clearing out preexisting content from %MAXPATH%\plugins\MaxImmerse
    @DEL /F /S /Q "%MAXPATH%\plugins\MaxImmerse"
)

@IF EXIST "%MAXPATH%\plugins\MaxImmerse\Scripts" (
    @ECHO.Clearing out preexisting content from "%MAXPATH%\plugins\MaxImmerse\Scripts"
    @DEL /F /S /Q "%MAXPATH%\plugins\MaxImmerse\Scripts"
)

@IF EXIST "%MAXPATH%\scripts\Gamebryo" (
    @ECHO.Removing previous script files in %MAXPATH%\scripts\Gamebryo
    @DEL /F /S /Q "%MAXPATH%\scripts\Gamebryo"
)

@IF EXIST "%MAXPATH%\plugins\GamebryoMaxPlugin.dlu" (
    @ECHO.Removing the GamebryoMaxPlugin.dlu file from %MAXPATH%\plugins\                   
    @DEL /F /S /Q "%MAXPATH%\plugins\GamebryoMaxPlugin.dlu"
)

@ENDLOCAL
