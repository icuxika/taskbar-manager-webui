cmake_minimum_required(VERSION 3.15)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 23)
project(taskbar-manager)
set(CMAKE_CXX_FLAGS "/utf-8 /EHsc")

file(GLOB SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} main.cpp ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

target_include_directories(${PROJECT_NAME} PRIVATE third-party\\nlohmann)

target_include_directories(${PROJECT_NAME} PRIVATE third-party\\webui\\include)
target_link_directories(${PROJECT_NAME} PRIVATE third-party\\webui)
target_link_libraries(${PROJECT_NAME} PRIVATE webui-2-static user32 Advapi32 Shell32)

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/index.html $<TARGET_FILE_DIR:${PROJECT_NAME}>/index.html)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/icon.ico $<TARGET_FILE_DIR:${PROJECT_NAME}>/icon.ico)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION .)

install(FILES ${CMAKE_SOURCE_DIR}/index.html
        DESTINATION .)

install(FILES ${CMAKE_SOURCE_DIR}/icon.ico
        DESTINATION .)

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "TaskbarManager")
set(CPACK_PACKAGE_VENDOR "浮木")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "一个使用 WebUI 构建的任务栏管理工具")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-win64")
set(CPACK_PACKAGING_INSTALL_PREFIX "/")
include(CPack)
