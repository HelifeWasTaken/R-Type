# R-TYPE
==========
## Game Video Example:
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/t6Xz1h8TDG8/0.jpg)](https://www.youtube.com/watch?v=t6Xz1h8TDG8)

## Contributing Guidelines

    - Documentation is mandatory for all new features and bug fixes.
    - All new features and bug fixes must be covered by tests.
    - Code must be formatted with clang-format with the provided .clang-format file.
    - Commit messages must be formatted as well possible.
    - UML diagrams are mandatory for all big changes.
    - Documentation is generated with Doxygen.

Documentation about server development can be found in the [documentation](./RTYPE_DOCUMENTATION.md)
RFC can be found in the [RFC docs](./docs/rfc)
Documentation about client development can be found in the [documentation](./RTYPE_DOCUMENTATION.md)
Documentation about the game engine can be found in the [documentation](./RTYPE_DOCUMENTATION.md)

## Building

    - All dependencies must be downloaded with conan
    - The project is built with cmake
    - Minimum C++ standard is C++17
    - The project is built with g++ on Linux and MSVC on Windows

> **Note:** The binaries are built in the `${BUILD_FOLDER}/target` directory.

## Easy build (Unix)

### Debug

```bash
./build.sh
```

You can add `--fast` to skip the cmake refresh.

### Release

```bash
./build.sh --release
```

## Easy build (Windows)

### Debug

```batch
.\build.bat
```

You can add `/fast` to skip the cmake refresh.

### Release

```batch
.\build.bat /release
```

## Build the project (Unix):
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
> **Note:** The binaries are built in the `build/target` directory.

## Build the project (Windows):
Generate the Visual Studio Solution with the provided CMAKE.
