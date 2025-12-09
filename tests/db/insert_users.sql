-- 插入管理员用户 (密码默认为 123456 的 MD5 值)
INSERT INTO user (name, account, email, passwd, role, state, is_deleted, create_time, update_time) 
VALUES ('Admin', 'admin', 'admin@blog.com', 'e10adc3949ba59abbe56e057f20f883e', 2, 1, 0, datetime('now'), datetime('now'));

-- 插入普通用户 1
INSERT INTO user (name, account, email, passwd, role, state, is_deleted, create_time, update_time) 
VALUES ('Zhang San', 'zhangsan', 'zhangsan@blog.com', 'e10adc3949ba59abbe56e057f20f883e', 1, 1, 0, datetime('now'), datetime('now'));

-- 插入普通用户 2
INSERT INTO user (name, account, email, passwd, role, state, is_deleted, create_time, update_time) 
VALUES ('Li Si', 'lisi', 'lisi@blog.com', 'e10adc3949ba59abbe56e057f20f883e', 1, 1, 0, datetime('now'), datetime('now'));

-- 插入测试用户 (带简介和位置)
INSERT INTO user (name, account, email, passwd, role, state, bio, location, is_deleted, create_time, update_time) 
VALUES ('Test User', 'test', 'test@blog.com', 'e10adc3949ba59abbe56e057f20f883e', 1, 1, 'This is a test account', 'Shanghai', 0, datetime('now'), datetime('now'));