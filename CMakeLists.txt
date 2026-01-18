cmake_minimum_required(VERSION 3.22)
project(Blog)

include(cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++17 -Wall -fPIC -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c17 -Wall -fPIC -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

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
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dbproxy/data)

include_directories(.)

set(LIB_SRC
    src/manager/article_category_rel_manager.cc
    src/manager/article_label_rel_manager.cc
    src/manager/article_manager.cc
    src/manager/assignment_manager.cc
    src/manager/category_manager.cc
    src/manager/label_manager.cc
    src/manager/user_manager.cc
    src/manager/organization_manager.cc
    src/manager/organization_user_rel_manager.cc
    src/manager/assignment_organization_rel_manager.cc
    src/manager/assignment_user_rel_manager.cc
    src/manager/resource_manager.cc
    src/servlets/article/article_admin_lists_servlet.cc
    src/servlets/article/article_create_servlet.cc
    src/servlets/article/article_delete_servlet.cc
    src/servlets/article/article_detail_servlet.cc
    src/servlets/article/article_publish_servlet.cc
    src/servlets/article/article_query_servlet.cc
    src/servlets/article/article_switch_state_servlet.cc
    src/servlets/article/article_update_servlet.cc
    src/servlets/article/article_verify_servlet.cc
    src/servlets/article/article_update_category_servlet.cc
    src/servlets/assignment/assignment_create_servlet.cc
    src/servlets/assignment/assignment_admin_lists_servlet.cc
    src/servlets/assignment/assignment_delete_servlet.cc
    src/servlets/assignment/assignment_detail_servlet.cc
    src/servlets/assignment/assignment_organization_create_servlet.cc
    src/servlets/assignment/assignment_submission_list_servlet.cc
    src/servlets/category/category_create_servlet.cc
    src/servlets/category/category_delete_servlet.cc
    src/servlets/category/category_query_servlet.cc
    src/servlets/file/file_upload_servlet.cc
    src/servlets/file/file_download_servlet.cc
    src/servlets/label/label_create_servlet.cc
    src/servlets/label/label_delete_servlet.cc
    src/servlets/label/label_query_servlet.cc
    src/servlets/organization/organization_admin_lists_servlet.cc
    src/servlets/organization/organization_create_servlet.cc
    src/servlets/organization/organization_delete_servlet.cc
    src/servlets/organization/organization_detail_servlet.cc
    src/servlets/organization/organization_join_servlet.cc
    src/servlets/organization/organization_join_check_servlet.cc
    src/servlets/organization/organization_list_servlet.cc
    src/servlets/organization/organization_user_list_servlet.cc
    src/servlets/organization/organization_user_switch_role_servlet.cc
    src/servlets/organization/organization_user_kick_servlet.cc
    src/servlets/organization/organization_assignment_list_servlet.cc
    src/servlets/user/user_admin_create_servlet.cc
    src/servlets/user/user_admin_delete_servlet.cc
    src/servlets/user/user_admin_lists_servlet.cc
    src/servlets/user/user_admin_recover_servlet.cc
    src/servlets/user/user_admin_reset_passwd_servlet.cc
    src/servlets/user/user_assignment_list_servlet.cc
    src/servlets/user/user_create_servlet.cc
    src/servlets/user/user_exists_servlet.cc
    src/servlets/user/user_info_servlet.cc
    src/servlets/user/user_list_servlet.cc
    src/servlets/user/user_login_servlet.cc
    src/servlets/user/user_logout_servlet.cc
    src/servlets/user/user_reset_passwd_servlet.cc
    src/servlets/user/user_send_code_servlet.cc
    src/servlets/user/user_update_servlet.cc
    src/servlets/user/user_query_servlet.cc
    src/servlets/user/user_organization_list_servlet.cc
    src/index.cc
    src/my_module.cc
    src/resource_servlet.cc
    src/struct.cc
)

add_executable(gen generator/generator.cc)

add_library(blog SHARED ${LIB_SRC})
# 将orm_data和blog链接到一起
target_link_libraries(blog PRIVATE dbproxy_data)
force_redefine_file_macro_for_sources(blog)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/module)
