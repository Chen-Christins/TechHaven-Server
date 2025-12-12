-- 为每个组织插入用户id=1为会长（拥有者）
INSERT INTO organization_user_rel (org_id, user_id, role, status, is_deleted, create_time, update_time) VALUES
(1, 1, 3, 1, 0, datetime('now'), datetime('now')),
(2, 1, 3, 1, 0, datetime('now'), datetime('now')),
(3, 1, 3, 1, 0, datetime('now'), datetime('now')),
(4, 1, 3, 1, 0, datetime('now'), datetime('now')),
(5, 1, 3, 1, 0, datetime('now'), datetime('now')),
(6, 1, 3, 1, 0, datetime('now'), datetime('now')),
(7, 1, 3, 1, 0, datetime('now'), datetime('now')),
(8, 1, 3, 1, 0, datetime('now'), datetime('now'));
