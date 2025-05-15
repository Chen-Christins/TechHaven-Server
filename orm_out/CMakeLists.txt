cmake_minimum_required(VERSION 3.22)
project(orm_data)

set(LIB_SRC
    blog/data/article_info.cc
    blog/data/user_info.cc
)
add_library(orm_data ${LIB_SRC})
force_redefine_file_macro_for_sources(orm_data)
