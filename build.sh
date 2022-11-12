#!/bin/sh

projectdir=$(dirname $0)
targetdir=$PWD

echo "Starting CMake..."
echo "Note: Fetching dependencies the first time can take a while."

mkdir -p $projectdir/build
cd $projectdir/build

if [[ $1 != "--fast" ]] && [[ $2 != "--fast" ]]; then
    cmake ..
fi

if [[ $1 == "--release" ]]; then
    cmake --build . --config Release
else
    cmake --build . -j 8 --config Debug
fi

cd $targetdir

echo "Cleaning old build"
rm -rf RType r-type_unix.tar.gz

echo "Done! Copying into RType subfolder."

cp -Rv $projectdir/build/target RType

if [[ $1 == "--release" ]]; then
    strip RType/Client/binary/r-type_client
    strip RType/Server/binary/r-type_server

    echo "Minifying JSON files..."
    for file in $(find ./RType/ -type f -name "*.json" -o -name "*.conf"); do
        echo "Minifying $file"
        cat "$file" | jq --compact-output > "$file-compressed"
        cat "$file-compressed" > "$file"
        rm -f "$file-compressed"
    done

    echo "Compressing audio..."
    for file in $(find ./RType/ -type f -name "*.ogg"); do
        echo "Compressing $file"
        ffmpeg -i "$file" -c:a libvorbis -ab 32k -ar 22050 "$file-compressed.ogg"
        mv "$file-compressed.ogg" "$file"
    done
fi

echo ""

echo "Generating the scripts..."
echo $'#!/bin/sh\npath=$(dirname $0);$path/Client/binary/r-type_client' > RType/run-client.sh
echo $'#!/bin/sh\npath=$(dirname $0);$path/Server/binary/r-type_server' > RType/run-server.sh
chmod +x RType/run-server.sh
chmod +x RType/run-client.sh

tar -czvf r-type_unix.tar.gz RType

echo ""
echo "The game is into the RType subfolder."
