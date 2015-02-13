[ $# -ne 1 ] && echo "sh sub_app.sh sub_tree"

date>>../operation.log
echo "add sub tree:$1" >>../operation.log

./dclient --conf client.cfg.xml --add-sub-tree -d $1

