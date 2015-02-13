<?php
/***************************************************************************
 * 
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file sqlcmd.php
 * @author Yuncong Zhang(zhangyuncong@baidu.com)
 * @date 2012/04/05 10:29:29
 * @version 1.0 
 * @brief 
 *  
 **/
include "conf/config.php";

$mysqli = new mysqli("p:$db_host", $db_user, $db_password, $db_name);
if ($mysqli->connect_error) {
    die($mysqli->connect_error);
}

if (isset($_POST['db'])){
	$db_name = $_POST['db'];
    $mysqli->select_db($db_name);
}
if(isset($_POST['cmd'])){
    if($mysqli->query(stripslashes($_POST['cmd']))) {
        echo "OK";
    } else {
        echo $mysqli->error;
    }
} else if (isset($_POST['find'])) {
    $ret = $mysqli->query(stripslashes($_POST['find']));
    $ret_json = array();
    while ($row = $ret->fetch_row()) {
        $ret_json[] = $row;
    }
    echo json_encode($ret_json);
} else {
echo 'ERROR';
}

?>
