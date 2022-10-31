#!/bin/sh

projectdir=$(dirname $0)
targetdir=$PWD

echo "Starting CMake..."
echo "Note: Fetching dependencies the first time can take a while."

mkdir -p $projectdir/build
cd $projectdir/build

cmake ..

if [[ $1 == "--release" ]]; then
    cmake --build . -j 8 --config Release
else
    cmake --build . -j 8 --config Debug
fi

cd $targetdir

echo "Done! Copying into RType subfolder."

cp -Rv $projectdir/build/target RType
echo ""

echo "Generating the scripts..."
echo $'#!/bin/sh\npath=$(dirname $0);$path/Client/binary/r-type_client' > RType/run-client.sh
echo $'#!/bin/sh\npath=$(dirname $0);$path/Server/binary/r-type_server' > RType/run-server.sh
chmod +x RType/run-server.sh
chmod +x RType/run-client.sh

tar -czvf r-type_unix.tar.gz RType

echo ""
echo "The game is into the RType subfolder."
