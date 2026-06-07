rem %1 targetFilePath
rem %2 projectDir
rem ECHO "" > postBuildLog.txt
rem ECHO %1 >> postBuildLog.txt
rem ECHO %2 >> postBuildLog.txt

copy "%1" "%2..\..\..\MayaPlugin\PostBuild"
