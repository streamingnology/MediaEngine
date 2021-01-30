::1. unzip Bento4-1.5.1-629.tar.gz to Bento4-1.5.1-629
::2. open Bento4.sln in Bento4-1.5.1-629\Build\Targets\x86_64-microsoft-win32-vs2015
::3. change from MTd to MDd and MT to MD in the setting and build x64
::4. after build, run this script

set CURDIR=%CD%

set INSTALL_DIR_DEBUG=%CURDIR%\..\build\bento4\Debug
set INSTALL_DIR_RELEASE=%CURDIR%\..\build\bento4\Release
set AP4_ROOT=%CURDIR%\Bento4-1.5.1-629
set SOURCE_ROOT=%AP4_ROOT%\Source\C++
set BUILD_TARGET_DIR=%AP4_ROOT%\Build\Targets\x86_64-microsoft-win32-vs2015\x64

MD %INSTALL_DIR_DEBUG%\include
MD %INSTALL_DIR_DEBUG%\lib
MD %INSTALL_DIR_RELEASE%\include
MD %INSTALL_DIR_RELEASE%\lib
XCOPY /Y /e /s %SOURCE_ROOT% %INSTALL_DIR_DEBUG%\include
XCOPY /Y /e /s %SOURCE_ROOT% %INSTALL_DIR_RELEASE%\include 
XCOPY /Y %BUILD_TARGET_DIR%\Debug\*.* %INSTALL_DIR_DEBUG%\lib
XCOPY /Y %BUILD_TARGET_DIR%\Release\*.* %INSTALL_DIR_RELEASE%\lib
