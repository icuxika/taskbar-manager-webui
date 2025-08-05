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
add_custom_command(TARGET taskbar-manager POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/icon.ico $<TARGET_FILE_DIR:taskbar-manager>/icon.ico)

install(TARGETS taskbar-manager
        RUNTIME DESTINATION .)

install(FILES ${CMAKE_SOURCE_DIR}/index.html
        DESTINATION .)

install(FILES ${CMAKE_SOURCE_DIR}/icon.ico
        DESTINATION .)

set(CPACK_GENERATOR "ZIP")
include(CPack)

set(CPACK_PACKAGE_NAME "TaskbarManager")
set(CPACK_PACKAGE_VENDOR "浮木")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "一个使用 WebUI 构建的任务栏管理工具")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "icuxika@outlook.com")

# 输出文件名：TaskbarManager-1.0.0-win64.zip
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-win64")

# 默认安装路径（无特别用途时可忽略）
set(CPACK_PACKAGING_INSTALL_PREFIX "/")