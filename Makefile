define time_wrapper
	echo "开始时间: $$(date '+%Y-%m-%d %H:%M:%S.%3N')" && \
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
#   make clean         → 清理并删除 build/ 目录
#   make -j4           → 手动控制并行数（默认留一核）

# 默认目标
.DEFAULT_GOAL := build

_NPROC := $(shell nproc)
_JOBS  ?= $(shell expr $(_NPROC) - 1)

# 从 make -jN 提取并行数
_JOB_CMD = _J="$$(echo "$(MAKEFLAGS)" | sed -n 's/.*-j[[:space:]]*\([0-9]\{1,\}\).*/\1/p')"

.PHONY: build
build:
	@$(_JOB_CMD) && \
	$(call time_wrapper, \
		if [ -d "build" ]; then \
			cd build && cmake -G Ninja .. && ninja -j"$${_J:-$(_JOBS)}"; \
		else \
			mkdir -p build && cd build && cmake -G Ninja .. && ninja -j"$${_J:-$(_JOBS)}"; \
		fi)

%:
	@$(_JOB_CMD) && \
	$(call time_wrapper, \
		if [ -d "build" ]; then \
			cd build && cmake -G Ninja .. && ninja -j"$${_J:-$(_JOBS)}" $@; \
		else \
			mkdir -p build && cd build && cmake -G Ninja .. && ninja -j"$${_J:-$(_JOBS)}" $@; \
		fi)

clean:
	@$(_JOB_CMD) && \
	$(call time_wrapper, \
		if [ -d "build" ]; then \
			cd build && cmake -G Ninja .. && ninja -j"$${_J:-$(_JOBS)}" clean && cd ..; \
			rm -rf build; \
		fi)

orm:
	@$(_JOB_CMD) && \
	$(call time_wrapper, bin/orm dbproxy/xml dbproxy/data)
