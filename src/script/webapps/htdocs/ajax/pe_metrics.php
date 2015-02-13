<?php
/***************************************************************************
 * 
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file count_pe.php
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
if (!isset($peidlist)) {
	 $peidlist = $_POST['peid'];
}
$app_path = $_GET['path'];
$cmd='cd bin;source paths.sh;$DSTREAM_UTILS_DIR/data_dumper -c $DSTREAM_CONF_DIR/dstream.cfg.xml -p '.$app_path.' -e "'.$peidlist.'" -h `sh get_zk_addr.sh`';
$str = shell_exec($cmd);
$arr=array();
parse_protobuf_elems($str,$arr);

if ($arr['pe_status_list']){
	foreach($arr['pe_status_list'] as $pe){
		if ($pe['metric_name']) foreach($pe['metric_name'] as $m_index => $name){
			$metrics[$pe['pe_id'][0]['id'][0]][trim($name)]=trim($pe['metric_value'][$m_index]);
		}
		if ($pe['revision']) {
		    $metrics[$pe['pe_id'][0]['id'][0]]['revision'] = $pe['revision'][0];
		}
	}
	print json_encode($metrics);
}else{
	print "{}";
}

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
?>
