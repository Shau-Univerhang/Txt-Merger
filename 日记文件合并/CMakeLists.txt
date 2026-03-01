cmake_minimum_required(VERSION 3.10)
project(DiaryFileMerger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Add source files
set(SOURCES
    src/main.cpp
    src/EncodingUtils.cpp
    src/FileUtils.cpp
)

# Create executable
add_executable(DiaryFileMerger ${SOURCES})
