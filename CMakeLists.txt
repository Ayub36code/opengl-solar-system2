cmake_minimum_required(VERSION 3.10)

project(opengl_solar_system)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)

add_library(imgui STATIC
        dependencies/imgui-1.92.8/imgui.cpp
        dependencies/imgui-1.92.8/imgui_draw.cpp
        dependencies/imgui-1.92.8/imgui_tables.cpp
        dependencies/imgui-1.92.8/imgui_tables.cpp

        dependencies/imgui-1.92.8/backends/imgui_impl_glfw.cpp
        dependencies/imgui-1.92.8/backends/imgui_impl_opengl3.cpp

)

target_include_directories(imgui PUBLIC
        dependencies/imgui-1.92.8
        dependencies/imgui-1.92.8/backends
)

target_link_libraries(imgui PUBLIC
        glfw
        OpenGL::GL
)

file(
        GLOB_RECURSE APP_SOURCES
        CONFIGURE_DEPENDS
        src/*.cpp
)

add_executable(opengl_solar_system
        ${APP_SOURCES}
        src/glad.c
        include/config.h
)

target_include_directories(opengl_solar_system PRIVATE
        include

        dependencies/glad

        dependencies/KHR

        dependencies/glm

        dependencies/GLFW
)
target_compile_definitions(opengl_solar_system PRIVATE
        PROJECT_ROOT="#{CMAKE_CURRENT_SOURCE_DIR}"

)
target_link_libraries(opengl_solar_system PRIVATE
        glfw
        OpenGL::GL
        imgui
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GUN|Clang")
    target_compile_options(opengl_solar_system PRIVATE
            -Wall
            -Wextra
            -Wpedantic

    )
endif ()


set_target_properties(opengl_solar_system PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
        "${CMAKE_BINARY_DIR}/bin"
)












