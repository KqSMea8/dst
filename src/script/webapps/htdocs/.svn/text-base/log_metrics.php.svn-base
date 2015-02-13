<?php
/***************************************************************************
 * 
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * $Id$ 
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
$TPL['menu']=array();
$TPL['menu_title']="LogMetricsDisplayer";
$TPL['menu_title_link']="?";
$TPL['menu']['view']='?do=view';
$TPL['menu']['search_counter']='?do=search_counter';
$TPL['title']="Log Metrics Displayer";
$TPL['tpl_name']='log_metrics';
$TPL['right_menu']['Job Display']='index.php';
$TPL['right_menu']['Log Metrics Display']='?';
$TPL['active_right_menu']='Log Metrics Display';

// set default do action to 'view'
if (empty($_GET['do'])) {
    $_GET['do'] = 'view';
}
	
if ($_GET['do']=='search_log') {
  // do action = 'search_log'
  $TPL['active_menu']='search_log';
  $suffix="";
  if ($_GET['name']) {
	  $suffix=$suffix." and name like '$_GET[name]'";
  }
  if ($_GET['log_priority']) {
    $suffix=$suffix." and log_priority = '$_GET[log_priority]'";
  }
  if ($_GET['log_time_begin']) {
    $suffix=$suffix." and log_time >= '$_GET[log_time_begin]'";
  }
  if ($_GET['log_time_end']) {
    $suffix=$suffix." and log_time <= '$_GET[log_time_end]'";
  }
  if ($_GET['log_position']) {
    $suffix=$suffix." and log_position like '%$_GET[log_position]%'";
  }
  if ($_GET['log_message']){
    $suffix=$suffix." and log_message like '%$_GET[log_message]%'";
  }
  if ($_GET['log_sender']){
    $suffix=$suffix." and log_sender like '$_GET[log_sender]'";
  }
  if ($_GET['sender_ip']){
	  $suffix=$suffix." and sender_ip='$_GET[sender_ip]'";
  }
  if ($_GET['data0']){
	  $suffix=$suffix." and data0='$_GET[data0]'";
  }
  if ($_GET['data1']){
	  $suffix=$suffix." and data1='$_GET[data1]'";
  }
  if ($_GET['data2']){
	  $suffix=$suffix." and data2='$_GET[data2]'";
  }
  if ($_GET['data3']){
	  $suffix=$suffix." and data3='$_GET[data3]'";
  }
  if ($_GET['data4']){
	  $suffix=$suffix." and data4='$_GET[data4]'";
  }
  $orderby=$_GET['orderby']?$_GET['orderby']:"id DESC";
  $columns=$_GET['columns']?$_GET['columns']:"*";
  $page=intval($_GET['page']);
  if($page==0) {
	  $page=1;
  }
  $perpage=intval($_GET['perpage']);
  if ($perpage<1 || $perpage>10000) {
    $perpage=100;
  }
  $result = $mysqli->query("select count(*) from dstream_log where 1=1 ".$suffix);
  $total_num = $result->fetch_row();
  $total_num = $total_num[0];

  $total_page=ceil($total_num/$perpage);
  $showpage=11;
  $prefix_of_pager="?do=search_log&columns=$_GET[columns]&orderby=$_GET[orderby]&perpage=$perpage&name=$_GET[name]&log_sender=$_GET[log_sender]&log_time_begin=$_GET[log_time_begin]&log_time_end=$_GET[log_time_end]&log_message=$_GET[log_message]&log_priority=$_GET[log_priority]&data0=$_GET[data0]&data1=$_GET[data1]&data2=$_GET[data2]&data3=$_GET[data3]&data4=$_GET[data4]&page=";

  $limit = " limit ".(($page-1)*$perpage).",".$perpage;
  $sqlcmd="select $columns from dstream_log where 1=1 ".$suffix."order by ".$orderby;
  $result = $mysqli->query($sqlcmd.$limit) or die($mysqli->error);

} else if ($_GET['do'] == 'view') {
  // do action = 'view'
  $TPL['active_menu']='view';
  $view = $_GET['view'];
  $page=intval($_GET['page']);
  if($page<1)
    $page=1;
  $perpage=intval($_GET['perpage']);
  if($perpage<1 || $perpage>10000) {
	  $perpage=100;
  } 
  $limit = $_GET['limit']?" limit $_GET[limit]":" limit ".(($page-1)*$perpage).",".$perpage;
  if(empty($view)) {
	  $show_add=true;
	  $sqlcmd="SELECT table_name,table_type FROM information_schema.tables WHERE table_schema =  '$db_name'";
  }else{
	  $sqlcmd="select * from $view ";
	  $result=$mysqli->query("show create table $view") or die($mysqli->error);
	  $row=$result->fetch_row();
	  $view_create=$row[1];
  }
  $user_where=$_GET['where']?"where ".stripslashes($_GET[where]):"";
  if ($_GET['show']!='txt') {
    $result = $mysqli->query("select count(*) from ($sqlcmd $user_where".($_GET['limit']?" limit $_GET[limit]":"").") as res");
    $total_num = $result->fetch_row();
    $total_num = $total_num[0];
    $showpage=11;

    $total_page=ceil($total_num/$perpage);
    $prefix_of_pager="?view=$view&perpage=$perpage&page=";
  }
  $result=$mysqli->query("$sqlcmd $user_where $limit") or die("SQL:$sql $user_where $limit:".$mysqli->error);
	if ($_GET['show']=='txt') {
    // plat print
		$delim="";
		while($field=$result->fetch_field()) {  
			echo $delim.$field->name;
			$delim="|";
		}
		echo "\n";
		while($row=$result->fetch_row()){
			$delim="";
			foreach($row as $item){
				echo $delim.$item;
				$delim="|";
			}
			echo "\n";
		}
		exit(0);
	} else if ($_GET['show']=='json') {
    // json print
    $data_items=array();
		while($row=$result->fetch_assoc()) {
      $data_items[]=$row;
		}
    print json_encode($data_items);
		exit(0);
  }
} else if($_GET['do'] == 'search_counter') {
  // counter log page
  $TPL['active_menu']='search_counter';

  $suffix="";
  if ($_GET['machine_name']){
    $suffix=$suffix." and machine_name = '" . $_GET['machine_name']. "'";
  }
  if($_GET['c_time_begin']){
    $suffix=$suffix." and c_time >= '".$_GET['c_time_begin']."'";
  }
  if($_GET['c_time_end']){
    $suffix=$suffix." and c_time <= '" .$_GET['c_time_end']."'";
  }
  if($_GET['c_name']){
    $suffix=$suffix." and c_name = '" .$_GET['c_name']. "'";
  }
  if($_GET['c_attr1']){
    $suffix=$suffix." and c_attr1=" .$_GET['c_attr1'];
  }
  if($_GET['c_attr2']){
    $suffix=$suffix." and c_attr2 = ". $_GET['c_attr2'];
  }
  if($_GET['c_attr3']){
    $suffix=$suffix." and c_attr3 = ". $_GET['c_attr3'];
  }
  if($_GET['c_type']){
    $suffix=$suffix." and c_type = '". $_GET['c_type']. "'";
  }
  if($_GET['cluster_name']){
    $suffix=$suffix." and cluster_name = '".$_GET['cluster_name']."'";
  }
  $orderby=$_GET['orderby']?$_GET['orderby']:"c_time DESC";
  $columns=$_GET['columns']?$_GET['columns']:"*";
  $page=intval($_GET['page']);
  if($page==0) {
    $page=1;
  }
  $perpage=intval($_GET['perpage']);
  if($perpage<1 || $perpage>10000){
    $perpage=100;
  }

  // show status run faster than select count(*) in innodb engine
  $result = $mysqli->query("show table status like 'counter'");
  $total_num = $result->fetch_assoc();
  $total_num = intval($total_num['Rows']);

  $total_page=ceil($total_num/$perpage);
  $showpage=11;
  $prefix_of_pager="?do=search_counter&orderby=" .$_GET['orderby'];
  $prefix_of_pager .= "&perpage=".$perpage;
  $prefix_of_pager .= "&machine_name=" .$_GET['machine_name'];
  $prefix_of_pager .= "&cluster_name=" .$_GET['cluster_name'];
  $prefix_of_pager .= "&c_time_begin=" .$_GET['c_time_begin'];
  $prefix_of_pager .= "&c_time_end=" .$_GET['c_time_end'];
  $prefix_of_pager .= "&c_name=" .$_GET['c_name'];
  $prefix_of_pager .= "&c_attr1=" .$_GET['c_attr1'];
  $prefix_of_pager .= "&c_attr2=" .$_GET['c_attr2'];
  $prefix_of_pager .= "&c_attr3=" .$_GET['c_attr3'];
  $prefix_of_pager .= "&c_type=" .$_GET['c_type'];
  $prefix_of_pager .= "&page=";

  $limit = " limit ".(($page-1)*$perpage).",".$perpage;
  $sqlcmd="select $columns from `counter` where 1=1 ".$suffix." order by ".$orderby;
  $result = $mysqli->query($sqlcmd.$limit) or die($mysqli->error);
}

////////////////// Edit Area End  //////////////////////
$tpl_name=$TPL['tpl_name'];
$tpl_dir=$GLOBALS['tpl_dir'];
include("$tpl_dir/header.tpl.php");
include("$tpl_dir/$tpl_name.tpl.php");
include("$tpl_dir/footer.tpl.php");

?>
