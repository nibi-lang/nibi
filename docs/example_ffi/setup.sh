cd math_lib
mkdir build && cd build
cmake ../
make -j4
mv math.lib ../../
cd ../
rm -rf build
cd ..

