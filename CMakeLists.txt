cmake_minimum_required(VERSION 3.22)
project(chat_room)

include(chen/cmake/utils.cmake)

set(ROOT_PATH ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -Wall -fPIC -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

include_directories(${PROJECT_SOURCE_DIR}/thirdpart)
include_directories(${PROJECT_SOURCE_DIR}/chen)
include_directories(${PROJECT_SOURCE_DIR}/orm_out)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/chen)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/orm_out)

include_directories(.)

set(LIB_SRC
    src/manager/article_manager.cc
    src/manager/user_manager.cc
    src/servlets/user_create_servlet.cc
    src/servlets/user_exists_servlet.cc
    src/servlets/user_info_servlet.cc
    src/servlets/user_login_servlet.cc
    src/servlets/user_logout_servlet.cc
    src/servlets/user_reset_passwd_servlet.cc
    src/servlets/user_send_code_servlet.cc
    src/servlets/user_update_servlet.cc
    src/my_module.cc
    src/struct.cc
)

add_executable(gen generator/generator.cc)

add_library(blog SHARED ${LIB_SRC})
# 将orm_data和blog链接到一起
target_link_libraries(blog PRIVATE orm_data)
force_redefine_file_macro_for_sources(blog)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/module)