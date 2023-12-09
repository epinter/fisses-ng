cmake_minimum_required(VERSION 3.24)

project(
        FISS
        VERSION 1.4.2
        LANGUAGES CXX)
set(PROJECT_LICENSE "MIT")
set(PROJECT_FRIENDLY_NAME ${PROJECT_NAME})
set(PROJECT_LONG_NAME "FileAccess Interface for Skyrim SE Scripts NG")
set(PROJECT_DESCRIPTION ${PROJECT_LONG_NAME})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(addpluginsources)
include(pluginconfig)
include(plugintarget)
include(pluginpackage)

target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)

######## dependencies
target_include_directories(${PROJECT_NAME} PRIVATE ${SIMPLEINI_INCLUDE_DIRS})


