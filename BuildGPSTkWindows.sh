# run in projects/gpstk
cd build
cmake -DBUILD_EXT=true -DCMAKE_INSTALL_PREFIX=$HOMEDRIVE$HOMEPATH/.local/gpstkDiffProc -G"Visual Studio 14 2015 Win64" ../
cmake --build . --config release --target install
cd ..

