# set(LIBRARY_NAME SandBox)

# file(GLOB PUBLIC_HEADERS "include/Platform/*.h*")
# file(GLOB PRIVATE_HEADER "src/*.h*")

# file(GLOB SOURCES "src/*.c*")

# add_executable(${LIBRARY_NAME} ${PUBLIC_HEADERS} ${PRIVATE_HEADER} ${SOURCES})
# target_link_libraries(${LIBRARY_NAME}
# PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )
# add_subdirectory(SandBox)
# add_subdirectory(FirstCube)

set(SUBPROJECTS SandBox 01_FirstCube 02_ECS_Entity 03_Event_Camera)

foreach(subproject ${SUBPROJECTS})
    add_subdirectory(${subproject})
    set_target_properties(${subproject} PROPERTIES FOLDER "Samples")
endforeach()