#!/bin/bash

get_bigpipe()
{
    ########################################
    #bigpipe
    if [ ! -e ../../include/bigpipe ];then
        tar xzvf bigpipe_capi.tar.gz
        mkdir -p ../include/bigpipe
        echo "cp -r bigpipe_capi/include/* ../include/bigpipe"
        cp -r bigpipe_capi/include/* ../include/bigpipe
        mkdir -p ../lib/
        echo "cp bigpipe_capi/lib/libbigpipe_api.so ../lib"
        cp bigpipe_capi/lib/libbigpipe_api.so ../lib
	rm -rf bigpipe_capi
    fi
}

save_gcc_version_file="build_gcc_version"
gcc_version=`gcc --version | awk 'NR==1{print}'`
build_gcc_version=`cat $save_gcc_version_file`
if [ "$gcc_version" != "$build_gcc_version" ];then
    rm deps.tgz
    rm -rf ../include/
    rm -rf ../bin/
    rm -rf ../lib/
elif [ "`cat third_lib_md5sum`" == "`md5sum *.tar.*`" ];then
    if [ ! -e ../include/ -o ! -e ../bin -o ! -e ../lib/ ];then
        tar zxf deps.tgz -C ../
    fi
    exit 0
fi

########################################
# build tinyxml
if [ ! -e tinyxml ] || ! grep "`md5sum tinyxml_2_6_1.tar.gz`" third_lib_md5sum &>/dev/null; then
    tar xzf tinyxml_2_6_1.tar.gz
    cd tinyxml
    make -j 16
    cd ..
    mkdir -p ../include/tinyxml
    echo "cp tinyxml/tinyxml.h tinyxml/tinystr.h ../include/tinyxml"
    cp tinyxml/tinyxml.h tinyxml/tinystr.h ../include/tinyxml
    mkdir -p ../lib
    echo "cp tinyxml/libtinyxml.so ../lib"
    cp tinyxml/libtinyxml.so ../lib
fi

########################################
# zeromq
if [ ! -e zeromq-2.2.0 ] || ! grep "`md5sum zeromq-2.2.0.tar.gz`" third_lib_md5sum &>/dev/null ;then
    tar xzvf zeromq-2.2.0.tar.gz
    patch -p0 zeromq-2.2.0/src/config.hpp < zeromq-2.2.0-dstream001-change_max_sockets.patch
    cd zeromq-2.2.0
    ./configure --prefix=`pwd`/../zeromq --enable-shared  
    make -j 16 && make install 
    cd ../
    mkdir -p ../include/zeromq
    echo "cp zeromq/include/zmq.* ../include/zeromq"
    cp zeromq/include/zmq.* ../include/zeromq
    mkdir -p ../lib/
    echo "cp zeromq/lib/libzmq* ../lib/"
    cp -d zeromq/lib/libzmq* ../lib/
fi

########################################
# protobuf
#if [ ! -e protobuf-2.4.1 ] || ! grep "`md5sum protobuf-2.4.1.tar.bz2`" third_lib_md5sum &>/dev/null;then
#    tar xjvf protobuf-2.4.1.tar.bz2
#    cd protobuf-2.4.1
#    # use -O2 to improve performance.
#    CXXFLAGS="-fPIC -O2 -g" ./configure --prefix=`pwd`/../protobuf --enable-shared --disable-static
#    make -j16 && make install
#    cd ../
#    mkdir -p ../include/protobuf
#    echo "cp -r protobuf/include/* ../include/protobuf"
#    cp -r protobuf/include/* ../include/protobuf
#    mkdir -p ../lib
#    echo "cp protobuf/lib/libproto* ../lib"
#    cp protobuf/lib/libproto* ../lib
#    mkdir -p ../bin
#    echo "cp protobuf/bin/protoc ../bin"
#    cp protobuf/bin/protoc ../bin
#fi

########################################
#log4cpp
#if [ ! -e log4cpp-1.0 ]  || ! grep "`md5sum log4cpp-1.0.tar.gz`" third_lib_md5sum &>/dev/null;then
#    tar xzvf log4cpp-1.0.tar.gz
#    cd log4cpp-1.0
#    # use -O2 to improve performance.
#    CXXFLAGS="-fPIC -O2 -g" ./configure --prefix=`pwd`/../log4cpp --enable-shared --disable-static
#    make -j 16 && make install
#    cd ../
#    mkdir -p ../include/log4cpp
#    echo "cp -r log4cpp/include/log4cpp/* ../include/log4cpp"
#    cp -r log4cpp/include/log4cpp/* ../include/log4cpp
#    mkdir -p ../lib
#    echo "cp -r log4cpp/lib/liblog4cpp* ../lib"
#    cp -d log4cpp/lib/liblog4cpp* ../lib # follow symbolic link.
#fi

########################################
#log4cplus
if [ ! -e log4cplus-1.1.0-rc4 ]  || ! grep "`md5sum log4cplus-1.1.0-rc4.tar.gz`" third_lib_md5sum &>/dev/null;then
    tar xzvf log4cplus-1.1.0-rc4.tar.gz
    cd log4cplus-1.1.0-rc4
    # use -O2 to improve performance.
    CXXFLAGS="-fPIC -O2 -g" ./configure --prefix=`pwd`/../log4cplus --enable-shared --disable-static
    make -j 16 && make install
    cd ../
    mkdir -p ../include/log4cplus
    echo "cp -r log4cplus/include/log4cplus/* ../include/log4cplus"
    cp -r log4cplus/include/log4cplus/* ../include/log4cplus
    mkdir -p ../lib
    echo "cp -r log4cplus/lib/liblog4cplus* ../lib"
    cp -d log4cplus/lib/liblog4cplus* ../lib # follow symbolic link.
fi

########################################
#zookeeper
#if [ ! -e zookeeper-3.3.3 ]  || ! grep "`md5sum zookeeper-3.3.3.tar.gz`" third_lib_md5sum &>/dev/null ;then
#    tar xzvf zookeeper-3.3.3.tar.gz
#    #patch -d zookeeper-3.3.3 -p0 < patches/ZOOKEEPER-981.patch
#    cd zookeeper-3.3.3/src/c
#    ./configure --prefix=`pwd`/../../../zookeeper --enable-shared --disable-static
#    make -j 16 && make install
#    cd ../../../
#    mkdir -p ../include/zookeeper
#    echo "cp zookeeper/include/c-client-src/* ../include/zookeeper"
#    cp zookeeper/include/c-client-src/* ../include/zookeeper
#    mkdir -p ../lib/
#    echo "cp zookeeper/lib/libzookeeper* ../lib"
#    cp zookeeper/lib/libzookeeper* ../lib
#fi

########################################
#libev
if [ ! -e libev-4.11 ]  || ! grep "`md5sum libev-4.11.tar.gz`" third_lib_md5sum &>/dev/null;then
    tar xzvf libev-4.11.tar.gz
    cd libev-4.11
    # use -O2 to improve performance.
    CXXFLAGS="-fPIC -O2 -g" ./configure --prefix=`pwd`/../../ --enable-shared --disable-static
    make && make install
    cd ../
    mkdir -p ../include/libev
    mv ../include/ev*.h ../include/libev/
fi


########################################
#tcmalloc
#if [ ! -e google-perftools-1.8.3 ] || ! grep "`md5sum google-perftools-1.8.3.tar.gz `" third_lib_md5sum &>/dev/null ;then
#    tar xzvf google-perftools-1.8.3.tar.gz 
#    cd google-perftools-1.8.3
#    ./configure --prefix=`pwd`/../tcmalloc --enable-shared
#    make -j 16 && make install
#    cd ../
#    mkdir -p ../include/tcmalloc
#    echo "cp -r tcmalloc/include/* ../include/tcmalloc"
#    cp -r tcmalloc/include/* ../include/tcmalloc
#    mkdir -p ../lib
#    echo "cp tcmalloc/lib/libtcmalloc* ../lib"
#    cp tcmalloc/lib/libtcmalloc* ../lib
#fi

#get_bigpipe
#for dir in `ls -al | grep ^d | awk '{print }'`;do
#    if [ "$dir" == "." -o "$dir" == ".." -o "$dir" == ".svn" ];then
#	continue
#    fi
#    rm -rf $dir
#done
########################################
#mongoose
if [ ! -e mongoose ] || ! grep "`md5sum mongoose-3.8.tar.gz`" third_lib_md5sum &>/dev/null;then
    tar xzvf mongoose-3.8.tar.gz
    cd mongoose
    make
    cd ..
fi

cd ../
tar zcf deps.tgz include bin lib
mv deps.tgz src
cd src
echo "$gcc_version" >$save_gcc_version_file
md5sum *.tar.* >third_lib_md5sum

