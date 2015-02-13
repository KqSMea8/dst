pwd=`pwd`
conf_path=$pwd"/../../conf"

cp $conf_path/test.conf.$1 $pwd/configs
cp $conf_path/dstream.cfg.xml $pwd/configs
cp $conf_path/log.conf_pm_pn $pwd/configs/log.conf


cp $conf_path/log.conf configs/comp_topo_submit/echo_importer/
cp $conf_path/log.conf configs/comp_topo_submit/echo_exporter/
cp $conf_path/log.conf configs/comp_topo_submit/echo_pe_task1/
cp $conf_path/log.conf configs/comp_topo_submit/echo_pe_task2/
cp $conf_path/log.conf configs/comp_topo_submit/echo_pe_task3/
cp $conf_path/log.conf configs/comp_topo_submit/echo_pe_task4/


