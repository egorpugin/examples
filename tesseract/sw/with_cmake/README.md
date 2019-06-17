# Tesseract Example (with CMake)

## Building

### Windows

Prerequisites:

1. Download cmake and add to PATH
1. Download the latest SW (https://software-network.org/) client from https://software-network.org/client/
1. Add sw to PATH.
1. On first run call `sw setup` to perform inital system integration.

Run:

```
git clone https://github.com/egorpugin/examples tesseract_example
cd tesseract/sw/with_cmake
mkdir build && cd build
cmake ..
cmake --build .
```

### Linux

Prerequisites:

1. Install cmake, the latest SW (https://software-network.org/) client.
1. Add sw to PATH.
1. On first run call `sw setup` to perform inital system integration.

Run:

```
git clone https://github.com/egorpugin/examples tesseract_example
cd tesseract/sw/with_cmake
mkdir build && cd build
cmake ..
make -j4
```

## Testing

1. Download tesseract english data to `tessdata` dir near the `main` binary.
2. Copy test image `phototest.tif` to working dir.
3. Run ``main phototest.tif``
