cmake_minimum_required(VERSION 3.22)
project(chat_room)

set(ROOT_PATH ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

find_package(jsoncpp REQUIRED)
if(JSONCPP_FOUND)
    include_directories(${JSONCPP_INCLUDE_DIRS})
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/chen)

include(chen/cmake/utils.cmake)

include_directories(.)
include_directories(${PROJECT_SOURCE_DIR}/chen/src)

set(LIB_SRC
    src/my_module.cc
)

add_library(blog SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(blog)

target_link_libraries(blog PRIVATE ${JSONCPP_LIBRARIES})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/module)