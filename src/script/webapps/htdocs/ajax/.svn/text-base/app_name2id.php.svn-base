<?php
/***************************************************************************
 * 
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
set_time_limit(10);
chdir("..");
$no_db=true;
include ("include/common.php");

$app_name = isset($_GET['name']) ? trim($_GET['name']): '';
$app_path = isset($_GET['path']) ? trim($_GET['path']): '';
if (empty($app_name) || empty($app_path)) {
    echo "[]";
    exit;
}

$cmd='cd bin;source paths.sh;$DSTREAM_UTILS_DIR/zk_meta_dumper -p '.$app_path . ' -h `sh get_zk_addr.sh`';
$str = shell_exec($cmd);
$arr=array();
parse_all($str,$arr);

$result = array();
foreach ($arr['apps'] as $id => $app) {
    if (trim($app['name']) == $app_name) {
        $result['id'] = $id;  
	break;
    }
}
print json_encode($result);
