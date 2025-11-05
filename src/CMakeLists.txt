cmake_minimum_required(VERSION 3.16)
project(LabWork01)

find_package(OpenCV REQUIRED)

SET (CMAKE_BUILD_TYPE "Release")
#SET (CMAKE_BUILD_TYPE "Debug")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++20")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3")
SET (CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

SET (BASE_DIR ${CMAKE_SOURCE_DIR} )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/../bin)

add_executable(negative_img negative_img.cpp)
target_link_libraries(negative_img ${OpenCV_LIBS})

add_executable(mirror_img mirror_img.cpp)
target_link_libraries(mirror_img ${OpenCV_LIBS})

add_executable(rotate_img rotate_img.cpp)
target_link_libraries(rotate_img ${OpenCV_LIBS})

add_executable(modifyLight_img modifyLight_img.cpp)
target_link_libraries(modifyLight_img ${OpenCV_LIBS})

add_library(golomb golomb.cpp)
add_executable(test_golomb golomb_test.cpp)
target_link_libraries(test_golomb golomb)