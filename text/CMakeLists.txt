add_executable(EmbeddedAACSTextDemo text.cpp)
include(${PROJECT_SOURCE_DIR}/cmake/aacstext.cmake)

# include directories for text
target_include_directories(EmbeddedAACSTextDemo PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CONTENT_SAFETY_TEXT_INC_DIR}
)

target_link_libraries(EmbeddedAACSTextDemo PUBLIC contentsafetytext)

# copy Azure.AI.ContentSafety.Embedded.Text.dll to the output directory
add_custom_command(TARGET EmbeddedAACSTextDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CONTENT_SAFETY_TEXT_LIB_DIR}/${CONTENT_SAFETY_TEXT_LIB_NAME}"
        $<TARGET_FILE_DIR:EmbeddedAACSTextDemo>)

# copy DirectML.dll to the output directory
add_dependencies(EmbeddedAACSTextDemo directml)
add_custom_command(TARGET EmbeddedAACSTextDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DIRECTML_LIB_DIR}/${DIRECTML_LIB_NAME}"
        $<TARGET_FILE_DIR:EmbeddedAACSTextDemo>)


add_custom_command(TARGET EmbeddedAACSTextDemo PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/config.ini"
        $<TARGET_FILE_DIR:EmbeddedAACSTextDemo>)

add_custom_command(TARGET EmbeddedAACSTextDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ONNXRUNTIME_SHARED_LIB}"
        $<TARGET_FILE_DIR:EmbeddedAACSTextDemo>)