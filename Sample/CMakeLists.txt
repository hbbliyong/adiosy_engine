#Shader
# 获取上级目录的 Resource/shader 路径
set(SHADER_DIR "${AD_DEFINE_RES_ROOT_DIR}Shader")

# 检查目录是否存在
if(NOT IS_DIRECTORY ${SHADER_DIR})
    message(FATAL_ERROR "Shader directory not found: ${SHADER_DIR}")
endif()

# 或者递归收集所有子目录的文件（包括子文件夹）
file(GLOB_RECURSE SHADER_FILES
    "${SHADER_DIR}/*.vert" 
    "${SHADER_DIR}/*.frag" 
    "${SHADER_DIR}/*.geom"
    "${SHADER_DIR}/*.comp"
)

set(SHADER_HEADERS "")
set(SOURCES "")
foreach(FILE ${SHADER_FILES})
    get_filename_component(FILE_NAME ${FILE} NAME)
    list(APPEND SHADER_HEADERS ${FILE_NAME})
    message(STATUS ${FILE_NAME})
endforeach()

spirv_shaders(
        RET_SHADER_HEADERS
        SPIRV_VERSION
        1.0 # Currently targeting Vulkan 1.0
        SOURCES
        ${SHADER_HEADERS}
)


#获取当前目录下的所有子目录（过滤隐藏目录）
file(GLOB SUBDIRECTORIES 
    LIST_DIRECTORIES true
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*"
    )

#过滤不需要的目录
set(EXCLUDED_DIRS 
   # "SandBox"
)

# 创建解决方案文件夹名称
set(SOLUTION_FOLDER "Sample")

 foreach(SUBDIR ${SUBDIRECTORIES})
    #检测是否为有效目录
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR})
        #排除不需要的目录
        if(NOT ${SUBDIR} IN_LIST EXCLUDED_DIRS)
            # 递归查找所有cpp文件
            file(GLOB_RECURSE SOURCES 
                "${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/*.c*"
                "${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}/*.h*"
            )
            # 如果找到源文件
            if(SOURCES)
                # 在 Visual Studio 中分组显示
                source_group("Shaders" FILES ${RET_SHADER_HEADERS})
                # 生成项目名称（替换特殊字符）
                string(REGEX REPLACE "[^a-zA-Z0-9]" "_" TARGET_NAME ${SUBDIR})
                 # 创建可执行程序
                add_executable(${TARGET_NAME} ${SOURCES} ${RET_SHADER_HEADERS})

                target_link_libraries(${TARGET_NAME}  # <- 目标名称在前
                PUBLIC                            # <- 关键字在中
                adiosy_core          # <- 依赖库在后
             )
                 # 设置VS解决方案文件夹
                set_target_properties(${TARGET_NAME} 
                     PROPERTIES 
                     FOLDER ${SOLUTION_FOLDER}
                     VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}"
                 )

               
                 # 可选：添加公共头文件路径
                #  target_include_directories(${TARGET_NAME} 
                #      PRIVATE 
                #      ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}
                #  )
 
                 message(STATUS "Generated project: ${TARGET_NAME} from ${SUBDIR}")
            endif()
           
        endif()
    endif()        
 endforeach()
 