mkdir build
rm build/*

qmake writernote.pro "CONFIG-=debug" "CONFIG+=release"
make -j $nproc

ldd ./build/writernote.exe | grep '\/mingw.*\.dll' -o | sort -u | xargs -I{} cp "{}" ./build

windeployqt.exe ./build/writernote.exe --force
