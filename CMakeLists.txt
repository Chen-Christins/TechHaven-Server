cmake_minimum_required(VERSION 3.22)
project(Blog)

include(cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置C和C++标准
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置构建类型及编译选项
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# 全局编译选项 - 对所有目标生效
add_compile_options(
    -rdynamic
    -Wall
    -fPIC
    -Wno-deprecated
    # -Werror
    -Wno-unused-function
    -Wno-builtin-macro-redefined
    -Wno-deprecated-declarations
    -Wno-use-after-free
)

# Auto-detect chen-sdk directory by pattern "chen-sdk-*"
file(GLOB CHEN_SDK_DIRS "${PROJECT_SOURCE_DIR}/chen-sdk-*")
if (CHEN_SDK_DIRS)
    list(GET CHEN_SDK_DIRS 0 CHEN_SDK_DIR)
    message(STATUS "Using CHEN_SDK_DIR=${CHEN_SDK_DIR}")
else()
    message(FATAL_ERROR "chen-sdk-* not found under ${PROJECT_SOURCE_DIR}. Please fetch chen-sdk.")
endif()

include_directories(${CHEN_SDK_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/orm_out)

add_subdirectory(${CHEN_SDK_DIR})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/orm_out)

include_directories(.)

# 封装创建服务器模块的函数
function(create_server_module module_name source_dir module_output_dir server_output_dir db_libs)
    chen_set_sources(${source_dir} MODULE_SOURCES)
    add_library(${module_name} SHARED ${MODULE_SOURCES})
    if(db_libs)
        target_link_libraries(${module_name} PRIVATE ${db_libs})
    endif()

    # 设置链接库变量 (例如 WORLD_LIBS)
    string(TOUPPER ${module_name} MODULE_UPPER)
    set(libs
        ${CHEN_LIBRARY}
        ${CHEN_SDK_DEP_LIBS}
    )
    message(STATUS "[DEBUG] create_server_module libs=${libs}")
    set(${MODULE_UPPER}_LIBS ${libs} PARENT_SCOPE)
    
    force_redefine_file_macro_for_sources(${module_name})
    chen_set_target_output_dir(${module_name} "${module_output_dir}")

    # 服务器程序
    chen_add_executable(${module_name}_server "servers/${module_name}.cc" "" "${libs}")
    chen_set_target_output_dir(${module_name}_server "${server_output_dir}")
endfunction()

# 创建博客服务器模块
create_server_module(blog
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/bin/module
    ${PROJECT_SOURCE_DIR}/bin
    orm_data
)
