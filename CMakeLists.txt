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

chen_set_sources(${PROJECT_SOURCE_DIR}/src/manager MANAGER_SRC)
chen_set_sources(${PROJECT_SOURCE_DIR}/src/servlets SERVLETS_SRC)

set(LIB_SRC
    ${MANAGER_SRC}
    ${SERVLETS_SRC}
    src/chunk_upload.cc
    src/index.cc
    src/my_module.cc
    src/resource_servlet.cc
    src/struct.cc
)

add_executable(gen generator/generator.cc)

add_library(blog SHARED ${LIB_SRC})
# 将orm_data和blog链接到一起
target_link_libraries(blog PRIVATE orm_data)
force_redefine_file_macro_for_sources(blog)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/module)
