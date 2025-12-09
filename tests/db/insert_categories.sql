-- 插入顶级分类: 技术
INSERT INTO category (id, name, color, description, url, icon, parent_id, status, is_deleted, create_time, update_time)
VALUES (1, 'Technology', '#007bff', 'All about technology and programming', '/category/tech', '💻', 0, 1, 0, datetime('now'), datetime('now'));

-- 插入顶级分类: 生活
INSERT INTO category (id, name, color, description, url, icon, parent_id, status, is_deleted, create_time, update_time)
VALUES (2, 'Life', '#28a745', 'Daily life, thoughts and stories', '/category/life', '🌟', 0, 1, 0, datetime('now'), datetime('now'));
-- 插入二级分类: 编程 (父分类: 技术)
INSERT INTO category (id, name, color, description, url, icon, parent_id, status, is_deleted, create_time, update_time)
VALUES (3, 'Programming', '#17a2b8', 'Coding tutorials and discussions', '/category/programming', '💻', 1, 1, 0, datetime('now'), datetime('now'));

-- 插入二级分类: 旅行 (父分类: 生活)
INSERT INTO category (id, name, color, description, url, icon, parent_id, status, is_deleted, create_time, update_time)
VALUES (4, 'Travel', '#ffc107', 'Travel logs and photos', '/category/travel', '✈️', 2, 1, 0, datetime('now'), datetime('now'));

-- 插入二级分类: 美食 (父分类: 生活)
INSERT INTO category (id, name, color, description, url, icon, parent_id, status, is_deleted, create_time, update_time)
VALUES (5, 'Food', '#dc3545', 'Delicious food and recipes', '/category/food', '🍔', 2, 1, 0, datetime('now'), datetime('now'));
-- 插入二级分类: Linux (父分类: 技术)
INSERT INTO category (id, name, color, description, url, icon, parent_id, status, is_deleted, create_time, update_time)
VALUES (6, 'Linux', '#343a40', 'Linux operating system and tools', '/category/linux', '🐧', 1, 1, 0, datetime('now'), datetime('now'));