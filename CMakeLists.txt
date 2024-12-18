cmake_minimum_required(VERSION 3.27)
project(EmbeddedAzureContentSafetyDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(THIRD_PARTY_PATH  "${CMAKE_BINARY_DIR}/third_party" CACHE STRING
        "A path setting third party libraries download & build directories.")

SET(AACS_LIB_ARCH "win-x64")
if (CMAKE_SYSTEM_PROCESSOR STREQUAL ARM64)
    SET(AACS_LIB_ARCH "win-arm64")
endif()

# include directml
include(cmake/directml.cmake)
# include onnxruntime
include(cmake/onnxruntime.cmake)

add_subdirectory(image)
add_subdirectory(text)

