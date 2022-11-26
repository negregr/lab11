root_dir=$(pwd)

if [ -d $root_dir/build ]; then
	rm -rf build
fi

mkdir build
cd build

cmake ../
make

cd $root_dir

