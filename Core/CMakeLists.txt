set(DIR_NAME core)
set(LIBRARY_NAME adiosy_${DIR_NAME})

file(GLOB_RECURSE PUBLIC_HEADERS "include/Core/*.h*")
file(GLOB_RECURSE PRIVATE_HEADER "src/*.h*")

file(GLOB_RECURSE SOURCES "src/*.c*")

add_library(${LIBRARY_NAME} ${PUBLIC_HEADERS} ${PRIVATE_HEADER} ${SOURCES})

target_include_directories(${LIBRARY_NAME}
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src    
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${DIR_NAME}
)
target_link_libraries(${LIBRARY_NAME} PUBLIC adiosy_platform)