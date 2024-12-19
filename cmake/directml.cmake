include(ExternalProject)

set(DIRECTML_PROJECT "extern_directml")
set(DIRECTML_VERSION "1.15.0")
set(DIRECTML_PREFIX_DIR ${THIRD_PARTY_PATH}/directml)
set(DIRECTML_SOURCE_DIR
        ${THIRD_PARTY_PATH}/directml/src/${DIRECTML_PROJECT})
set(DIRECTML_INSTALL_DIR ${THIRD_PARTY_PATH}/install/directml)
set(DIRECTML_LIB_DIR
        "${DIRECTML_INSTALL_DIR}/lib"
        CACHE PATH "DirectML lib directory." FORCE)
set(DIRECTML_URL
        "https://globalcdn.nuget.org/packages/microsoft.ai.directml.${DIRECTML_VERSION}.nupkg"
)

set(DIRECTML_LIB_NAME
        "DirectML.dll")

SET(DIRECTML_LIB_ARCH "x64-win")
if (CMAKE_SYSTEM_PROCESSOR STREQUAL ARM64)
    SET(DIRECTML_LIB_ARCH "arm64-win")
endif()

set(DIRECTML_SHARED_LIB_SOURCE
        "${DIRECTML_SOURCE_DIR}/bin/${DIRECTML_LIB_ARCH}/${DIRECTML_LIB_NAME}"
        CACHE FILEPATH "DirectML source library." FORCE)
set(DIRECTML_SHARED_LIB
        "${DIRECTML_INSTALL_DIR}/lib/${DIRECTML_LIB_NAME}"
        CACHE FILEPATH "DirectML shared library." FORCE)

set(DIRECTML_STATIC_LIB_SOURCE
        "${DIRECTML_SOURCE_DIR}/bin/${DIRECTML_LIB_ARCH}/DirectML.lib"
        CACHE FILEPATH "DirectML source library." FORCE)
set(DIRECTML_STATIC_LIB
        "${DIRECTML_INSTALL_DIR}/lib/DirectML.lib"
        CACHE FILEPATH "DirectML shared library." FORCE)

ExternalProject_Add(
        ${DIRECTML_PROJECT}
        ${EXTERNAL_PROJECT_LOG_ARGS}
        URL ${DIRECTML_URL}
        PREFIX ${DIRECTML_PREFIX_DIR}
        DOWNLOAD_NO_PROGRESS 1
        DOWNLOAD_EXTRACT_TIMESTAMP true
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        INSTALL_COMMAND
        ${CMAKE_COMMAND} -E copy ${DIRECTML_SHARED_LIB_SOURCE} ${DIRECTML_SHARED_LIB} &&
        ${CMAKE_COMMAND} -E copy ${DIRECTML_STATIC_LIB_SOURCE} ${DIRECTML_STATIC_LIB}
        BUILD_BYPRODUCTS ${DIRECTML_STATIC_LIB})
add_library(directml STATIC IMPORTED GLOBAL)
add_dependencies(directml ${DIRECTML_PROJECT})
