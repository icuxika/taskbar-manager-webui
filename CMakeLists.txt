cmake_minimum_required(VERSION 3.15)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 23)
project(taskbar-manager)
set(CMAKE_CXX_FLAGS "/utf-8 /EHsc")

add_executable(taskbar-manager include/nlohmann/json.hpp main.cpp)
target_include_directories(taskbar-manager PUBLIC include)

target_include_directories(taskbar-manager PRIVATE webui-lib\\webui-windows-msvc-x64\\include)
target_link_directories(taskbar-manager PRIVATE webui-lib\\webui-windows-msvc-x64)
target_link_libraries(taskbar-manager PRIVATE webui-2-static user32 Advapi32 Shell32)

set_target_properties(taskbar-manager PROPERTIES
    WIN32_EXECUTABLE TRUE
)

add_custom_command(TARGET taskbar-manager POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/index.html $<TARGET_FILE_DIR:taskbar-manager>/index.html)
