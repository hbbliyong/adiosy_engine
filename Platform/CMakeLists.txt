set(DIR_NAME platform)
set(LIBRARY_NAME adiosy_${DIR_NAME})

file(GLOB_RECURSE PUBLIC_HEADERS "include/${DIR_NAME}/*.h*")
file(GLOB_RECURSE PRIVATE_HEADER "src/*.h*")

file(GLOB_RECURSE SOURCE_FILES "src/*.c*")

add_library(${LIBRARY_NAME} ${PUBLIC_HEADERS} ${PRIVATE_HEADER} ${SOURCE_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/${DIR_NAME} 
    PREFIX "Header Files"  # 自定义前缀
    FILES ${PUBLIC_HEADERS}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src 
    PREFIX "Header Files"  # 自定义前缀
    FILES ${PRIVATE_HEADER}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src 
    PREFIX "Source Files"  # 自定义前缀
    FILES ${SOURCE_FILES}
)

#spdlog
add_subdirectory(External/spdlog)
target_link_libraries(${LIBRARY_NAME} PUBLIC spdlog::spdlog)

#glfw
#关掉不需要编译的项目
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_INSTALL OFF)

add_subdirectory(External/glfw EXCLUDE_FROM_ALL)

target_link_libraries(${LIBRARY_NAME} PRIVATE glfw)

#glm
add_subdirectory(External/glm)
target_link_libraries(${LIBRARY_NAME} PUBLIC glm)

set_target_properties(spdlog glfw glm PROPERTIES FOLDER "Dependencies")

#stb
target_include_directories(${LIBRARY_NAME} PUBLIC External/stb)
#Vulkan
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message("Find vulakn success:${Vulkan_INCLUDE_DIRS}")
    include_directories(${Vulkan_INCLUDE_DIRS})
endif()
target_include_directories(${LIBRARY_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${LIBRARY_NAME} PRIVATE ${Vulkan_LIBRARY})

target_include_directories(${LIBRARY_NAME}
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/External
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${DIR_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src    
        
)