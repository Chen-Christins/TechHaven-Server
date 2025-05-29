#!/bin/sh

command_error_exit() {
    $*
    if [ $? -ne 0 ]
    then
        exit 1
    fi
}

command_error_exit mkdir -p ~/apps
command_error_exit cd ~/apps
# 安装yaml-cpp
command_error_exit git clone https://github.com/jbeder/yaml-cpp.git
command_error_exit cd yaml-cpp
command_error_exit mkdir build && cd build
command_error_exit cmake -D BUILD_SHARED_LIBS=ON ..
command_error_exit make -j4
command_error_exit sudo make install

command_error_exit cd ~/apps

# 安装ragel-6.10
command_error_exit curl -O http://www.colm.net/files/ragel/ragel-6.10.tar.gz
command_error_exit tar -zxvf ragel-6.10.tar.gz
command_error_exit cd ragel-6.10
command_error_exit sudo yum install libtool gcc g++ autoconf automake
command_error_exit ./configure
command_error_exit make -j4
command_error_exit sudo make install

command_error_exit cd ~/apps

# 安装tinyxml2
command_error_exit git clone https://github.com/leethomason/tinyxml2.git
command_error_exit cd tinyxml2
command_error_exit mkdir build
command_error_exit cd build
command_error_exit cmake -D BUILD_SHARED_LIBS=ON ..
command_error_exit make -j4
command_error_exit sudo make install

command_error_exit cd ~/apps

# 安装hiredis_vip
command_error_exit git clone https://github.com/vipshop/hiredis-vip.git
command_error_exit cd hiredis-vip
command_error_exit make -j4
command_error_exit sudo make install
command_error_exit sudo cp /usr/local/lib/*hiredis* /usr/local/lib64/

command_error_exit cd ~/apps

# 安装其他相关的依赖库
command_error_exit sudo yum install boost-devel sqlite-devel openssl-devel libevent-devel

echo "the dependencies has successfully installed at ~/apps." 
