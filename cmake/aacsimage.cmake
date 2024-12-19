include(ExternalProject)
set(PACKAGE_NAME_CONTENT_SAFETY_IMAGE "azure.ai.contentsafety.extension.embedded.image")
set(PACKAGE_VERSION_CONTENT_SAFETY_IMAGE "1.0.1-beta.2")
set(CONTENT_SAFETY_IMAGE_PROJECT "extern_aacsimage")

SET(AACS_IMG_LIB_FOLDER "Release")
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    SET(AACS_IMG_LIB_FOLDER "Debug")
endif()

set(CONTENT_SAFETY_IMAGE_PREFIX_DIR ${THIRD_PARTY_PATH}/contentsafety/image)
set(CONTENT_SAFETY_IMAGE_SOURCE_DIR
        ${THIRD_PARTY_PATH}/contentsafety/image/src/${CONTENT_SAFETY_IMAGE_PROJECT})
set(CONTENT_SAFETY_IMAGE_INSTALL_DIR ${THIRD_PARTY_PATH}/install/contentsafety/image)
set(CONTENT_SAFETY_IMAGE_INC_DIR
        "${CONTENT_SAFETY_IMAGE_INSTALL_DIR}/include"
        CACHE PATH "ContentSafety image include directory." FORCE)
set(CONTENT_SAFETY_IMAGE_LIB_DIR
        "${CONTENT_SAFETY_IMAGE_INSTALL_DIR}/lib"
        CACHE PATH "ContentSafety image lib directory." FORCE)
set(CONTENT_SAFETY_IMAGE_URL
        "https://globalcdn.nuget.org/packages/${PACKAGE_NAME_CONTENT_SAFETY_IMAGE}.${PACKAGE_VERSION_CONTENT_SAFETY_IMAGE}.nupkg"
)

set(CONTENT_SAFETY_IMAGE_LIB_NAME
        "Azure.AI.ContentSafety.Embedded.Image.dll")

set(CONTENT_SAFETY_IMAGE_HEADER_DIR
        "${CONTENT_SAFETY_IMAGE_SOURCE_DIR}/build/native/include")

set(CONTENT_SAFETY_IMAGE_SHARED_LIB_SOURCE
        "${CONTENT_SAFETY_IMAGE_SOURCE_DIR}/runtimes/${AACS_LIB_ARCH}/native/${AACS_IMG_LIB_FOLDER}/${CONTENT_SAFETY_IMAGE_LIB_NAME}"
        CACHE FILEPATH "ContentSafety image source library." FORCE)
set(CONTENT_SAFETY_IMAGE_SHARED_LIB
        "${CONTENT_SAFETY_IMAGE_INSTALL_DIR}/lib/${CONTENT_SAFETY_IMAGE_LIB_NAME}"
        CACHE FILEPATH "ContentSafety image shared library." FORCE)

set(CONTENT_SAFETY_IMAGE_STATIC_LIB_SOURCE
        "${CONTENT_SAFETY_IMAGE_SOURCE_DIR}/runtimes/${AACS_LIB_ARCH}/native/${AACS_IMG_LIB_FOLDER}/Azure.AI.ContentSafety.Embedded.Image.lib"
        CACHE FILEPATH "ContentSafety image source library." FORCE)
set(CONTENT_SAFETY_IMAGE_STATIC_LIB
        "${CONTENT_SAFETY_IMAGE_INSTALL_DIR}/lib/Azure.AI.ContentSafety.Embedded.Image.lib"
        CACHE FILEPATH "ContentSafety image shared library." FORCE)

ExternalProject_Add(
        ${CONTENT_SAFETY_IMAGE_PROJECT}
        ${EXTERNAL_PROJECT_LOG_ARGS}
        URL ${CONTENT_SAFETY_IMAGE_URL}
        PREFIX ${CONTENT_SAFETY_IMAGE_PREFIX_DIR}
        DOWNLOAD_NO_PROGRESS 1
        DOWNLOAD_EXTRACT_TIMESTAMP true
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        INSTALL_COMMAND
        ${CMAKE_COMMAND} -E copy ${CONTENT_SAFETY_IMAGE_SHARED_LIB_SOURCE} ${CONTENT_SAFETY_IMAGE_SHARED_LIB} &&
        ${CMAKE_COMMAND} -E copy ${CONTENT_SAFETY_IMAGE_STATIC_LIB_SOURCE} ${CONTENT_SAFETY_IMAGE_STATIC_LIB} &&
        ${CMAKE_COMMAND} -E copy_directory ${CONTENT_SAFETY_IMAGE_HEADER_DIR} ${CONTENT_SAFETY_IMAGE_INC_DIR}
        BUILD_BYPRODUCTS ${CONTENT_SAFETY_IMAGE_STATIC_LIB})
add_library(contentsafetyimage STATIC IMPORTED GLOBAL)
set_property(TARGET contentsafetyimage PROPERTY IMPORTED_LOCATION ${CONTENT_SAFETY_IMAGE_STATIC_LIB})
add_dependencies(contentsafetyimage ${CONTENT_SAFETY_IMAGE_PROJECT})
