######## target
find_package(CommonLibSSE REQUIRED)
add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${HEADER_FILES} ${SOURCE_FILES}
                        NAME ${PROJECT_NAME}
                        VERSION ${PROJECT_VERSION}
                        )
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")
target_include_directories(${PROJECT_NAME}
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/cmake>
        $<INSTALL_INTERFACE:src>)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/PCH.h")
    target_precompile_headers(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/PCH.h")
endif()