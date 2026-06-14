#!/bin/bash
set -e

# =============================================
# deploy.sh — 支持热重载和优雅关闭的智能部署脚本
# =============================================
# 通过对比本地构建产物与远程实际运行文件的 hash，
# 自动决定部署策略：
#   - 仅 libblog.so 有变更 → 热重载 (./blog_server -s reload)
#   - libchen.so 有变更      → 优雅关闭 + 替换 + 重启
#
# 用法:
#   ./deploy.sh <remote_ip> <user> <password> <remote_path>
#
# 远程部署目录结构:
#   <remote_path>/
#   ├── bin/
#   │   ├── blog_server
#   │   └── module/
#   │       └── libblog.so
#   ├── lib/            (chen-sdk 动态库)
#   ├── logs/
#   └── work/           (PID 文件等运行时数据)
# =============================================

# ------------------------------
# 配置
# ------------------------------
SOURCE_DIR="bin"
PROJECT_NAME="blog"
APP_NAME="blog_server"
MODULE_SO="module/libblog.so"
MODULE_SO_PATH="$SOURCE_DIR/$MODULE_SO"

# 动态查找最新的 chen-sdk 目录
SDK_DIR=$(ls -d chen-sdk-* 2>/dev/null | sort -V | tail -1)
if [ -z "$SDK_DIR" ]; then
    echo "[错误] 未找到 chen-sdk-* 目录！"
    exit 1
fi
echo "SDK 目录: $SDK_DIR"

LIBCHEM_SO="$SDK_DIR/lib/libchen.so"

# 退出时清理残留的 ssh/scp 子进程，防止孤儿进程
cleanup() {
    # 杀掉本脚本 expect 可能遗留的 ssh/scp 子进程
    local mypid=$$
    pgrep -P $mypid ssh 2>/dev/null | xargs kill 2>/dev/null || true
    pgrep -P $mypid scp 2>/dev/null | xargs kill 2>/dev/null || true
}
trap cleanup EXIT

# 检查本地构建产物
if [ ! -f "$SOURCE_DIR/$APP_NAME" ]; then
    echo "[错误] $SOURCE_DIR/$APP_NAME 不存在！请先执行 make xx"
    exit 1
fi
if [ ! -f "$MODULE_SO_PATH" ]; then
    echo "[错误] $MODULE_SO_PATH 不存在！请先执行 make xx"
    exit 1
fi
if [ ! -f "$LIBCHEM_SO" ]; then
    echo "[错误] $LIBCHEM_SO 不存在！"
    exit 1
fi

# 计算本地 hash
LOCAL_MODULE_HASH=$(md5sum "$MODULE_SO_PATH" | awk '{print $1}')
LOCAL_LIBCHEM_HASH=$(md5sum "$LIBCHEM_SO" | awk '{print $1}')

echo "本地: libblog.so  = $LOCAL_MODULE_HASH"
echo "本地: libchen.so  = $LOCAL_LIBCHEM_HASH"

