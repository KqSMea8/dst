bin_path="../../../cc/output/bin/"

echo "update Client"
cp $bin_path/dstream_client configs/

echo "update PEs"
app_path=configs/echo_app1
cp echo_exporter $app_path/echo_exporter
cp echo_pe_task $app_path/echo_pe_task
cp echo_importer $app_path/echo_importer

app_path=configs/echo_app2
cp echo_exporter $app_path/echo_exporter
cp echo_pe_task $app_path/echo_pe_task
cp echo_importer $app_path/echo_importer

