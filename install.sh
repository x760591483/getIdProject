#!/bin/bash

path=`pwd`

cd ./tarfile

tar xzf json-c-0.9.tar.gz
cd json-c-0.9
./configure --prefix=$path
make -j 8
make install
cd $path/tarfile
rm json-c-0.9 -rf

tar xzf libevent-2.0.21-stable.tar.gz
cd libevent-2.0.21-stable
./configure --prefix=$path
make -j 8
make install
cd $path/tarfile
rm libevent-2.0.21-stable -rf

tar zxf libevhtp.tar.gz
cd libevhtp
cmake -DCMAKE_PREFIX_PATH=$path -DCMAKE_INSTALL_PREFIX=$path
make -j 8
make install
cd $path/tarfile
rm libevhtp -rf 

cd $path/src
cp ./liblogLog.so ../lib
make
cp ./idget ./run.sh ./stop.sh ./idget.conf $path/bin 

echo ">>>>>>>>>>ok<<<<<<<<<<<"


