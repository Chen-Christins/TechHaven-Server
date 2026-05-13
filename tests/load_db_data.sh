#! /bin/sh

WORK_DIR=$(pwd)

rm -rf ~/apps/blog/uploads/*

for sql in "${WORK_DIR}/tests/db/"*.sql; do
  sqlite3 /home/chen/workspace/Blog/blog.db < "$sql"
done
