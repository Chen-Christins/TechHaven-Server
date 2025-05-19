#!/bin/sh

cd ~
mkdir -p apps
# 安装yaml-cpp
git clone https://github.com/jbeder/yaml-cpp.git
cd yaml-cpp
mkdir build && cd build
cmake -D BUILD_SHARED_LIBS=ON ..
make -j4
sudo make install
cd ~/apps

# 安装ragel-6.10
curl -O http://www.colm.net/files/ragel/ragel-6.10.tar.gz
extract ragel-6.10.tar.gz
cd ragel-6.10
sudo yum install libtool gcc g++ autoconf automake
./configure
make -j4
sudo make install
which ragel
cd ~/apps

# 安装tinyxml2
git clone https://github.com/leethomason/tinyxml2.git
cd tinyxml2
mkdir build
cd build
cmake -D BUILD_SHARED_LIBS=ON ..
make -j4
sudo make install
cd ~/apps

# 安装hiredis_vip
git clone https://github.com/vipshop/hiredis-vip.git
cd hiredis-vip
make -j4
sudo make install
sudo cp /usr/local/lib/*hiredis* /usr/local/lib64/
cd ~/apps

# 安装其他相关的依赖库
sudo yum install boost-devel sqlite-devel openssl-devel
