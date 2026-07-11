cmake_minimum_required(VERSION 3.22)
project(dbproxy_data)

set(LIB_SRC
    blog/data/article_info.cc
    blog/data/article_category_rel_info.cc
    blog/data/article_label_rel_info.cc
    blog/data/article_praise_rel_info.cc
    blog/data/assignment_info.cc
    blog/data/assignment_organization_rel_info.cc
    blog/data/assignment_user_rel_info.cc
    blog/data/backup_record_info.cc
    blog/data/bug_info.cc
    blog/data/category_info.cc
    blog/data/chunk_upload_info.cc
    blog/data/comment_info.cc
    blog/data/comment_praise_rel_info.cc
    blog/data/email_verification_info.cc
    blog/data/export_record_info.cc
    blog/data/label_info.cc
    blog/data/notification_info.cc
    blog/data/organization_info.cc
    blog/data/organization_apply_info.cc
    blog/data/organization_repo_prs_info.cc
    blog/data/organization_repos_info.cc
    blog/data/organization_user_rel_info.cc
    blog/data/requirement_info.cc
    blog/data/resource_info.cc
    blog/data/system_settings_info.cc
    blog/data/task_info.cc
    blog/data/user_info.cc
    blog/data/user_ai_config_info.cc
    blog/data/user_follow_rel_info.cc
)
add_library(dbproxy_data ${LIB_SRC})
force_redefine_file_macro_for_sources(dbproxy_data)
