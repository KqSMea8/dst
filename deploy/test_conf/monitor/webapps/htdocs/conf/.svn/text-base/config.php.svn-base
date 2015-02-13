<?php 
$create_pe_history_path = "./logpull/.cache.history";
$hdfs_web_url_prefix="http://cq01-testing-dcqa-b7.cq01.baidu.com:8408/browseDirectory.jsp?dir=";
$cfg_pmdir="/";
$cfg_pndir="/";
$cfg_pm_file_server_port=8100
$cfg_pn_file_server_port=8100
$no_db = false;
$db_host="127.0.0.1:3306";
$db_user="root";
$db_password="123456";
$db_name="dstream";

$hdfs_display_url="hfs_web_url"
$zookeeper_display_url="zgx_roshan"
$bigpipe_display_url="#";


$GLOBALS['tpl_dir']=dirname(__FILE__)."/../tpl/";



// this should be replaced if the path format changed..so put it in the config...
// function example:
// get_hdfs_dir_from_url("hdfs://yx-testing-platqa1003.yx01:38002/DStream_acmol/test/3/log")  get return value :"/DStream_acmol/test/3/log"
function get_hdfs_dir_from_url($url){
  if(sscanf($url,"hdfs://%*[^/]%s",$dir)<1){
    return "";
  }
  return $dir;
}

?>
