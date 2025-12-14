cmake_minimum_required(VERSION 3.22)
project(orm_data)

set(LIB_SRC
    blog/data/article_info.cc
    blog/data/article_category_rel_info.cc
    blog/data/article_label_rel_info.cc
    blog/data/assignment_info.cc
    blog/data/assignment_organization_rel_info.cc
    blog/data/assignment_user_rel_info.cc
    blog/data/category_info.cc
    blog/data/email_verification_info.cc
    blog/data/label_info.cc
    blog/data/organization_info.cc
    blog/data/organization_user_rel_info.cc
    blog/data/resource_info.cc
    blog/data/user_info.cc
)
add_library(orm_data ${LIB_SRC})
force_redefine_file_macro_for_sources(orm_data)
