<?php
/***************************************************************************
 * 
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file common.php
 * @author Yuncong Zhang(zhangyuncong@baidu.com)
 * @date 2012/05/02 12:27:21
 * @version 1.0 
 * @brief 
 *  
 **/
$TPL['site_name']="DStream Displayer";
include ("conf/config.php");
include ("function.php");
include ("parser.php");
if(!$no_db){
	$mysqli = new mysqli("p:$db_host",$db_user,$db_password, $db_name);
    if ($mysqli->connect_error) {
        die($mysqli->connect_error);
    }
}
$TPL['right_menu']['Job Display']='index.php';
$TPL['right_menu']['Log Metrics Display']='?';
$TPL['right_menu']['HDFS Display']=$hdfs_display_url;
$TPL['right_menu']['ZooKeeper Display']=$zookeeper_display_url;
$TPL['right_menu']['BigPipe Display']=$bigpipe_display_url;
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
session_start();
if (empty($_GET['path'])){
	$_GET['path']=$_SESSION['path'];
}else{
$_SESSION['path']=$_GET['path'];
}
$app_stat = array("SUBMIT",  //0
                  "RUN",     //1
                  "KILL",    //2
                  "ADDTOPO", //3
                  "DELTOPO", //4
                  "UPPARA"); //5
class Benchmark {
    var $bench_file = "benchmark";
    var $start_time;
    var $end_time;
    function __construct($filename) {
        $this->bench_file = $filename;
        $this->start_time = 0;
        $this->end_time =0;
    }
    function microtime_float()  {
        list($usec, $sec) = explode(" ", microtime());
        return ((float)$usec + (float)$sec);
    }

    function start() {
        $this->start_time = $this->microtime_float();
    }
    function stop() {
        $this->end_time = $this->microtime_float();
    }
    function tlog($str) {
        $time = $this->end_time - $this->start_time;
        error_log("===============================\n", 3, $this->bench_file);
        error_log($str.":".$time."\n", 3, $this->bench_file);
    }
}
date_default_timezone_set('Asia/Shanghai');

?>
