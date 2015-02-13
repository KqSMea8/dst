<?php
/***************************************************************************
 * 
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
/**
 * @file api_app_pe.php
 * @author Yuncong Zhang(zhangyuncong@baidu.com)
 * @date 2012/05/04 14:51:07
 * @version 1.0 
 * @brief 
 *  
 **/
chdir("..");
$default_path="/DStream";
include("include/common.php");
$path=$_GET['path']?$_GET['path']:$default_path;
if ($_GET['show']) {
  $show=$_GET['show'];
}

// get data
$appid=intval($_GET['appid']);
$arr=array();
get_zk_data($path,$appid,$arr);
$apps=$arr['apps'];
$app=$apps[$appid];

// output
if ($show && $show == "json") {
  // json print
  if ($app['pelist']) { 
    foreach ($app['pelist'] as $pe) {
      $output_data[$pe["peid"]] = $pe["processor"]["name"];
    }
  }
  print json_encode($output_data);
} else {
  // plat print
  if($app['pelist']) foreach($app['pelist'] as $pe){
	  echo $pe["peid"]."|".$pe["processor"]["name"]."\n";
  }
}

?>
