#!/usr/bin/env bash
#
# switch-env.sh — 在开发/生产配置之间切换
#
# 用法:
#   ./switch-env.sh dev     # 使用 bin/conf/dev.env 渲染配置
#   ./switch-env.sh prod    # 使用 bin/conf/prod.env 渲染配置（不存在则提示先复制 prod.env.example）
#
# 说明:
#   - WORK_PATH 优先取自 <ENV>.env（如 prod.env 中的部署路径），未设置时自动用仓库根目录
#   - 将 bin/conf/ 下所有 *.yml.tpl 渲染为 *.yml（自动发现，无需在脚本中登记文件名）
#   - 使用 envsubst 渲染模板；未安装 envsubst 时回退到 sed
#   - 仅切换配置，不重启服务（如需生效请自行重启或发送 SIGHUP 热重载）
#
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
CONF_DIR="$ROOT_DIR/bin/conf"
# 渲染输出目录（默认与模板同目录；打包时可覆盖为 dist/conf）
OUT_CONF_DIR="${OUT_CONF_DIR:-$CONF_DIR}"

ENV_NAME="${1:-}"

usage() {
    echo "Usage: $0 <dev|prod>" >&2
    exit 1
}

case "$ENV_NAME" in
    dev | prod) ;;
    *) usage ;;
esac

ENV_FILE="$CONF_DIR/$ENV_NAME.env"
if [ ! -f "$ENV_FILE" ]; then
    if [ "$ENV_NAME" = "prod" ]; then
        echo "缺少生产配置: $ENV_FILE" >&2
        echo "请先执行: cp bin/conf/prod.env.example bin/conf/prod.env 并填写真实值" >&2
    else
        echo "缺少配置文件: $ENV_FILE" >&2
    fi
    exit 1
fi

# 导出环境变量供 envsubst 使用（同时保留给回退渲染）
set -a
# shellcheck disable=SC1090
. "$ENV_FILE"
set +a

# WORK_PATH 可由调用方覆盖（如打包时指定远程部署目录）
export WORK_PATH="${WORK_PATH:-$ROOT_DIR}"

# 渲染单个模板文件
render() {
    local tpl="$1"
    local out="$2"

    if command -v envsubst >/dev/null 2>&1; then
        envsubst < "$tpl" > "$out"
    else
        # 回退：按 env 文件逐行替换 ${VAR}，并单独替换 ${WORK_PATH}
        cp "$tpl" "$out"
        while IFS='=' read -r key value; do
            [ -z "$key" ] && continue
            case "$key" in
                \#*) continue ;;
            esac
            local escaped
            escaped=$(printf '%s' "$value" | sed 's/[&|\\]/\\&/g')
            sed "s|\${${key}}|${escaped}|g" "$out" > "$out.tmp" && mv "$out.tmp" "$out"
        done < "$ENV_FILE"
        sed "s|\${WORK_PATH}|${WORK_PATH}|g" "$out" > "$out.tmp" && mv "$out.tmp" "$out"
    fi

    if grep -qE '\$\{[A-Za-z_][A-Za-z0-9_]*\}' "$out"; then
        echo "错误: $out 中存在未替换的占位符，请检查 $ENV_FILE 是否缺少对应变量" >&2
        exit 1
    fi
}

# 渲染 bin/conf/ 下所有 *.yml.tpl → *.yml
shopt -s nullglob
tpls=("$CONF_DIR"/*.yml.tpl)
if [ ${#tpls[@]} -eq 0 ]; then
    echo "错误: $CONF_DIR 下没有 *.yml.tpl 模板文件" >&2
    exit 1
fi

mkdir -p "$OUT_CONF_DIR"
for tpl in "${tpls[@]}"; do
    out="$OUT_CONF_DIR/$(basename "${tpl%.tpl}")"
    render "$tpl" "$out"
    echo "  已生成 $(basename "$out")"
done

echo "配置已生成 [$ENV_NAME]"
echo "  work_path: $WORK_PATH"
echo "  输出目录: $OUT_CONF_DIR"
