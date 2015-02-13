<?php
/***************************************************************************
 * 
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file log_metrics.php
 * @author Yuncong Zhang(zhangyuncong@baidu.com)
 * @date 2012/04/10 11:43:10
 * @version 1.0 
 * @brief 
 *  
 **/
include("../conf/config.php");
include ("../include/function.php");
$mysqli = new mysqli("p:$db_host",$db_user,$db_password, $db_name);
if($_GET['do']=='add'){
	if(empty($_GET['view']) || empty($_GET['cmd']))
		json_exit("WrongArgs -- view name empty or cmd empty...",false);
	$view_name=$_GET['view'];
	$cmd=stripslashes($_GET['cmd']);
	if(strcasecmp(substr(trim($cmd),0,6),"select")){ json_exit("query_cmd must be a sentence start with select",false);}
	$mysqli->query("create view `$view_name` as $cmd") or json_exit($mysqli->error,false);	
    json_exit("add view success");
}else if ($_GET['do']=='delete'){
	$mysqli->query("drop view `$_GET[view]`") or json_exit("delete failed..".$mysqli->error,false);
	json_exit("delete sucess");
}







/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
?>
