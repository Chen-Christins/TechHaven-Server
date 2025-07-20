# 使用官方的Ubuntu基础镜像
FROM ubuntu:22.04

# 设置时区
ENV TZ=Asia/Shanghai
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# 安装必要的工具和依赖
RUN apt-get update && apt-get install -y \
	apt-utils \
	vim \
    g++ \
    cmake \
    make \
	git \
	wget \
    libboost-all-dev \
	libsqlite3-dev \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

RUN wget https://www.colm.net/files/ragel/ragel-6.10.tar.gz && \
    tar -xzf ragel-6.10.tar.gz && \
    cd ragel-6.10 && \
    ./configure && make -j$(nproc) && make install && \
    cd .. && rm -rf ragel-6.10*

RUN git clone https://github.com/jbeder/yaml-cpp.git && \
	cd yaml-cpp && \
	mkdir build && cd build && \
	cmake -D BUILD_SHARED_LIBS=ON .. && \
	make -j$(nproc) && make install

RUN git clone https://github.com/leethomason/tinyxml2.git && \
	cd tinyxml2 && \
	mkdir build && cd build && \
	cmake -D BUILD_SHARED_LIBS=ON .. && \
	make -j$(nproc) && make install 

RUN git clone https://github.com/vipshop/hiredis-vip.git && \
	cd hiredis-vip && \
	make -j$(nproc) && make install 

RUN git clone https://github.com/Chen-Christins/jsoncpp.git && \
	cd jsoncpp && \
	mkdir build && cd build && \
	cmake -D BUILD_SHARED_LIBS=ON .. && \
	make -j$(nproc) && make install

# 设置工作目录
WORKDIR /home/chen/workspace/Blog

# 将当前目录内容复制到容器中
COPY . .

# 编译项目
RUN make && make -j$(nproc)

# 暴露应用程序运行的端口
EXPOSE 8080

# 设置容器启动时执行的命令
# CMD ["tail", "-f", "/dev/null"]
CMD ["bin/blog", "-d"]