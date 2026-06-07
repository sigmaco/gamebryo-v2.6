@echo off
set OLDPATH=%PATH%;
set PATH=..\..\ThirdPartyCode\Xerces\xerces-c-windows_2000-msvc_60\bin
..\..\ThirdPartyCode\Xalan\Xalan-C_1_10_0-win32-msvc_60\bin\XalanTransform %1 nsf.xsl %2

set PATH=%OLDPATH%;
set OLDPATH=