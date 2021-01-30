:: unzip uv-cpp-1.5.4.tar.gz to uv-cpp-1.5.4
:: create uv-cpp-1.5.4\build
:: cd uv-cpp-1.5.4\project\vs2017, change dependent libuv's include and lib dir and name
:: build Debug and Release (x64)
:: run this script

set CURDIR=%CD%
set LIBUVCPP_DIR=%CURDIR%\uv-cpp-1.5.4
set INSTALL_DIR_DEBUG=%CURDIR%\..\uv-cpp\Debug
set INSTALL_DIR_RELEASE=%CURDIR%\..\uv-cpp\Release
set LIBUVCPP_BUILD_DIR=%LIBUVCPP_DIR%\project\vs2017\x64

MD %INSTALL_DIR_DEBUG%\include
MD %INSTALL_DIR_DEBUG%\lib
MD %INSTALL_DIR_RELEASE%\include
MD %INSTALL_DIR_RELEASE%\lib

XCOPY /Y /e /s %LIBUVCPP_DIR%\uv\include %INSTALL_DIR_DEBUG%\include
XCOPY /Y /e /s %LIBUVCPP_DIR%\uv\include %INSTALL_DIR_RELEASE%\include 
XCOPY /Y %LIBUVCPP_BUILD_DIR%\Debug\*.* %INSTALL_DIR_DEBUG%\lib
XCOPY /Y %LIBUVCPP_BUILD_DIR%\Release\*.* %INSTALL_DIR_RELEASE%\lib