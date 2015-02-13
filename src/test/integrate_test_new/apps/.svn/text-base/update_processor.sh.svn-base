[ $# -ne 1 ] && echo "update_processor.sh app_name"

date>>../operation.log
echo "update processor $1" >>../operation.log

./dclient --conf client.cfg.xml --update-processor -d $1
