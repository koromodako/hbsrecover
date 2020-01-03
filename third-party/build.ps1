#
# Variables
#
$refd=$pwd.Path
$botan="$($refd)\botan-2.12.1"
$distdir=(Resolve-Path "$($refd)\..\hbsrecover\third-party\build").Path
#
# Script
#
mkdir $distdir
cd $botan
python.exe configure.py --cc=gcc --os=mingw --prefix=$distdir
mingw32-make.exe -j4
mingw32-make.exe install
cd $refd
