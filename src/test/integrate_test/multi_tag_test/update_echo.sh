###################
# update test_app
###################
echo "update test_app"
cp echo_importer configs/comp_topo_submit/echo_importer/
cp echo_exporter configs/comp_topo_submit/echo_exporter/
cp echo_pe_task configs/comp_topo_submit/echo_pe_task1/
cp echo_pe_task configs/comp_topo_submit/echo_pe_task2/
cp echo_pe_task configs/comp_topo_submit/echo_pe_task3/
cp echo_pe_task configs/comp_topo_submit/echo_pe_task4/
echo "update add_topo"
cp echo_exporter configs/comp_topo_submit_add+/echo_exporter_add/
