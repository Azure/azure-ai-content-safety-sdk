add_executable(EmbeddedAACSImageDemo image.cpp)
include(${PROJECT_SOURCE_DIR}/cmake/aacsimage.cmake)

# include directories for image
target_include_directories(EmbeddedAACSImageDemo PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CONTENT_SAFETY_IMAGE_INC_DIR}
)
target_link_libraries(EmbeddedAACSImageDemo PUBLIC contentsafetyimage)

# copy Azure.AI.ContentSafety.Embedded.Image.dll to the output directory
add_custom_command(TARGET EmbeddedAACSImageDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CONTENT_SAFETY_IMAGE_LIB_DIR}/${CONTENT_SAFETY_IMAGE_LIB_NAME}"
        $<TARGET_FILE_DIR:EmbeddedAACSImageDemo>)

# copy DirectML.dll to the output directory
add_dependencies(EmbeddedAACSImageDemo directml)
add_custom_command(TARGET EmbeddedAACSImageDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DIRECTML_LIB_DIR}/${DIRECTML_LIB_NAME}"
        $<TARGET_FILE_DIR:EmbeddedAACSImageDemo>)

# copy OpenCV.dll to the output directory
add_dependencies(EmbeddedAACSImageDemo opencv)
add_custom_command(TARGET EmbeddedAACSImageDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${OPENCV_INSTALL_DIR}/lib/${OPENCV_LIB_NAME}.dll"
        $<TARGET_FILE_DIR:EmbeddedAACSImageDemo>)

# copy config file
add_custom_command(TARGET EmbeddedAACSImageDemo PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/config.ini"
        $<TARGET_FILE_DIR:EmbeddedAACSImageDemo>)


add_custom_command(TARGET EmbeddedAACSImageDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ONNXRUNTIME_SHARED_LIB}"
        $<TARGET_FILE_DIR:EmbeddedAACSImageDemo>)