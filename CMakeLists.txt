cmake_minimum_required(VERSION 3.12)

project(FileForest)

set(glfw3_DIR "C:/Program Files (x86)/GLFW/lib/cmake/glfw3")
set(imgui_DIR ${PROJECT_SOURCE_DIR}/extern/imgui/)

set(IMGUI_SRC
        ${imgui_DIR}/backends/imgui_impl_opengl3.h
        ${imgui_DIR}/backends/imgui_impl_opengl3.cpp
        ${imgui_DIR}/backends/imgui_impl_opengl3_loader.h
        ${imgui_DIR}/backends/imgui_impl_glfw.h
        ${imgui_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_DIR}/imgui.h
        ${imgui_DIR}/imgui.cpp
        ${imgui_DIR}/imgui_draw.cpp
        ${imgui_DIR}/imgui_tables.cpp
        ${imgui_DIR}/imgui_widgets.cpp
        )

find_package(glfw3 REQUIRED)

## Create main executable
add_executable(FileForest main.cpp ${IMGUI_SRC})


target_include_directories(FileForest PUBLIC ${GLFW_INCLUDE_DIRS})

target_link_libraries(FileForest PRIVATE glfw opengl32)
