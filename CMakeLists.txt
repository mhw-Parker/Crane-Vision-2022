cmake_minimum_required(VERSION 3.17)
project(Crane2022)

SET(CMAKE_BUILE_TYPE DEBUG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -std=c++14 -lv4l2")
SET(BIN_NAME "run")

set(OpenCV_DIR D:\\program_files\\opencv-4.5.4\\mingw-build)
#set(OpenCV_DIR D:\\program_files\\opencv\\mingw-build)
find_package(OpenCV 4.5.4 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/Serials)

MESSAGE(STATUS "OpenCV Version: ${OpenCV_VERSION}")
message(STATUS "OpenCV PATH: ${OpenCV_LIBS_DIR}")
MESSAGE(STATUS "OpenCV Libraries: ${OpenCV_LIBS}")

FILE(GLOB_RECURSE FILE_SRC "/Serials/*.cpp")

add_executable(${BIN_NAME} main.cpp)

target_link_libraries(${BIN_NAME} ${OpenCV_LIBS})