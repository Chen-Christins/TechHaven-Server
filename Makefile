# 定义时间记录函数
define time_wrapper
	@echo "开始时间: $$(date '+%Y-%m-%d %H:%M:%S.%3N')" && \
	start_time=$$(date +%s%3N) && \
	$(1) && \
	end_time=$$(date +%s%3N) && \
	duration=$$((end_time - start_time)) && \
	echo "结束时间: $$(date '+%Y-%m-%d %H:%M:%S.%3N')" && \
	echo "总计耗时: $$(echo "scale=3; $$duration/1000" | bc) 秒"
endef

# ---- 构建 ----
#   make               → 全量构建（默认）
#   make build         → 全量构建
#   make blog          → 构建 blog .so 模块目标
#   make blog_server   → 仅构建 blog_server 目标
#   make orm           → 重新生成 ORM 数据类
#   make clean         → 清理并删除 build/ 目录

# 默认目标：裸 make 即全量构建
.DEFAULT_GOAL := build

.PHONY: build
build:
	$(call time_wrapper, \
		if [ -d "build" ]; then \
			cd build && cmake .. && $(MAKE); \
		else \
			mkdir -p build && cd build && cmake .. && $(MAKE); \
		fi)

# 透传任意 target 到 CMake build 目录
%:
	$(call time_wrapper, \
		if [ -d "build" ]; then \
			cd build && cmake .. && $(MAKE) $@; \
		else \
			mkdir -p build && cd build && cmake .. && $(MAKE) $@; \
		fi)

clean:
	$(call time_wrapper, \
		if [ -d "build" ]; then \
			cd build && $(MAKE) clean && cd ..; \
			rm -rf build; \
		fi)

orm:
	$(call time_wrapper, bin/orm dbproxy/xml dbproxy/data)