<?php

date_default_timezone_set('Asia/Shanghai');
require_once("../conf/config.php");

$appid = isset($_GET['appid']) ? trim($_GET['appid']) : '';
if (empty($appid)) {
    echo '[]';
    die;
}

// one minute late
$timestamp = time();
$time = $timestamp - $timestamp % 60 - 60;
$start_time = date('Y-m-d H:i:s', $time);

$mysqli = new mysqli("p:$db_host" ,$db_user,$db_password, $db_name);
if ($mysqli->connect_error) {
    die($mysqli->connect_error);
}

$sql_cmd = 'select cluster_name as cluster, machine_name as host, c_time as time,';
$sql_cmd .= 'c_name as name, c_attr1 as peid, c_attr2 as appid, c_attr3 as processorid, c_value as value from counter ';
$sql_cmd .= "where c_time = '$start_time' and c_attr2='$appid'";

$result = $mysqli->query($sql_cmd) or die($mysqli->error);
while ($row = $result->fetch_assoc()) {
    $ret[] = $row;
}
echo json_encode($ret);
