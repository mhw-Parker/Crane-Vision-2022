cmake_minimum_required(VERSION 3.17)
project(Crane2022)

SET(CMAKE_BUILd_TYPE DEBUG)
SET(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -std=c++17 -pthread")
SET(BIN_NAME "run")

#set(OpenCV_DIR D:\\program_files\\opencv\\mingw-build)
find_package(OpenCV REQUIRED)
find_package(Threads)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

MESSAGE(STATUS "OpenCV Version: ${OpenCV_VERSION}")
message(STATUS "OpenCV PATH: ${OpenCV_LIBS_DIR}")
MESSAGE(STATUS "OpenCV Libraries: ${OpenCV_LIBS}")

FILE(GLOB_RECURSE FILE_SRC "${PROJECT_SOURCE_DIR}/src/Driver.cpp"
        "${PROJECT_SOURCE_DIR}/src/MilkBoxDetector.cpp"
        "${PROJECT_SOURCE_DIR}/src/SelfDefine.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${BIN_NAME} main.cpp ${FILE_SRC})

target_link_libraries(${BIN_NAME} ${OpenCV_LIBS})