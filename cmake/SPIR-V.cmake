find_program(GLSLANG_COMMAND glslang)
if(NOT GLSLANG_COMMAND )
    message(FATAL_ERROR "glslangValidator required - source maintained at https://github.com/KhronosGroup/glslang")
endif()

#
# Generate SPIR-V header files from the arguments. Returns a list of headers.
#
function(spirv_shaders ret)
    set(options)
    set(oneValueArgs SPIRV_VERSION)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(_spirvshaders "${options}" "${oneValueArgs}"
            "${multiValueArgs}" ${ARGN})

    if(NOT _spirvshaders_SPIRV_VERSION)
        set(_spirvshaders_SPIRV_VERSION 1.0)
    endif()

    foreach(GLSL ${_spirvshaders_SOURCES})
        string(MAKE_C_IDENTIFIER ${GLSL} IDENTIFIER)
        set(HEADER "${AD_DEFINE_RES_ROOT_DIR}Shader/${GLSL}.spv")
        set(GLSL "${AD_DEFINE_RES_ROOT_DIR}Shader/${GLSL}")

        message("GLSL Command: ${GLSLANG_COMMAND} -V --target-env spirv${_spirvshaders_SPIRV_VERSION} ${GLSL} -o ${HEADER}")

        add_custom_command(
                OUTPUT ${HEADER}
                COMMAND ${GLSLANG_COMMAND} -V --target-env spirv${_spirvshaders_SPIRV_VERSION} ${GLSL} -o ${HEADER}
                DEPENDS ${GLSL})
        list(APPEND HEADERS ${HEADER})
    endforeach()

    set(${ret} "${HEADERS}" PARENT_SCOPE)
endfunction(spirv_shaders)
