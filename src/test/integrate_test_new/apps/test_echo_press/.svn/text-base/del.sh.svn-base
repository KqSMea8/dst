#!bin/bash

apps=(echo_press1 echo_press2 echo_press3 echo_press4 echo_press5 echo_press6)

for((i=0;i<${#apps[@]};i++));do
    ./dclient --conf ./dstream.cfg.xml --decommission-app --app test_${apps[i]} >> /dev/null
    sleep 2
done
exit 0

