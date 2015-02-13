[ $# -lt 2 ] && echo "roll_back_processor.sh app_name revision"

date>>../operation.log
echo "roll back:$1 revision:$2 maxvision:$3" >>../operation.log

./dclient --conf client.cfg.xml --update-processor -d $1 --revision $2
