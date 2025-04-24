set(LIBRARY_NAME SandBox)

file(GLOB PUBLIC_HEADERS "include/Platform/*.h*")
file(GLOB PRIVATE_HEADER "src/*.h*")

file(GLOB SOURCES "*.c*")

#shader files
set (SHADERS 
    00_hello_triangle.vert
    00_hello_triangle.frag
    00_hello_buffer.vert
    00_hello_buffer.frag
)

spirv_shaders(
        SHADER_HEADERS
        SPIRV_VERSION
        1.0 # Currently targeting Vulkan 1.0
        SOURCES
        ${SHADERS}
)

add_executable(${LIBRARY_NAME} ${PUBLIC_HEADERS} ${PRIVATE_HEADER} ${SOURCES})
target_link_libraries(${LIBRARY_NAME}
    PRIVATE
        # ${CMAKE_CURRENT_SOURCE_DIR}
        adiosy_core
        adiosy_platform
)

# set_target_properties(${LIBRARY_NAME} PROPERTIES
#     VS_DEBUGGER_WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
# )

