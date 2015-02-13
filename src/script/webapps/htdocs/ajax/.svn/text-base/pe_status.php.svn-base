<?php
/***************************************************************************
 * 
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @author Yuncong Zhang(zhangyuncong@baidu.com)
 * @date 2012/07/27 11:28:01
 * @version 1.0 
 * @brief 
 *  
 **/
set_time_limit(10);
chdir("..");
$no_db=true;
include ("include/common.php");
$peidlist = $_GET['peid'];
$app_path = $_GET['path'];
$cmd='cd bin;source paths.sh;$DSTREAM_UTILS_DIR/data_dumper -c $DSTREAM_CONF_DIR/dstream.cfg.xml -p '.$app_path.' -e "'.$peidlist.'" -h `sh get_zk_addr.sh`';
$str = shell_exec($cmd);
$arr=array();
parse_protobuf_elems($str,$arr);
if ($arr['pe_status_list']){
	foreach($arr['pe_status_list'] as $pe){
		$peid = $pe['pe_id'][0]['id'][0];
		$metrics[$peid]['appid']=trim($pe['app_id'][0]['id'][0]);
		$metrics[$peid]['cpu_used']=trim($pe['cpu_used'][0])."%";
		$metrics[$peid]['memory_used']=trim($pe['memory_used'][0])."K";
	}
	print json_encode($metrics);
}else{
	print "{}";
}

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
?>
