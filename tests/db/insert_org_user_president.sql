-- 为每个组织插入用户id=1为组织管理员（最高权限）
-- role: 1=普通成员 2=报告者 3=开发者 4=研发主管 5=组织管理员
INSERT INTO organization_user_rel (org_id, user_id, role, status, is_deleted, create_time, update_time) VALUES
(1, 1, 5, 1, 0, datetime('now'), datetime('now')),
(2, 1, 5, 1, 0, datetime('now'), datetime('now')),
(3, 1, 5, 1, 0, datetime('now'), datetime('now')),
(4, 1, 5, 1, 0, datetime('now'), datetime('now')),
(5, 1, 5, 1, 0, datetime('now'), datetime('now')),
(6, 1, 5, 1, 0, datetime('now'), datetime('now')),
(7, 1, 5, 1, 0, datetime('now'), datetime('now')),
(8, 1, 5, 1, 0, datetime('now'), datetime('now'));
