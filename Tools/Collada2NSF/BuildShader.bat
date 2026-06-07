@echo off

:: Create a local environment so we can modify the path
setlocal

set PATH=%PATH%;%EGB_PATH%\ThirdPartyCode\Xerces\xerces-c-windows_2000-msvc_60\bin;%EGB_PATH%\ThirdPartyCode\Xalan\Xalan-C_1_10_0-win32-msvc_60\bin

:: NOTE: %~dp0 evaluates to drive+path of argument 0 (which is the name/way
:: the batch file was invoked, so this gets us to the same directory as
:: the batch file).

:: Generate NSF from dae via nsf.xsl
XalanTransform %2\%1.dae %~dp0\nsf.xsl %3\%1.NSF

:: Generate list of compile targets
XalanTransform %2\%1.dae %~dp0\compile.xsl %~dp0\%1_compile_list.bat

:: Compile targets
call %~dp0\%1_compile_list.bat %2 %3

:: Remove compile list
del /f %~dp0\%1_compile_list.bat

:: close out the local environment
endlocal
