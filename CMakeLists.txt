cmake_minimum_required(VERSION 3.10)
set(TARGET breakout)
project(${TARGET})

set(CMAKE_CXX_STANDARD 11)

add_executable(${TARGET}
        src/glad.c
                         src/Texture.cpp
                         src/ResourceManager.cpp
                         src/main.cpp
                         src/Shader.cpp
                         )
target_include_directories(${TARGET} PUBLIC include)
target_link_libraries(${TARGET} GL GLU glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor)



