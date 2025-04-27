set(LIBRARY_NAME SandBox)

file(GLOB PUBLIC_HEADERS "include/Platform/*.h*")
file(GLOB PRIVATE_HEADER "src/*.h*")

file(GLOB SOURCES "*.c*")

#shader files
set (SHADERS 
    00_hello_triangle.vert
    00_hello_triangle.frag
    01_hello_buffer.vert
    01_hello_buffer.frag
)

spirv_shaders(
        SHADER_HEADERS
        SPIRV_VERSION
        1.0 # Currently targeting Vulkan 1.0
        SOURCES
        ${SHADERS}
)
# 添加测试内存泄露工具包含目录
include_directories("C:/Program Files (x86)/Visual Leak Detector/include")
# 添加测试内存泄露工具库目录
link_directories("C:/Program Files (x86)/Visual Leak Detector/lib/Win64")
# 添加测试内存泄露工具全局的预处理器定义
add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)

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


