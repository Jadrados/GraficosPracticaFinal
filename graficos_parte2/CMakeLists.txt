cmake_minimum_required(VERSION 3.16)
project(P2GLSL)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

# stb_image is header-only, bundled in src/
add_executable(P2GLSL
    src/main.cpp
    src/BOX.h
    src/stb_image.h
)

target_include_directories(P2GLSL PRIVATE src)

target_link_libraries(P2GLSL
    OpenGL::GL
    glfw
    GLEW::GLEW
)

# Copy shaders and images to build dir
add_custom_command(TARGET P2GLSL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:P2GLSL>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/img $<TARGET_FILE_DIR:P2GLSL>/img
)
