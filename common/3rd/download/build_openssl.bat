rem 1. install perl
rem 2. unzip openssl-OpenSSL_1_1_1d.tar.gz to openssl-OpenSSL_1_1_1d
rem 3. comment all code in sub _warn from line 395 in Config.pm. refer here https://www.jianshu.com/p/c45c2cca1932
rem 4. call vcvars64.bat. refer here https://gist.github.com/terrillmoore/995421ea6171a9aa50552f6aa4be0998
rem 5. you can download compiled openssl from https://slproweb.com/products/Win32OpenSSL.html if you don't want to compile from scratch

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
set CURDIR=%CD%
cd %CURDIR%\openssl-OpenSSL_1_1_1d

set INSTALL_DIR_DEBUG=%CURDIR%\..\build\openssl\Debug
perl Configure --debug VC-WIN64A --prefix=%INSTALL_DIR_DEBUG%
nmake
nmake install

set INSTALL_DIR_RELEASE=%CURDIR%\..\build\openssl\Release
perl Configure --release VC-WIN64A --prefix=%INSTALL_DIR_RELEASE%
nmake
nmake install