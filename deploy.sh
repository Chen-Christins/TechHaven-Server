#!/bin/bash

# =============================================
# 脚本功能：打包 bin 目录（排除指定项）并支持 SCP 部署
# 使用示例：./deploy.sh [SCP远程IP] [用户名] [密码] 
# =============================================

# ------------------------------
# 1. 初始化配置
# ------------------------------
# 定义源目录（bin）和目标打包目录
SOURCE_DIR="bin"
APP_NAME='mblog'
TEMP_DIR="deploy_package_$(date +%Y%m%d%H%M%S)"  # 临时目录名含时间戳防冲突

# 检查源目录是否存在
if [ ! -d "$SOURCE_DIR" ]; then
    echo "[错误] 源目录 $SOURCE_DIR 不存在！"
    exit 1
fi

# ------------------------------
# 2. 创建临时目录并复制文件（排除指定项）
# ------------------------------
mkdir "$TEMP_DIR" || exit 1

# 复制 bin 内容到临时目录，排除 .git、orm、gen、.vscode
echo "正在复制文件到临时目录 $TEMP_DIR (排除 .git, orm, gen, .vscode)..."
rsync -av --exclude='.git' --exclude='orm' --exclude='gen' --exclude='.vscode' "$SOURCE_DIR"/ "$TEMP_DIR"/

# 检查复制是否成功
if [ $? -ne 0 ]; then
    echo "[错误] 文件复制失败！"
    rm -rf "$TEMP_DIR"
    exit 1
fi

# ------------------------------
# 3. 打包临时目录
# ------------------------------
PACKAGE_NAME="${TEMP_DIR}.tar.gz"
echo "正在打包 $TEMP_DIR -> $PACKAGE_NAME..."
tar -czf "$PACKAGE_NAME" "$TEMP_DIR"

# 检查打包结果
if [ ! -f "$PACKAGE_NAME" ]; then
    echo "[错误] 打包文件 $PACKAGE_NAME 未生成！"
    rm -rf "$TEMP_DIR"
    exit 1
fi

# ------------------------------
# 4. 清理临时目录
# ------------------------------
rm -rf "$TEMP_DIR"
echo "临时目录 $TEMP_DIR 已清理"

# ------------------------------
# 5. SCP 自动化部署
# ------------------------------
if [ $# -eq 4 ]; then
    REMOTE_IP="$1"
    REMOTE_USER="$2"
    REMOTE_PASSWORD="$3"
    REMOTE_PATH="$4"      # 远程目标目录

    # 检查 expect 命令是否存在
    if ! command -v expect &> /dev/null; then
        echo "[错误] 请先安装 expect 工具：sudo apt-get install expect"
        exit 1
    fi

    # 使用 expect 自动化 SCP 传输
    echo "正在通过 SCP 部署到 $REMOTE_USER@$REMOTE_IP:$REMOTE_PATH ..."
    /usr/bin/expect << EOF
        set timeout 30
        spawn scp "$PACKAGE_NAME" $REMOTE_USER@$REMOTE_IP:$REMOTE_PATH
        expect {
            "yes/no" { send "yes\r"; exp_continue }
            "password:" { send "$REMOTE_PASSWORD\r" }
        }
        expect eof
        catch wait result
        exit [lindex \$result 3]
EOF

    # 检查 SCP 结果
    if [ $? -eq 0 ]; then
        echo "部署成功！文件已传输至：$REMOTE_IP:$REMOTE_PATH/$PACKAGE_NAME"
    else
        echo "[错误] SCP 传输失败！"
        exit 1
    fi

	echo "正在登录服务器部署程序 $REMOTE_USER@$REMOTE_IP ..."
	/usr/bin/expect << EOF
        set timeout 30
        spawn ssh $REMOTE_USER@$REMOTE_IP
        expect {
            "yes/no" { send "yes\r"; exp_continue }
            "password:" { send "$REMOTE_PASSWORD\r" }
        }
        expect "#*" 
		send "cd $REMOTE_PATH\r" 

		# 首先尝试查找旧进程并终止
		send "cd $REMOTE_PATH\r"
		send "pid=\\\$(ps aux | grep '$APP_NAME' | grep -v grep | awk '{print \\\$2}')\r"
		send "if test -n \"\\\$pid\"; then\r"  ; # 这里使用了 test 命令
		send "    echo \"找到旧进程 PID: \\\$pid，正在终止...\"\r"
		send "    kill -15 \\\$pid\r"
		send "    sleep 2\r"
		send "    # 检查进程是否仍在运行，如果是则强制终止\r"
		send "    if kill -0 \\\$pid 2>/dev/null; then\r"
		send "        echo \"进程仍在运行，强制终止...\"\r"
		send "    kill -9 \\\$pid\r"
		send "    fi\r"
		send "    echo \"旧进程已终止\"\r"
		send "else\r"
		send "    echo \"未找到运行的 $APP_NAME 进程\"\r"
		send "fi\r"

		send "tar -xzf $PACKAGE_NAME\r" 
		send "cd $TEMP_DIR\r" 
		send "export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH\r" 
		send "nohup ./$APP_NAME -s > blog.log 2>&1 &\r" 
		expect eof
        catch wait result
        exit [lindex \$result 3]
EOF

fi

echo "脚本执行完成！服务器程序已成功部署..."