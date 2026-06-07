@ECHO %1 is the project directory > postBuildLog.txt
@ECHO %2 is the maya installation directory. >> postBuildLog.txt
@ECHO %3 is the plugin >> postBuildLog.txt

if NOT EXIST %2\scripts\GamebryoBackup mkdir %2\scripts\GamebryoBackup
%1..\..\PostBuild\MayaMELMunger.exe -log=log.txt -maya="%~2" -install=false 
%1..\..\PostBuild\MayaMELMunger.exe -log=log.txt -maya="%~2" -install=true  
xcopy %1..\..\MEL\*.mel   %2\scripts\others /c /i /y
xcopy %1..\..\Icons\*.bmp %2\Icons          /c /i /y
xcopy %1..\..\Icons\*.xpm %2\Icons          /c /i /y
xcopy %1%3                %2\bin\plug-ins\  /c /i /y
if NOT EXIST %2\bin\plug-ins\"Gamebryo Scripts" mkdir %2\bin\plug-ins\"Gamebryo Scripts"
xcopy %1..\..\..\GamebryoScripts\*.* %2\bin\plug-ins\"Gamebryo Scripts" /c /i /y

