-- 插入作业数据
INSERT INTO assignment (name, subject_name, status, description, max_size, file_type, deadline, is_deleted, create_time, update_time) VALUES 
('Linear Algebra Homework 1', 'Mathematics', 1, 'Solve problems 1-10 from Chapter 2.', 10485760, 'pdf,jpg', datetime('now', '+7 days'), 0, datetime('now'), datetime('now')),
('Calculus Quiz 3', 'Mathematics', 1, 'Integration techniques review.', 5242880, 'pdf', datetime('now', '+3 days'), 0, datetime('now'), datetime('now')),
('Physics Lab Report 1', 'Physics', 1, 'Report on the pendulum experiment.', 20971520, 'pdf,docx', datetime('now', '+14 days'), 0, datetime('now'), datetime('now')),
('History Essay: WW2', 'History', 1, 'Write a 2000-word essay on the causes of WW2.', 5242880, 'docx,pdf', datetime('now', '+30 days'), 0, datetime('now'), datetime('now')),
('Programming Project: Snake Game', 'Computer Science', 1, 'Implement the Snake game in C++.', 52428800, 'zip,tar.gz', datetime('now', '+21 days'), 0, datetime('now'), datetime('now')),
('Chemistry Pre-lab', 'Chemistry', 0, 'Safety quiz and pre-lab questions.', 2097152, 'pdf', datetime('now', '-1 days'), 0, datetime('now', '-5 days'), datetime('now')),
('English Literature Review', 'English', 1, 'Review of "To Kill a Mockingbird".', 5242880, 'pdf,docx', datetime('now', '+10 days'), 0, datetime('now'), datetime('now')),
('Biology Field Notes', 'Biology', 1, 'Submit your field notes from the park visit.', 10485760, 'jpg,png,pdf', datetime('now', '+5 days'), 0, datetime('now'), datetime('now')),
('Economics Case Study', 'Economics', 1, 'Analyze the 2008 financial crisis.', 10485760, 'pdf', datetime('now', '+14 days'), 0, datetime('now'), datetime('now')),
('Art History Portfolio', 'Art', 1, 'Submit a portfolio of 5 sketches.', 104857600, 'jpg,png,zip', datetime('now', '+20 days'), 0, datetime('now'), datetime('now'));
