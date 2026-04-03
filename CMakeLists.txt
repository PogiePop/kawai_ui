cmake_minimum_required(VERSION 3.20)

project(kawai_ui)

if(MSVC)
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
add_compile_options("$<$<COMPILE_LANGUAGE:C>:/utf-8>")
add_compile_options(/W0)
endif()

set(CMAKE_CXX_STANDARD 23)
set(GLAD ${PROJECT_SOURCE_DIR}/submodules/glad)
set(GLFW ${PROJECT_SOURCE_DIR}/submodules/GLFW)
set(GLM ${PROJECT_SOURCE_DIR}/submodules/GLM)
set(SRC ${PROJECT_SOURCE_DIR}/src)

add_library(glad STATIC
    ${GLAD}/src/glad.c
)

add_library(kawai STATIC
    ${SRC}/shader.cpp
    ${SRC}/ui_render.cpp
    ${SRC}/mesh.cpp
    ${SRC}/ui_window.cpp
    ${SRC}/ui_components.cpp
)

include_directories(
    ${GLAD}/include
    ${GLFW}/include
    ${GLM}
    ${PROJECT_SOURCE_DIR}/include
)

link_directories(${GLFW}/build/src/Debug)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} glfw3 opengl32 glad kawai)


add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders  # 直接在 Debug/Release 文件夹内
    COMMENT "Copying shaders..."
    VERBATIM
)


