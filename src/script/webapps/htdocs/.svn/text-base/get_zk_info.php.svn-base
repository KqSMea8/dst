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
set_time_limit(5);
////////////////// Edit Area Begin//////////////////////
if ($_GET['path'] && empty($_GET['zk_path'])) $_GET['zk_path']=$_GET['path'];
$zk_path = $_GET['zk_path']?$_GET['zk_path']:"/DStream";

$addr=$_GET['addr'];
$max_depth=$_GET['depth'];
if (empty($addr)){
  $addr = trim(shell_exec("cd bin;sh get_zk_addr.sh"));
}
$TPL['menu']=array();
$TPL['menu_title']="Job Display";
$TPL['menu_title_link']="index.php?path=$path";

$TPL['title']="Job Display";
$TPL['right_menu']['Job Display']='index.php';
$TPL['right_menu']['Log Metrics Display']='log_metrics.php';
$TPL['active_right_menu']='Job Display';
$cmd = 'cd bin;source paths.sh;export LD_LIBRARY_PATH=$DSTREAM_LIB_DIR:$LD_LIBRARY_PATH;$DSTREAM_UTILS_DIR/get_zk_info  "'.$addr."\" \"$zk_path\" ".$max_depth;
//print $cmd;
$output=shell_exec($cmd);
$TPL['tpl_name']='get_zk_info';

////////////////// Edit Area End  //////////////////////
$tpl_name=$TPL['tpl_name'];
$tpl_dir=$GLOBALS['tpl_dir'];
include("$tpl_dir/header.tpl.php");
include("$tpl_dir/$tpl_name.tpl.php");
include("$tpl_dir/footer.tpl.php");

?>
