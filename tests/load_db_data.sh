#! /bin/sh

WORK_DIR=$(pwd)

rm -rf ~/apps/blog/uploads/*

for sql in "${WORK_DIR}/tests/db/"*.sql; do
  sqlite3 ~/apps/blog/blog.db < "$sql"
done
