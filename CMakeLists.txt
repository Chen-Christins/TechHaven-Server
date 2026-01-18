cmake_minimum_required(VERSION 3.22)
project(Blog)

include(cmake/common.cmake)
include(cmake/utils.cmake)

# Auto-detect chen-sdk directory by pattern "chen-sdk-*"
file(GLOB CHEN_SDK_DIRS "${PROJECT_SOURCE_DIR}/chen-sdk-*")
if (CHEN_SDK_DIRS)
    list(GET CHEN_SDK_DIRS 0 CHEN_SDK_DIR)
    message(STATUS "Using CHEN_SDK_DIR=${CHEN_SDK_DIR}")
else()
    message(FATAL_ERROR "chen-sdk-* not found under ${PROJECT_SOURCE_DIR}. Please fetch chen-sdk.")
endif()

include_directories(.)
include_directories(${CHEN_SDK_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/dbproxy/data)

add_subdirectory(${CHEN_SDK_DIR})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dbproxy/data)

# 创建博客服务器模块
create_server_module(blog
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/bin/module
    ${PROJECT_SOURCE_DIR}/bin
    dbproxy_data
)

add_executable(gen generator/generator.cc)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/module)
