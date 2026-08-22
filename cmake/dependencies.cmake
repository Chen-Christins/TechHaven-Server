# dependencies.cmake - Manage project dependencies

# Auto-detect chen-sdk directory by pattern "chen-sdk-*"
file(GLOB CHEN_SDK_DIRS "${PROJECT_SOURCE_DIR}/chen-sdk-*")
if (CHEN_SDK_DIRS)
    list(GET CHEN_SDK_DIRS 0 CHEN_SDK_DIR)
    message(STATUS "Using CHEN_SDK_DIR=${CHEN_SDK_DIR}")
else()
    message(FATAL_ERROR "chen-sdk-* not found under ${PROJECT_SOURCE_DIR}. Please fetch chen-sdk.")
endif()

include_directories(${CHEN_SDK_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/dbproxy/data)

add_subdirectory(${CHEN_SDK_DIR})
add_subdirectory(${PROJECT_SOURCE_DIR}/dbproxy/data)

# cppjieba — 中文分词（全文搜索）
include(FetchContent)
FetchContent_Declare(cppjieba
    GIT_REPOSITORY https://github.com/yanyiwu/cppjieba.git
    GIT_TAG v5.1.0
)
FetchContent_MakeAvailable(cppjieba)
include_directories(${cppjieba_SOURCE_DIR}/include)
include_directories(${cppjieba_SOURCE_DIR}/deps/limonp/include)
add_compile_definitions(CPPJIEBA_DICT_PATH="${cppjieba_SOURCE_DIR}/dict")
