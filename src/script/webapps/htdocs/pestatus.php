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
$no_db=true;
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
$backuppeid = $_GET['backuppeid'];
$app_path=$_GET['path']?$_GET['path']:"/DStream";
$zk_data=array();
// use $backuppeid >> 40 to get the APPID
$appid=$backuppeid>>40;
get_zk_data($app_path,$appid,$zk_data);
$processor=$zk_data['apps'][$appid]['pelist'][$backuppeid]['processor'];
$str = shell_exec("cd bin;/bin/bash ./metrics_dumper.sh -p $app_path -e $backuppeid");
parse_protobuf_elems($str,$arr);
foreach($arr['pe_status_list'][0] as $key => $value){
	$status[$key]=$value[0];
}
$status=array_merge($processor,$status);
$status['metric_name']=$arr['pe_status_list'][0]['metric_name'];
$status['metric_value']=$arr['pe_status_list'][0]['metric_value'];
$status['revision']=$arr['pe_status_list'][0]['revision'][0];

$TPL['tpl_name']='pestatus';

////////////////// Edit Area End  //////////////////////
$tpl_name=$TPL['tpl_name'];
$tpl_dir=$GLOBALS['tpl_dir'];
include("$tpl_dir/header.tpl.php");
include("$tpl_dir/$tpl_name.tpl.php");
include("$tpl_dir/footer.tpl.php");

?>
