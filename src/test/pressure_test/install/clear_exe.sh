kill -9 `ps ux | grep "memcheck" | grep -v "grep" |awk '{print $2}'`
kill -9 `ps ux | grep "pn_press" | grep -v "grep" |awk '{print $2}'`
kill -9 `ps ux | grep "valgrind" | grep -v "grep" |awk '{print $2}'`
killall echo_importer
killall echo_pe_task
killall echo_exporter
