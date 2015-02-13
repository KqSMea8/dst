#/bin/bash

#set -e

WEBAPP_DIR=`pwd`
HTDOCS_ROOT=$WEBAPP_DIR/htdocs
MYSQL_INSTALL=$WEBAPP_DIR/lnmp/mysql
NGINX_INSTALL=$WEBAPP_DIR/lnmp/nginx
PHP_INSTALL=$WEBAPP_DIR/lnmp/php
MYSQL_PASSWD=123456

function print_help()
{
    echo $0 'start | stop | init'
    exit 0
}

function web_start()
{
    echo "starting web apps..."
    cd $MYSQL_INSTALL && $MYSQL_INSTALL/bin/mysqld_safe &
    sleep 5
    echo 'start mysql done.'

    cd $PHP_INSTALL && $PHP_INSTALL/sbin/php-fpm 
    echo 'start php-fpm done.'

    cd $NGINX_INSTALL && $NGINX_INSTALL/sbin/nginx
    echo 'start nginx done.'
}

function web_stop()
{
    echo "stopping web apps..."
    cd $NGINX_INSTALL && $NGINX_INSTALL/sbin/nginx -s stop
    echo "stop nginx done."

    cd $PHP_INSTALL && kill -15 `cat $PHP_INSTALL/var/run/php-fpm.pid`
    echo "stop php-fpm done."

    cd $MYSQL_INSTALL && $MYSQL_INSTALL/bin/mysqladmin -uroot -p$MYSQL_PASSWD shutdown
    echo "stop mysql done."
}

function web_init()
{
    echo "init web apps..."
    cd $MYSQL_INSTALL && $MYSQL_INSTALL/bin/mysql_install_db 
    cd $MYSQL_INSTALL && $MYSQL_INSTALL/bin/mysqld_safe &
    sleep 5
    cd $MYSQL_INSTALL && $MYSQL_INSTALL/bin/mysqladmin -u root password $MYSQL_PASSWD
    cd $HTDOCS_ROOT && sh setup.sh
    cd $MYSQL_INSTALL && $MYSQL_INSTALL/bin/mysqladmin -uroot -p$MYSQL_PASSWD shutdown
    echo 'init web apps done.'
}

case "x$1" in

"xstop" ) web_stop
        ;;

"xstart" ) web_start
        ;;

"xinit" ) web_init
        ;;

* ) print_help
        ;;
esac

