[ $# -ne 1 ] && echo "sh sub_app.sh app_name"


echo -e "\n\n\n\n$1\n\n\n\n"
./dclient --conf dstream.cfg.xml --decommission-app --app $1


