# set(LIBRARY_NAME SandBox)

# file(GLOB PUBLIC_HEADERS "include/Platform/*.h*")
# file(GLOB PRIVATE_HEADER "src/*.h*")

# file(GLOB SOURCES "src/*.c*")

# add_executable(${LIBRARY_NAME} ${PUBLIC_HEADERS} ${PRIVATE_HEADER} ${SOURCES})
# target_link_libraries(${LIBRARY_NAME}
# PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )
add_subdirectory(SandBox)

