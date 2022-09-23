# R-TYPE

## Contributing Guidelines

    - Documentation is mandatory for all new features and bug fixes.
    - All new features and bug fixes must be covered by tests.
    - Code must be formatted with clang-format with the provided .clang-format file.
    - Commit messages must be formatted as well possible.
    - UML diagrams are mandatory for all big changes.
    - Documentation is generated with Doxygen.

Documentation about server development (+RFC) can be found in the [wiki]()
Documentation about client development can be found in the [wiki]()
Documentation about the game engine can be found in the [wiki]()

## Building

    - All dependencies must be downloaded with conan
    - The project is built with cmake
    - Minimum C++ standard is C++17
    - The project is built with g++ on Linux and MSVC on Windows

## Build the project (Unix):
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Build the project (Windows):
Generate the Visual Studio Solution with the provided CMAKE.