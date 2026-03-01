# Diary File Merger

This is a C++ tool to merge multiple text files (diaries) into a single file. It supports automatic encoding detection and conversion to UTF-8 with BOM.

## Features

- **Recursive Search**: Finds all `.txt` files in a given directory and its subdirectories.
- **Natural Sorting**: Sorts files naturally (e.g., `1.txt`, `2.txt`, ..., `10.txt`).
- **Encoding Support**:
    - UTF-8 (with/without BOM)
    - UTF-16 LE/BE
    - ANSI (Windows system locale)
- **Output**: Merges content into `data/combined.txt` in UTF-8 BOM format.

## Directory Structure

- `src/`: Source code (`.cpp`)
- `include/`: Header files (`.h`)
- `data/`: Output directory
- `build/`: Build artifacts

## Build

You can build this project using CMake or Visual Studio.

### Using CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Usage

Run the executable and enter the directory path containing the text files when prompted.
