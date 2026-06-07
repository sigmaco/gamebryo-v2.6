:: %1 is the project directory
:: %2 is the maya installation directory 
:: %3 is the plugin

@SETLOCAL

@SET PROJDIR=%1
@SET MAYADIR=%2
@SET PLUGIN=%3

@ECHO.**************************************************
@ECHO.Removing any previous existance of the Maya Plugin
@ECHO.**************************************************

@IF EXIST "%MAYADIR%%PLUGIN%" (
    @ECHO.Deleting previous plugin "%MAYADIR%%PLUGIN%"
    @DEL /S /Q "%MAYADIR%%PLUGIN%"
)

@ENDLOCAL
