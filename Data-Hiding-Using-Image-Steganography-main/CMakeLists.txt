cmake_minimum_required(VERSION 3.10)
project(ImageSteganography)

set(CMAKE_CXX_STANDARD 17)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include OpenCV headers
include_directories(${OpenCV_INCLUDE_DIRS})

# Create encoder executable
add_executable(encoder Text_Encoder.cpp)
target_link_libraries(encoder ${OpenCV_LIBS})

# Create decoder executable  
add_executable(decoder Text_Decoder.cpp)
target_link_libraries(decoder ${OpenCV_LIBS})
