# Tesseract Example (with SW)

A very basic Tesseract-OCR example with C++ Archive Network building.

## Building

Prerequisites:

1. Download cmake and add to PATH
1. Download the latest SW (https://software-network.org/) client from https://software-network.org/client/
1. Add sw to PATH.
1. On first run call `sw setup` to perform inital system integration.

Run:

```

Run `sw build main.cpp` to build your simple application that uses tesseract.
```

## Testing

1. Download tesseract english data to `tessdata` dir near the `main` binary.
2. Copy test image `img/phototest.tif`
3. Run ``main phototest.tif``
