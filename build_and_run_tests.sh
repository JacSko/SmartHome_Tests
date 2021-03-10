if [[ ! -d "build/" ]]
then
    echo "creating build directory"
    mkdir build
fi

cd build

cmake ..

make

ctest --output-on-failure --schedule-random --timeout 6000
