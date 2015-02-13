<?php
session_start();
if($_SESSION['admin']==true){
$host=$_GET['host'];
$port=intval($_GET['port']);
$level=intval($_GET['level']);
$cmd="cd ../bin;sh ./pn_debugger.sh tcp://$host:$port set_log_level $level";
echo $cmd."\n";
system($cmd);
}else{
	echo "Permission Denied";
}
?>
