[ $# -ne 1 ] && echo "sh sub_app.sh app_name"

retry=0
date>>../operation.log
echo "submit app $1" >>../operation.log

while((1))
do
	./dclient --conf client.cfg.xml --add-importer -d dynamic_importer_test/$1
	if [ $? -eq 0 ]
	then
		if [ $retry -ne 0 ]
		then
			echo "[lyjmark]retry for $retry times & success"
		fi
		exit 0
	elif [ $retry -eq 3 ]
	then
		echo "[lyjmark]retry for $retry times & fail!"
		exit $retry
	else
		retry=$[retry+1]
	fi
done

