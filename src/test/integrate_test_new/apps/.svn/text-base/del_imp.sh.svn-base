[ $# -ne 1 ] && echo "sh sub_app.sh app_name"

date>>../operation.log
echo "submit app $1" >>../operation.log

./dclient --conf client.cfg.xml --del-importer -d dynamic_importer_test/$1



