<?php
set_time_limit(10);
chdir("..");
$no_db=true;
include ("include/common.php");

function get_all_pn($app_path) 
{
    // get all pn & convert to ip format
    $pn_all = array();
    $pn_list = get_pn_install_list($app_path);
    foreach($pn_list as $pn) {
        $pn_all[] = gethostbyname($pn);
    }
    return $pn_all;
}

function get_alive_pn($app_path)
{
    // get alive pn & convert to ip format
    $pn_alive = array();
    $zk_pn = get_zk_info("$app_path/PN");
    foreach($zk_pn as $key => $val) {
        if(empty($val)) {
            continue;
        }
        $pn_alive[] = $val['host'][0];
    }
    return $pn_alive;
}

# zk root
$app_path = isset($_GET['path']) ? trim($_GET['path']): '';

# type = all: list all pn 
# type = alive: list only alive pn
# type = fail: list fail pn 
$type = isset($_GET['type']) ? trim($_GET['type']): 'all';
if (empty($app_path)) {
    echo "[]";
    exit;
}

if ($type == 'alive') {
    print json_encode(get_alive_pn($app_path));
} else if ($type == 'fail') {
    print json_encode(array_values(array_diff(get_all_pn($app_path), get_alive_pn($app_path))));
} else {
    print json_encode(get_all_pn($app_path));
}
exit;
