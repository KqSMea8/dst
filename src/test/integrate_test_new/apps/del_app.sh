[ $# -ne 1 ] && echo "sh sub_app.sh app_name"

date>>../operation.log
echo "decommission app $1" >>../operation.log

echo -e "\n\n\n\n$1\n\n\n\n"
./dclient --conf client.cfg.xml --decommission-app --app $1


