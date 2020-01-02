REM
REM Variables
REM
set cwd=%cd%
set botan=%cwd%\botan-2.12.1
set distdir=%cwd%\..\hbsrecover\third-party\build
REM
REM Script
REM
mkdir %distdir%
cd %botan%
python.exe configure.py --cc=gcc --os=mingw --prefix=%distdir%
mingw32-make.exe -j4
mingw32-make.exe install
cd %pwd%
