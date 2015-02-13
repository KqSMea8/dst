<?php
$no_db=true;
include_once("include/common.php");
get_zk_data($_GET['path'],intval($_GET['appid']),$zk_data);
$ret = get_path_dir($_GET['type'],$_GET['id'],$zk_data);
$ret_dir=$ret['dir'];
$host=$ret['host'];
$port=$ret['port'];
$req_dir = $_GET['dir'];

$dir=comb_dir($ret_dir,$req_dir);
header('Content-Description: File Transfer');
header('Content-Type: application/octet-stream');
header('Content-Disposition: attachment; filename='.basename($dir));
header('Content-Transfer-Encoding: binary');
header('Expires: 0');
header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
header('Pragma: public');
ob_clean();
flush();
readfile("http://${host}:${port}/down/$dir");
?>
