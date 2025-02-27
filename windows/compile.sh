mkdir build
rm -r build/*

git pull

version=$(git describe --tags --abbrev=0)
## version contain the version of the last tag on github

#/mingw64/bin/qmake writernote.pro DEFINES+="VERSION_SNAPCRAFT=$version" DEFINES+=QT_NO_DEBUG_OUTPUT
#make release -j 2

mkdir build
cd build
cmake .. \
    -DPDFSUPPORT=ON \
    -DVERSION_SNAPCRAFT=$version

ninja

cd ..

echo "INFO WRITERNOTE: Add library"
ldd ./build/writernote.exe | grep '\/mingw.*\.dll' -o | sort -u | xargs -I{} cp "{}" ./build

echo "INFO WRITERNOTE: exec windeployqt"
/mingw64/bin/windeployqt-qt6.exe ./build/writernote.exe --force

echo "INFO WRITERNOTE: Build updater"
cd updater
rm -r build
mkdir build

cd build
cmake ..
ninja
cd ..

cd build
rm -r CMakeFiles
rm .ninja_deps .ninja_log build.ninja CMakeCache.txt cmake_install.cmake
cd ..

#/mingw64/bin/qmake updater.pro
#make release -j 2
ldd ./build/updater.exe | grep '\/mingw.*\.dll' -o | sort -u | xargs -I{} cp "{}" ./build
ldd ./build/updater.exe | grep '\/mingw.*\.dll' -o | sort -u | xargs -I{} cp "{}" ./build
#/mingw64/bin/windeployqt.exe build/updater.exe --force


