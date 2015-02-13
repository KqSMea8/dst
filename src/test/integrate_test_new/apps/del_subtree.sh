[ $# -ne 1 ] && echo "sh sub_app.sh sub_tree"

date>>../operation.log
echo "delete subtree: $1" >>../operation.log

./dclient --conf client.cfg.xml --del-sub-tree -d $1

