# 业务配置

# system default ai model for article summmary
ai:
  type: glm
  model: glm-4.7-flash
  api_key: # system default AI API key, used when user has not configured their own AI settings

# 默认超级管理员：服务启动时若不存在管理员则自动创建，用于首次登录并配置 SMTP 等系统设置
admin:
  account: admin
  passwd: admin123456
  email: admin@example.com

# 分词字典配置
search:
  jieba_dict_path: ${WORK_PATH}/dict
  index_path: ${WORK_PATH}/search_index.dat
