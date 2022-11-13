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
rm -rf RType r-type_unix.tar.gz r-type_unix.zip RType-uncompressed

echo "Done! Copying into RType subfolder."

if [[ $1 == "--release" ]]; then
    cp -Rv $projectdir/build/target RType-uncompressed
fi

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
        #ffmpeg -i "$file" -c:a libvorbis -ab 32k -ar 22050 "$file-compressed.ogg"
        #mv "$file-compressed.ogg" "$file"
    done
fi

echo ""

echo "Generating the scripts..."
echo $'#!/bin/sh\npath=$(dirname $0);$path/Client/binary/r-type_client' > RType/run-client.sh
echo $'#!/bin/sh\npath=$(dirname $0);$path/Server/binary/r-type_server' > RType/run-server.sh
chmod +x RType/run-server.sh
chmod +x RType/run-client.sh

if [[ $1 == "--release" ]]; then
    echo $'#!/bin/sh\npath=$(dirname $0);$path/Client/binary/r-type_client' > RType-uncompressed/run-client.sh
    echo $'#!/bin/sh\npath=$(dirname $0);$path/Client/binary/r-type_client' > RType-uncompressed/run-server.sh
    chmod +x RType-uncompressed/run-server.sh
    chmod +x RType-uncompressed/run-client.sh
fi

tar -czvf r-type_unix.tar.gz RType
zip -r r-type_unix.zip RType

if [[ $1 == "--release" ]]; then
    echo ""
    uncompressed_size=$(du -sh RType-uncompressed | cut -f1 | sed 's/M*//g')
    compressed_size=$(du -sh RType | cut -f1 | sed 's/M*//g')
    zip_size=$(du -sh r-type_unix.zip | cut -f1 | sed 's/M*//g')
    tar_size=$(du -sh r-type_unix.tar.gz | cut -f1 | sed 's/M*//g')
    compressed_rate=$(echo "scale=2; 100 - $compressed_size / $uncompressed_size * 100" | bc)
    zip_rate=$(echo "scale=2; 100 - $zip_size / $uncompressed_size * 100" | bc)
    tar_rate=$(echo "scale=2; 100 - $tar_size / $uncompressed_size * 100" | bc)

    echo "Uncompressed build size: $uncompressed_size M"
    echo "Compressed build size: $compressed_size M"
    echo "Compressed build size (tar.gz): $tar_size M"
    echo "Compressed build size (zip): $zip_size M"
    echo "Benchmark: compressed($compressed_rate%) / tar.gz($tar_rate%) / zip($zip_rate%)"
fi

echo ""
echo "The game is into the RType subfolder."
