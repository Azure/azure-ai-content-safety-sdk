cmake_minimum_required(VERSION 3.27)
project(EmbeddedAzureContentSafetyDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(THIRD_PARTY_PATH  "${CMAKE_BINARY_DIR}/third_party" CACHE STRING
        "A path setting third party libraries download & build directories.")
# include directml
include(cmake/directml.cmake)
# include onnxruntime
include(cmake/onnxruntime.cmake)
# include opencv
include(cmake/opencv.cmake)

add_subdirectory(image)
add_subdirectory(text)

