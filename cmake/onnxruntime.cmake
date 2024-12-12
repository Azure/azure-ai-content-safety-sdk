# Check if the build is for Android
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    SET(ANDROID ON)
endif ()

include(ExternalProject)

set(ONNXRUNTIME_PROJECT "extern_onnxruntime")
set(ONNXRUNTIME_VERSION "1.18.0")
set(ONNXRUNTIME_PREFIX_DIR ${THIRD_PARTY_PATH}/onnxruntime)
set(ONNXRUNTIME_SOURCE_DIR
        ${THIRD_PARTY_PATH}/onnxruntime/src/${ONNXRUNTIME_PROJECT})
set(ONNXRUNTIME_INSTALL_DIR ${THIRD_PARTY_PATH}/install/onnxruntime)
set(ONNXRUNTIME_INC_DIR
        "${ONNXRUNTIME_INSTALL_DIR}/include"
        CACHE PATH "onnxruntime include directory." FORCE)
set(ONNXRUNTIME_LIB_DIR
        "${ONNXRUNTIME_INSTALL_DIR}/lib"
        CACHE PATH "onnxruntime lib directory." FORCE)


set(CMAKE_BUILD_RPATH "${CMAKE_BUILD_RPATH}" "${ONNXRUNTIME_LIB_DIR}")

set(ONNXRUNTIME_URL
        "https://globalcdn.nuget.org/packages/microsoft.ml.onnxruntime.directml.${ONNXRUNTIME_VERSION}.nupkg"
)

SET(ONNXRUNTIME_LIB_ARCH "win-x64")
if (CMAKE_SYSTEM_PROCESSOR STREQUAL ARM64)
    SET(ONNXRUNTIME_LIB_ARCH "win-arm64")
endif()

# For ONNXRUNTIME code to include internal headers.
include_directories(${ONNXRUNTIME_INC_DIR})
set(ONNXRUNTIME_LIB_NAME
        "onnxruntime${CMAKE_SHARED_LIBRARY_SUFFIX}")

set(ONNXRUNTIME_SHARED_LIB_SOURCE
        "${ONNXRUNTIME_SOURCE_DIR}/runtimes/${ONNXRUNTIME_LIB_ARCH}/native/onnxruntime.dll"
        CACHE FILEPATH "ONNXRUNTIME source library." FORCE)
set(ONNXRUNTIME_SHARED_LIB
        "${ONNXRUNTIME_INSTALL_DIR}/lib/onnxruntime.dll"
        CACHE FILEPATH "ONNXRUNTIME shared library." FORCE)
set(ONNXRUNTIME_STATIC_LIB_SOURCE
        "${ONNXRUNTIME_SOURCE_DIR}/runtimes/${ONNXRUNTIME_LIB_ARCH}/native/onnxruntime.lib"
        CACHE FILEPATH "ONNXRUNTIME static library." FORCE)
set(ONNXRUNTIME_STATIC_LIB
        "${ONNXRUNTIME_INSTALL_DIR}/lib/onnxruntime.lib"
        CACHE FILEPATH "ONNXRUNTIME static library." FORCE)
set(ONNXRUNTIME_HEADER_DIR
        "${ONNXRUNTIME_SOURCE_DIR}/build/native/include")
set(ONNXRUNTIME_LIB
        "${ONNXRUNTIME_STATIC_LIB}"
        CACHE FILEPATH "ONNXRUNTIME static library." FORCE)

ExternalProject_Add(
        ${ONNXRUNTIME_PROJECT}
        ${EXTERNAL_PROJECT_LOG_ARGS}
        URL ${ONNXRUNTIME_URL}
        PREFIX ${ONNXRUNTIME_PREFIX_DIR}
        DOWNLOAD_NO_PROGRESS 1
        DOWNLOAD_EXTRACT_TIMESTAMP true
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        INSTALL_COMMAND
        ${CMAKE_COMMAND} -E copy ${ONNXRUNTIME_SHARED_LIB_SOURCE} ${ONNXRUNTIME_SHARED_LIB} &&
        ${CMAKE_COMMAND} -E copy ${ONNXRUNTIME_STATIC_LIB_SOURCE} ${ONNXRUNTIME_STATIC_LIB} &&
        ${CMAKE_COMMAND} -E copy_directory ${ONNXRUNTIME_HEADER_DIR} ${ONNXRUNTIME_INC_DIR}
        BUILD_BYPRODUCTS ${ONNXRUNTIME_LIB})

add_library(onnxruntime STATIC IMPORTED GLOBAL)
message(STATUS "ONNXRUNTIME_LIB: ${ONNXRUNTIME_LIB}")
set_property(TARGET onnxruntime PROPERTY IMPORTED_LOCATION ${ONNXRUNTIME_LIB})
add_dependencies(onnxruntime ${ONNXRUNTIME_PROJECT})

function(copy_onnx TARGET_NAME)
    # If error of Exitcode0xc000007b happened when a .exe running, copy onnxruntime.dll
    # to the .exe folder.
    if (TARGET ${TARGET_NAME})
        add_custom_command(
                TARGET ${TARGET_NAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${ONNXRUNTIME_SHARED_LIB}
                ${CMAKE_CURRENT_BINARY_DIR} DEPENDS onnxruntime)
    endif ()
endfunction()