set(DIR_NAME core)
set(LIBRARY_NAME adiosy_${DIR_NAME})

file(GLOB_RECURSE PUBLIC_HEADERS "include/Core/*.h*")
file(GLOB_RECURSE PRIVATE_HEADER "src/*.h*")

file(GLOB_RECURSE SOURCE_FILES "src/*.c*")

add_library(${LIBRARY_NAME} ${PUBLIC_HEADERS} ${PRIVATE_HEADER} ${SOURCE_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/${DIR_NAME} 
    PREFIX "Header Files"  # �Զ���ǰ׺
    FILES ${PUBLIC_HEADERS}
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src 
    PREFIX "Header Files"  # �Զ���ǰ׺
    FILES ${PRIVATE_HEADER}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src 
    PREFIX "Source Files"  # �Զ���ǰ׺
    FILES ${SOURCE_FILES}
)

target_include_directories(${LIBRARY_NAME}
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src    
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${DIR_NAME}
)
target_link_libraries(${LIBRARY_NAME} PUBLIC adiosy_platform)