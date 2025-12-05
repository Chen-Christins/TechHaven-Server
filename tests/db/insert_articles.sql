-- 假设用户ID如下:
-- 1: Admin
-- 2: Zhang San
-- 3: Li Si
-- 4: Test User

-- Admin (ID 1) 的文章
INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (1, 'Blog System Update v1.0', 'We have released the first version of our blog system. It includes basic features like user management and article publishing.', 1, 2, 1, 0, datetime('now', '-10 days'), datetime('now', '-10 days'), datetime('now', '-10 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (1, 'Maintenance Notice', 'The system will undergo maintenance this Sunday from 2 AM to 4 AM.', 1, 2, 1, 0, datetime('now', '-5 days'), datetime('now', '-5 days'), datetime('now', '-5 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (1, 'Welcome to Our Blog', 'Welcome everyone to join our community. Please read the guidelines before posting.', 1, 2, 1, 0, datetime('now', '-20 days'), datetime('now', '-20 days'), datetime('now', '-20 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (1, 'Draft: Future Plans', 'Here are some thoughts on what we want to build next year...', 1, 1, 1, 0, datetime('now'), datetime('now'), datetime('now'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (1, 'Private: Admin Notes', 'Internal notes for administration purposes.', 1, 4, 1, 0, datetime('now'), datetime('now'), datetime('now'));


-- Zhang San (ID 2) 的文章 - 活跃用户
INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (2, 'My First Trip to Japan', 'Japan was amazing! The food, the culture, everything was perfect. Here are some photos...', 1, 2, 2, 0, datetime('now', '-15 days'), datetime('now', '-15 days'), datetime('now', '-15 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (2, 'Learning C++: Pointers', 'Pointers are confusing at first, but once you understand memory addresses, it makes sense.', 1, 2, 3, 0, datetime('now', '-12 days'), datetime('now', '-12 days'), datetime('now', '-12 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (2, 'Best Coffee Shops in Town', 'I visited 5 different coffee shops this week. Here is my ranking.', 1, 2, 2, 0, datetime('now', '-8 days'), datetime('now', '-8 days'), datetime('now', '-8 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (2, 'Review: The Latest Sci-Fi Movie', 'Just watched the new movie. The visual effects were stunning, but the plot was a bit weak.', 1, 2, 4, 0, datetime('now', '-3 days'), datetime('now', '-3 days'), datetime('now', '-3 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (2, 'Why I Love Linux', 'Linux gives me full control over my system. It is great for development.', 1, 2, 3, 0, datetime('now', '-1 day'), datetime('now', '-1 day'), datetime('now', '-1 day'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (2, 'Draft: My Next Project', 'Thinking about building a weather app using Python.', 1, 1, 3, 0, datetime('now'), datetime('now'), datetime('now'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (2, 'Interesting Article on AI', 'Found this great article about the future of AI. Sharing it here.', 2, 2, 3, 0, datetime('now', '-6 days'), datetime('now', '-6 days'), datetime('now', '-6 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (2, 'Deleted Post', 'This post was deleted by the user.', 1, 2, 2, 1, datetime('now', '-30 days'), datetime('now', '-30 days'), datetime('now', '-30 days'));


-- Li Si (ID 3) 的文章 - 技术宅
INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (3, 'Understanding Docker Containers', 'Docker simplifies deployment. Let''s dive into how containers work.', 1, 2, 3, 0, datetime('now', '-25 days'), datetime('now', '-25 days'), datetime('now', '-25 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (3, 'Kubernetes Basics', 'After Docker, the next step is orchestration with K8s.', 1, 2, 3, 0, datetime('now', '-20 days'), datetime('now', '-20 days'), datetime('now', '-20 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (3, 'Python vs Go for Backend', 'Comparing performance and development speed between Python and Go.', 1, 2, 3, 0, datetime('now', '-18 days'), datetime('now', '-18 days'), datetime('now', '-18 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (3, 'Setting up a Home Lab', 'Bought a Raspberry Pi to set up a home server.', 1, 2, 3, 0, datetime('now', '-10 days'), datetime('now', '-10 days'), datetime('now', '-10 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (3, 'My Vim Configuration', 'Sharing my .vimrc file. Vim is the best editor!', 1, 2, 3, 0, datetime('now', '-5 days'), datetime('now', '-5 days'), datetime('now', '-5 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (3, 'Rejected: Hacking Tutorial', 'How to hack into...', 1, 3, 3, 0, datetime('now', '-2 days'), datetime('now', '-2 days'), datetime('now', '-2 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (3, 'Forward: New Tech Stack', 'Check out this new framework.', 2, 2, 3, 0, datetime('now', '-14 days'), datetime('now', '-14 days'), datetime('now', '-14 days'));


-- Test User (ID 4) 的文章 - 混合内容
INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (4, 'Hello World', 'This is my first post on this blog.', 1, 2, 5, 0, datetime('now', '-40 days'), datetime('now', '-40 days'), datetime('now', '-40 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (4, 'Testing Markdown Support', '# Header 1\n## Header 2\n* List item 1\n* List item 2', 1, 2, 5, 0, datetime('now', '-35 days'), datetime('now', '-35 days'), datetime('now', '-35 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (4, 'A Rainy Day', 'It has been raining all day. Good weather for reading.', 1, 2, 2, 0, datetime('now', '-28 days'), datetime('now', '-28 days'), datetime('now', '-28 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (4, 'Book Review: The Great Gatsby', 'A classic novel that explores themes of decadence and idealism.', 1, 2, 4, 0, datetime('now', '-22 days'), datetime('now', '-22 days'), datetime('now', '-22 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (4, 'Recipe: Chocolate Cake', 'Ingredients: Flour, Sugar, Cocoa Powder...', 1, 2, 2, 0, datetime('now', '-16 days'), datetime('now', '-16 days'), datetime('now', '-16 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (4, 'Workout Routine', 'Monday: Chest, Tuesday: Back, Wednesday: Legs...', 1, 2, 2, 0, datetime('now', '-9 days'), datetime('now', '-9 days'), datetime('now', '-9 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (4, 'Music Playlist for Coding', 'Lo-fi beats are the best for concentration.', 1, 2, 4, 0, datetime('now', '-4 days'), datetime('now', '-4 days'), datetime('now', '-4 days'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (4, 'Draft: Travel Plans 2024', 'Planning to visit Europe next summer.', 1, 1, 2, 0, datetime('now'), datetime('now'), datetime('now'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (4, 'Private: Personal Diary', 'Today was a tough day...', 1, 4, 5, 0, datetime('now'), datetime('now'), datetime('now'));

INSERT INTO article (user_id, title, content, type, state, channel, is_deleted, publish_time, create_time, update_time)
VALUES (4, 'Forward: Funny Cat Video', 'Look at this cat!', 2, 2, 5, 0, datetime('now', '-2 days'), datetime('now', '-2 days'), datetime('now', '-2 days'));
