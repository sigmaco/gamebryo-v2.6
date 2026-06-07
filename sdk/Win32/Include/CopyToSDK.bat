@echo off
::
:: This batch file is used to copy files from a project into the
:: SDK Include directory. It also marks those files read only.
::

:: FIND THE SDK DIRECTORY
:: Prefer the most local sdk dir,
:: so order these checks such that local dirs set the variable last.
set SDK_DIR=not_found
if exist ..\..\..\..\..\sdk set SDK_DIR=..\..\..\..\..\sdk\Win32
if exist ..\..\..\..\sdk    set SDK_DIR=..\..\..\..\sdk\Win32
if exist ..\..\..\sdk       set SDK_DIR=..\..\..\sdk\Win32
if (%SDK_DIR%)==(not_found) goto :ERROR_NO_SDK

:: Create folders if they are missing
if not exist %SDK_DIR%\Include  md %SDK_DIR%\Include
if not exist %SDK_DIR%\Resource md %SDK_DIR%\Resource

echo (only copying new or modified files)

:: COPY STANDARD FILES
if exist ..\Ni*.h      xcopy /D /F /I /R /Y ..\Ni*.h      %SDK_DIR%\Include
if exist ..\Ni*.inl    xcopy /D /F /I /R /Y ..\Ni*.inl    %SDK_DIR%\Include
if exist ..\Ni*.rc     xcopy /D /F /I /R /Y ..\Ni*.rc     %SDK_DIR%\Resource
if exist ..\Ni*.hrc    xcopy /D /F /I /R /Y ..\Ni*.hrc    %SDK_DIR%\Resource
if exist ..\..\Ni*.h   xcopy /D /F /I /R /Y ..\..\Ni*.h   %SDK_DIR%\Include
if exist ..\..\Ni*.inl xcopy /D /F /I /R /Y ..\..\Ni*.inl %SDK_DIR%\Include
if exist ..\..\Ni*.rc  xcopy /D /F /I /R /Y ..\..\Ni*.rc  %SDK_DIR%\Resource
if exist ..\..\Ni*.hrc xcopy /D /F /I /R /Y ..\..\Ni*.hrc %SDK_DIR%\Resource

:: COPY USER SPECIFIED FILES
:==LOOP
if (%~1)==() goto LOOP_END
if exist "%~1" xcopy /D /F /I /R /Y "%~1" %SDK_DIR%\Include
shift
goto LOOP
:==LOOP_END

:: MARK FILES READ ONLY
if exist %SDK_DIR%\Include attrib +R %SDK_DIR%\Include\*
if exist %SDK_DIR%\Resource attrib +R %SDK_DIR%\Resource\*

exit /b



:==ERROR_NO_SDK
echo %0: fatal error: Cannot locate SDK directory.
exit 1
