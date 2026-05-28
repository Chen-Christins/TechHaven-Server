cmake_minimum_required(VERSION 3.22)
project(Blog)

include(cmake/common.cmake)
include(cmake/utils.cmake)
include(cmake/dependcies.cmake)

include_directories(.)

# 创建博客服务器模块
create_server_module(blog
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/bin/module
    ${PROJECT_SOURCE_DIR}/bin
    dbproxy_data
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/module)
