<?php
/***************************************************************************
 * 
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
/**
 * @file index.php
 * @author Bijia Lan(lanbijia@baidu.com)
 * @date 2012/12/04 15:24:24
 * @version 1.0 
 * @brief 
 *  
 **/
$no_db = true;
include ("include/common.php");
////////////////// Edit Area Begin//////////////////////

$path=$app_path = $_GET['path']?$_GET['path']:"/DStream";
$appid=$_GET['appid'];
$TPL['menu']=array();
//$TPL['menu']['Chart Display']="./chart/index.php?path=$session_path&appid=$appid";
$TPL['menu_title']="Job Display";

$TPL['menu_title_link']="index.php?path=$path";

$TPL['title']="Job Display";
$TPL['right_menu']['Job Display']='index.php';
$TPL['right_menu']['Log Metrics Display']='log_metrics.php';
$TPL['active_right_menu']='Job Display';

$TPL['menu_search_form']=array();
$TPL['menu_search_form']['action']= "";
$TPL['menu_search_form']['method']="GET";
$TPL['menu_search_form']['search_name']="path";
$TPL['menu_search_form']['default_text']="$path";

$appid=isset($_GET['appid']) ? $_GET['appid'] :"";

// $str=shell_exec("cd bin;/bin/bash ./dumpshell.sh \"$app_path\" $appid ");
// parse_all($str,$arr);
// $cur_app=$arr['apps'][$appid];

$str=shell_exec("cd bin;/bin/bash ./dump_processor.sh $appid $path");
$all_processor=array();
$processor_count=0;
parse_app_processor_without_head($str);
while ($str != '' && parse_app_processor($str,$processor)) {
  $all_processor[$processor_count]=$processor;
  $processor_count++;
}
$str = shell_exec("cd bin;/bin/bash ./dumpshell.sh $path $appid topology");
$raw_str_array = explode("\n",$str);
$raw_len = count($raw_str_array);
$node_info_array = array();
$topo='[]';
if($raw_len > 3) {
    $topo_with_prefix = $raw_str_array[1];
    $topo = substr($topo_with_prefix, 9);
    for ($i = 3; $i < $raw_len; $i++) {
        if(strlen($raw_str_array[$i]) > 6) {
            $node_info_array[] = $raw_str_array[$i];
        }
    }
}
$TPL['tpl_name'] = 'processorlist';

////////////////// Edit Area End  //////////////////////
$tpl_name=$TPL['tpl_name'];
$tpl_dir=$GLOBALS['tpl_dir'];
include("$tpl_dir/header.tpl.php");
include("$tpl_dir/$tpl_name.tpl.php");
include("$tpl_dir/dag.tpl.php");
include("$tpl_dir/footer.tpl.php");

?>
