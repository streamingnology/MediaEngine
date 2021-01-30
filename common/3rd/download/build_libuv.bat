:: unzip libuv-1.40.0.tar.gz to libuv-1.40.0
:: create libuv-1.40.0\build
:: cd libuv-1.40.0\build and cmake ..
:: build Debug and Release (x64)
:: copy libuv-1.40.0\build\Debug and libuv-1.40.0\build\Release to 

set CURDIR=%CD%
set LIBUV_DIR=%CURDIR%\libuv-1.40.0
set INSTALL_DIR_DEBUG=%CURDIR%\..\libuv\Debug
set INSTALL_DIR_RELEASE=%CURDIR%\..\libuv\Release

MD %INSTALL_DIR_DEBUG%\include
MD %INSTALL_DIR_DEBUG%\lib
MD %INSTALL_DIR_RELEASE%\include
MD %INSTALL_DIR_RELEASE%\lib

XCOPY /Y /e /s %LIBUV_DIR%\include %INSTALL_DIR_DEBUG%\include
XCOPY /Y /e /s %LIBUV_DIR%\include %INSTALL_DIR_RELEASE%\include 
XCOPY /Y %LIBUV_DIR%\build\Debug\*.* %INSTALL_DIR_DEBUG%\lib
XCOPY /Y %LIBUV_DIR%\build\Release\*.* %INSTALL_DIR_RELEASE%\lib