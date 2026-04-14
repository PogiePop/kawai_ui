cmake_minimum_required(VERSION 3.20)
project(kawai_ui)

# ======================================
# 【核心优化 1】MSVC 全速编译配置
# ======================================
if(MSVC)
    # UTF-8 编码
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:/utf-8>")
    # 禁用警告（不影响功能，大幅加速解析）
    add_compile_options(/W0)

    # 【关键】多线程全速编译（默认使用所有 CPU 核心）
    add_compile_options(/MP)

    # 【加速】禁用调试信息（Debug 也关闭，需要调试再打开）
    add_compile_options(/Zi-)
    add_link_options(/DEBUG:FASTLINK)

    # 【加速】禁用运行时检查
    add_compile_options(/RTC-)
endif()

# C++ 标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ======================================
# 【核心优化 2】禁用不必要的 CMake 检查
# ======================================
set(CMAKE_C_COMPILE_OPTIONS_USE_PCH ON)
set(CMAKE_CXX_COMPILE_OPTIONS_USE_PCH ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# 路径定义
set(GLAD ${PROJECT_SOURCE_DIR}/submodules/glad)
set(GLFW ${PROJECT_SOURCE_DIR}/submodules/GLFW)
set(GLM ${PROJECT_SOURCE_DIR}/submodules/GLM)
set(SRC ${PROJECT_SOURCE_DIR}/src)
set(FREETYPE ${CMAKE_SOURCE_DIR}/submodules/freetype)

# ======================================
# 【核心优化 3】静态库禁用不必要属性
# ======================================
add_library(glad STATIC EXCLUDE_FROM_ALL
    ${GLAD}/src/glad.c
 )
target_include_directories(glad PRIVATE ${GLAD}/include)

add_library(kawai STATIC
    ${SRC}/ka_font.cpp
    ${SRC}/mesh.cpp
    ${SRC}/shader.cpp  
    ${SRC}/ui_layout.cpp
    ${SRC}/ui_component.cpp
    ${SRC}/ui_window.cpp
    ${SRC}/ui_render.cpp
)

# ======================================
# 【核心优化 4】统一用 target_include，替代 include_directories
# ======================================
target_include_directories(kawai PUBLIC
    ${GLAD}/include
    ${GLFW}/include
    ${GLM}
    ${PROJECT_SOURCE_DIR}/include
    ${FREETYPE}/include
)

# 库目录
link_directories(
    ${GLFW}/build/src/Debug
    ${FREETYPE}/builds/Release
)

# 可执行文件
add_executable(${PROJECT_NAME} main.cpp)

set(LIB glfw3
    opengl32
    glad
    kawai
    freetype)

# 链接
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${LIB}
)

# ======================================
# 【绝对稳定】CMake 官方增量拷贝（无 xcopy 坑）
# ======================================
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    COMMENT "Copy shaders (fast incremental)..."
    VERBATIM
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_SOURCE_DIR}/fonts
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts
    COMMENT "Copy fonts (fast incremental)..."
    VERBATIM
)


enable_testing()
add_executable(layout_test test/layout_test.cpp)
add_test(layout_test layout_test)

target_link_libraries(layout_test PRIVATE ${LIB})