# ------------------------------
# 参数检查
# ------------------------------
if [ $# -lt 4 ]; then
    echo ""
    echo "用法: $0 <remote_ip> <user> <password> <remote_path>"
    echo "示例: $0 192.168.1.100 root mypass /opt/blog"
    exit 0
fi

REMOTE_IP="$1"
REMOTE_USER="$2"
REMOTE_PASSWORD="$3"
REMOTE_PATH="$4"
TIMESTAMP=$(date +%Y%m%d%H%M%S)

if ! command -v expect &> /dev/null; then
    echo "[错误] 需要 expect 工具: sudo apt-get install expect"
    exit 1
fi

# ------------------------------
# Phase 1: 获取远程 hash，判定部署类型
# ------------------------------
echo ""
echo "[检测] 连接远程获取当前运行版本..."

# 为 expect 块导出环境变量（单引号 heredoc 中通过 $env() 获取）
export REMOTE_USER REMOTE_IP REMOTE_PATH MODULE_SO REMOTE_PASSWORD

# 用 expect 执行远程 md5sum，捕获输出
REMOTE_OUTPUT=$(expect << 'EOF'
    set timeout 30
    set remote_user $env(REMOTE_USER)
    set remote_ip $env(REMOTE_IP)
    set remote_path $env(REMOTE_PATH)
    set module_so $env(MODULE_SO)
    set remote_pass $env(REMOTE_PASSWORD)

    spawn ssh -o StrictHostKeyChecking=no -o ConnectTimeout=10 $remote_user@$remote_ip "echo '<<<HASH_START>>>'; md5sum $remote_path/bin/$module_so 2>/dev/null; md5sum $remote_path/lib/libchen.so 2>/dev/null; echo '<<<HASH_END>>>'"
    log_user 0
    expect {
        "yes/no*" { send "yes\r"; exp_continue }
        "*assword:*" { send "$remote_pass\r"; exp_continue }
        timeout { puts "TIMEOUT"; exit 1 }
        eof { }
    }
    # 即使 log_user 0，也要把远程命令输出打印到 stdout 供 bash 捕获
    puts $expect_out(buffer)
    catch wait result
    exit [lindex $result 3]
EOF
)

# 提取 HASH_START 和 HASH_END 之间的内容作为远程输出
REMOTE_HASH_OUTPUT=$(echo "$REMOTE_OUTPUT" | sed -n '/<<<HASH_START>>>/,/<<<HASH_END>>>/p' | grep -v '<<<HASH_START>>>' | grep -v '<<<HASH_END>>>')

# 解析远程 hash（expect 输出可能包含额外字符，取第一个有效行）
REMOTE_MODULE_HASH=$(echo "$REMOTE_HASH_OUTPUT" | grep "$MODULE_SO" | awk '{print $1}')
REMOTE_LIBCHEM_HASH=$(echo "$REMOTE_HASH_OUTPUT" | grep "libchen.so" | awk '{print $1}')

if [ -z "$REMOTE_MODULE_HASH" ] && [ -z "$REMOTE_LIBCHEM_HASH" ]; then
    echo "[检测] 远程文件不存在，按首次部署处理 → 完整部署"
    DEPLOY_TYPE="full"
elif [ -z "$REMOTE_LIBCHEM_HASH" ]; then
    echo "[检测] 远程 libchen.so 不存在 → 完整部署"
    DEPLOY_TYPE="full"
else
    echo "远程: libblog.so  = $REMOTE_MODULE_HASH"
    echo "远程: libchen.so  = $REMOTE_LIBCHEM_HASH"

    if [ "$LOCAL_LIBCHEM_HASH" != "$REMOTE_LIBCHEM_HASH" ]; then
        DEPLOY_TYPE="full"
        echo "[检测] libchen.so (框架) 有变更 → 完整部署"
    elif [ "$LOCAL_MODULE_HASH" != "$REMOTE_MODULE_HASH" ]; then
        DEPLOY_TYPE="module"
        echo "[检测] 仅 libblog.so (模块) 有变更 → 热重载"
    else
        echo "[检测] 远程已是最新版本，无需部署"
        exit 0
    fi
fi

# ------------------------------
# Phase 2: 打包
# ------------------------------
if [ "$DEPLOY_TYPE" = "module" ]; then
    PACKAGE_NAME="${PROJECT_NAME}_module_${TIMESTAMP}.tar.gz"
    echo "[打包] 模块热重载包: $PACKAGE_NAME"
    tar -czf "$PACKAGE_NAME" -C "$SOURCE_DIR" "$MODULE_SO"
else
    PACKAGE_NAME="${PROJECT_NAME}_full_${TIMESTAMP}.tar.gz"
    echo "[打包] 完整部署包: $PACKAGE_NAME"

    TEMP_DIR="deploy_package_${TIMESTAMP}"
    mkdir -p "$TEMP_DIR"/{bin/module,lib}

    # 复制 bin（排除不需要的文件）
    rsync -a --exclude='.git' --exclude='orm' --exclude='gen' --exclude='.vscode' \
        "$SOURCE_DIR"/ "$TEMP_DIR/bin/"

    # 复制 SDK 动态库
    cp -r "$SDK_DIR"/lib/* "$TEMP_DIR/lib/" 2>/dev/null || true

    tar -czf "$PACKAGE_NAME" "$TEMP_DIR"
    rm -rf "$TEMP_DIR"
fi
echo "  -> $PACKAGE_NAME ($(du -h "$PACKAGE_NAME" | cut -f1))"
export PACKAGE_NAME

# 清理旧包（保留最近 3 个）
ls -t ${PROJECT_NAME}_*.tar.gz 2>/dev/null | tail -n +4 | xargs rm -f 2>/dev/null || true

# ------------------------------
# Phase 3: 传输 + 远程部署
# ------------------------------
echo ""
echo "=========================================="
echo "远程部署 → $REMOTE_USER@$REMOTE_IP:$REMOTE_PATH"
echo "部署类型: $DEPLOY_TYPE"
echo "=========================================="

# SCP 传输
echo "[传输] 上传 $PACKAGE_NAME ..."
expect << 'EOF'
    set timeout 120
    set remote_user $env(REMOTE_USER)
    set remote_ip $env(REMOTE_IP)
    set remote_pass $env(REMOTE_PASSWORD)
    set remote_path $env(REMOTE_PATH)
    set package_name $env(PACKAGE_NAME)

    spawn scp -o StrictHostKeyChecking=no -o ConnectTimeout=10 $package_name $remote_user@$remote_ip:$remote_path
    log_user 0
    expect {
        "yes/no*" { send "yes\r"; exp_continue }
        "*assword:*" { send "$remote_pass\r"; exp_continue }
        timeout { puts "TIMEOUT"; exit 1 }
        eof { }
    }
    catch wait result
    exit [lindex $result 3]
EOF

if [ $? -ne 0 ]; then
    echo "[错误] SCP 传输失败！"
    exit 1
fi
echo "[传输] 完成"

# 导出变量供 expect 使用（单引号 heredoc 中通过 $env() 获取）
export REMOTE_USER REMOTE_IP REMOTE_PASSWORD REMOTE_PATH APP_NAME MODULE_SO PROJECT_NAME PACKAGE_NAME

# 远程部署执行
if [ "$DEPLOY_TYPE" = "module" ]; then
    # ---- 模块热重载 ----
    expect << 'EOF'
        set timeout 30
        set remote_user $env(REMOTE_USER)
        set remote_ip $env(REMOTE_IP)
        set remote_pass $env(REMOTE_PASSWORD)
        set remote_path $env(REMOTE_PATH)
        set app_name $env(APP_NAME)
        set module_so $env(MODULE_SO)
        set package_name $env(PACKAGE_NAME)

        spawn ssh -o StrictHostKeyChecking=no -o ConnectTimeout=10 $remote_user@$remote_ip
        log_user 0
        expect {
            "yes/no*" { send "yes\r"; exp_continue }
            "*assword:*" { send "$remote_pass\r"; exp_continue }
            timeout { puts "TIMEOUT"; exit 1 }
            "$ " {}
            "# " {}
        }
        set prompt "$ "
        send "cd $remote_path\r"
        expect $prompt

        # 备份旧模块
        send "cp bin/$module_so bin/${module_so}.bak.$(date +%Y%m%d%H%M%S) 2>/dev/null; true\r"
        expect $prompt

        # 解压并替换模块
        send "tar -xzf $package_name -C bin/\r"
        expect $prompt
        send "rm -f $package_name\r"
        expect $prompt

        # 热重载
        send "LD_LIBRARY_PATH=./lib ./bin/$app_name -s reload\r"
        expect $prompt
        send {echo '远程: 热重载完成'}
        send "\r"
        expect $prompt
        send "exit\r"
        expect eof
        catch wait result
        exit [lindex $result 3]
EOF
    echo "[远程] 模块热重载完成"

else
    # ---- 完整重启（优雅关闭 + 替换 + 启动） ----
    expect << 'EOF'
        set timeout 120
        set remote_user $env(REMOTE_USER)
        set remote_ip $env(REMOTE_IP)
        set remote_pass $env(REMOTE_PASSWORD)
        set remote_path $env(REMOTE_PATH)
        set app_name $env(APP_NAME)
        set project_name $env(PROJECT_NAME)
        set package_name $env(PACKAGE_NAME)

        spawn ssh -o StrictHostKeyChecking=no -o ConnectTimeout=10 $remote_user@$remote_ip
        log_user 0
        expect {
            "yes/no*" { send "yes\r"; exp_continue }
            "*assword:*" { send "$remote_pass\r"; exp_continue }
            timeout { puts "TIMEOUT"; exit 1 }
            "$ " {}
            "# " {}
        }
        set prompt "$ "
        send "cd $remote_path\r"
        expect $prompt

        # 优雅关闭
        send {echo '远程: 优雅关闭...'}
        send "\r"
        expect $prompt
        send "LD_LIBRARY_PATH=./lib ./bin/$app_name -s stop 2>/dev/null; true\r"
        expect $prompt
        send "sleep 3\r"
        expect $prompt

        # 确保进程已退出
        send "if pgrep -f $app_name > /dev/null 2>&1; then sleep 5; fi\r"
        expect $prompt
        send "if pgrep -f $app_name > /dev/null 2>&1; then pkill -9 $app_name 2>/dev/null; sleep 1; fi\r"
        expect $prompt

        # 备份旧版本
        send {BACKUP_DIR="backup_$(date +%Y%m%d%H%M%S)"}
        send "\r"
        expect $prompt
        send {mkdir -p "$BACKUP_DIR"/bin/module "$BACKUP_DIR"/lib}
        send "\r"
        expect $prompt
        send {[ -d lib ] && cp -r lib/ "$BACKUP_DIR/lib/" 2>/dev/null; true}
        send "\r"
        expect $prompt
        send {[ -f bin/blog_server ] && cp bin/blog_server "$BACKUP_DIR/bin/" 2>/dev/null; true}
        send "\r"
        expect $prompt
        send {[ -d bin/module ] && cp -r bin/module/ "$BACKUP_DIR/bin/module/" 2>/dev/null; true}
        send "\r"
        expect $prompt
        send {echo "远程: 已备份到 $BACKUP_DIR"}
        send "\r"
        expect $prompt

        # 确保目标目录存在（兼容首次部署 / 旧版扁平结构升级）
        send {mkdir -p bin/module lib logs}
        send "\r"
        expect $prompt

        # 解压新版本
        send "tar -xzf $package_name\r"
        expect $prompt
        send "rm -f $package_name\r"
        expect $prompt
        send {TEMP_DIR=$(ls -d deploy_package_* 2>/dev/null | head -1)}
        send "\r"
        expect $prompt
        send {if [ -n "$TEMP_DIR" ]; then}
        send "\r"
        send {  cp -r "$TEMP_DIR"/bin/* bin/ 2>/dev/null; true}
        send "\r"
        send {  cp -r "$TEMP_DIR"/lib/* lib/ 2>/dev/null; true}
        send "\r"
        send {  rm -rf "$TEMP_DIR"}
        send "\r"
        send {fi}
        send "\r"
        expect $prompt

        # 用生产环境配置覆盖默认配置
        send {if [ -f system.yml.bak ]; then cp system.yml.bak bin/conf/system.yml; echo '远程: 配置已覆盖'; fi}
        send "\r"
        expect $prompt

        # 启动新版本
        send {echo '远程: 启动服务...'}
        send "\r"
        expect $prompt
        send "rm -f logs/*.log 2>/dev/null; true\r"
        expect $prompt
        send "LD_LIBRARY_PATH=./lib nohup ./bin/$app_name -d > logs/${project_name}.log 2>&1 &\r"
        expect $prompt
        send "sleep 2\r"
        expect $prompt

        # 验证
        send {if pgrep -f $app_name > /dev/null 2>&1; then echo '远程: 启动成功'; else echo '远程: 警告: 进程未启动，请检查日志'; fi}
        send "\r"
        expect $prompt
        send "exit\r"
        expect eof
        catch wait result
        exit [lindex $result 3]
EOF
    echo "[远程] 完整部署完成"
fi

echo ""
echo "=========================================="
echo "部署完成 ($DEPLOY_TYPE)"
echo "=========================================="
