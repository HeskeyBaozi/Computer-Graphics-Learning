cmake_minimum_required(VERSION 3.9)
project(hw4)

set(CMAKE_CXX_STANDARD 11)
set(ENTRY src/main.cpp src/imgui_impl_glfw_gl3.cpp src/Shader.h)
set(LIBS
        ${PROJECT_SOURCE_DIR}/include/glad/glad.c
        ${PROJECT_SOURCE_DIR}/include/GL/gl3w.c
        ${PROJECT_SOURCE_DIR}/include/imgui.cpp
        ${PROJECT_SOURCE_DIR}/include/imgui_demo.cpp
        ${PROJECT_SOURCE_DIR}/include/imgui_draw.cpp
        )

include_directories(
        ${PROJECT_SOURCE_DIR}/include
)

link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(hw4 ${LIBS} ${ENTRY})
target_link_libraries(hw4 glfw3 opengl32)