if [ $# -eq 1 ] ; then
	MYSQL_BIN=$1
else
	MYSQL_BIN="../lnmp/mysql/bin/mysql"
fi

$MYSQL_BIN -u root -p'123456' < initial.sql

