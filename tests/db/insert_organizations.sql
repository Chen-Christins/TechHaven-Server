-- 插入组织数据
INSERT INTO organization (name, type, description, owner_id, status, is_deleted, create_time, update_time) VALUES
('Tech Pioneers', 'Technology', 'A community for technology enthusiasts and innovators.', 1, 1, 0, datetime('now'), datetime('now')),
('Open Source Alliance', 'Software', 'Promoting open source software and collaboration.', 2, 1, 0, datetime('now'), datetime('now')),
('Creative Writers Guild', 'Literature', 'A space for writers to share and critique work.', 3, 1, 0, datetime('now'), datetime('now')),
('Data Science Hub', 'Education', 'Focusing on big data, AI, and machine learning.', 1, 1, 0, datetime('now'), datetime('now')),
('Game Devs United', 'Gaming', 'For game developers of all levels.', 4, 1, 0, datetime('now'), datetime('now')),
('Startup Incubator', 'Business', 'Helping new businesses grow.', 4, 1, 0, datetime('now'), datetime('now')),
('Art Collective', 'Art', 'A group for artists to share and collaborate.', 1, 1, 0, datetime('now'), datetime('now')),
('Health & Wellness Club', 'Health', 'Promoting healthy living and wellness.', 3, 1, 0, datetime('now'), datetime('now'));
