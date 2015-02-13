#!/bin/bash

if [ "$dstream_root" == "" ];then
    dstream_root="../"
fi

# conf
WEBAPP_DIR=`pwd`
WEBAPP_PORT=8090
HTDOCS_ROOT=$WEBAPP_DIR/htdocs

if [ -e $dstream_root/conf/dstream.conf ];then
    source $dstream_root/conf/dstream.conf
    WEBAPP_PORT=$webapps_port
fi

# build begin
MYSQL_PKG=mysql-5.1.30.tar.gz
MYSQL_INSTALL=$WEBAPP_DIR/lnmp/mysql

PCRE_PKG=pcre-7.7.tar.gz
NGINX_PKG=nginx-1.2.6.tar.gz
NGINX_INSTALL=$WEBAPP_DIR/lnmp/nginx

PHP_PKG=php-5.4.11.tar.gz
PHP_INSTALL=$WEBAPP_DIR/lnmp/php

# check error
set -e

# build mysql
echo 'building mysql...'
cd $WEBAPP_DIR/deps
tar zxvf $MYSQL_PKG && cd ${MYSQL_PKG%.tar.gz}
sh ./BUILD/compile-amd64-max --prefix=$MYSQL_INSTALL
make -j8 && make install
mkdir $MYSQL_INSTALL/etc && mkdir $MYSQL_INSTALL/var
sed -e "s@{INSTALL_PATH}@${MYSQL_INSTALL}@g" $WEBAPP_DIR/deps/conf/my.cnf > $MYSQL_INSTALL/etc/my.cnf 
echo 'install mysql done.'

# building nginx
echo 'building nginx...'
cd $WEBAPP_DIR/deps
tar zxvf $PCRE_PKG 
tar zxvf $NGINX_PKG && cd ${NGINX_PKG%.tar.gz}
./configure --prefix=$NGINX_INSTALL --with-pcre=$WEBAPP_DIR/deps/${PCRE_PKG%.tar.gz}
make -j8 && make install
cp $WEBAPP_DIR/deps/conf/nginx.conf $NGINX_INSTALL/conf/ && mkdir $NGINX_INSTALL/conf/vhost
cp $WEBAPP_DIR/deps/conf/vhost.conf $NGINX_INSTALL/conf/vhost/

sed -i "s@listen@listen ${WEBAPP_PORT}@g" $NGINX_INSTALL/conf/vhost/vhost.conf
sed -i "s@server_name@server_name $(hostname)@g" $NGINX_INSTALL/conf/vhost/vhost.conf
sed -i "s@root@root $HTDOCS_ROOT@g" $NGINX_INSTALL/conf/vhost/vhost.conf
sed -i "s@access_log@access_log $(hostname).log@g" $NGINX_INSTALL/conf/vhost/vhost.conf
sed -i "s@return@return http://$(hostname):${WEBAPP_PORT}@g" $NGINX_INSTALL/conf/vhost/vhost.conf

echo 'build nginx done.'

# building php
cd $WEBAPP_DIR/deps
tar zxvf $PHP_PKG && cd ${PHP_PKG%.tar.gz}
./configure --prefix=$PHP_INSTALL --with-mysql=$MYSQL_INSTALL --enable-fpm \
            --with-mysqli=$MYSQL_INSTALL/bin/mysql_config
make -j8 && make install
cp $PHP_INSTALL/etc/php-fpm.conf.default $PHP_INSTALL/etc/php-fpm.conf
cp $WEBAPP_DIR/deps/conf/php-fpm.conf $PHP_INSTALL/etc/php-fpm.conf
mkdir -p $PHP_INSTALL/log

cd $WEBAPP_DIR/htdocs/
sh build.sh

echo "Install Complete!"

