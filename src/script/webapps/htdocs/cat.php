<?php
/***************************************************************************
 * 
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
/**
 * @file index.php
 * @author Yuncong Zhang(zhangyuncong@baidu.com)
 * @date 2012/05/02 12:24:24
 * @version 1.0 
 * @brief 
 *  
 **/
include ("include/common.php");
////////////////// Edit Area Begin//////////////////////

$path=$app_path = $_GET['path']?$_GET['path']:"/DStream";
$TPL['menu']=array();
$TPL['menu_title']="Job Display";
$TPL['menu_title_link']="index.php?path=$path";

$TPL['title']="Job Display";
$TPL['right_menu']['Job Display']='index.php';
$TPL['right_menu']['Log Metrics Display']='log_metrics.php';
$TPL['active_right_menu']='Job Display';

$TPL['menu_search_form']=array();
$TPL['menu_search_form']['action']="";
$TPL['menu_search_form']['method']="GET";
$TPL['menu_search_form']['search_name']="path";
$TPL['menu_search_form']['default_text']="$path";
get_zk_data($_GET['path'],intval($_GET['appid']),$zk_data);
$ret = get_path_dir($_GET['type'],$_GET['id'],$zk_data);
$ret_dir=$ret['dir'];
$host=$ret['host'];
$port=$ret['port'];
$req_dir = $_GET['dir'];


$dir=comb_dir($ret_dir,$req_dir);
@$xml=simplexml_load_file("http://$host:$port/cat/$dir");
$down_url = "down.php?appid=$_GET[appid]&path=$_GET[path]&type=$_GET[type]&id=$_GET[id]&dir=$req_dir";

$TPL['tpl_name']='cat';

////////////////// Edit Area End  //////////////////////
$tpl_name=$TPL['tpl_name'];
$tpl_dir=$GLOBALS['tpl_dir'];
include("$tpl_dir/header.tpl.php");
include("$tpl_dir/$tpl_name.tpl.php");
include("$tpl_dir/footer.tpl.php");

?>
