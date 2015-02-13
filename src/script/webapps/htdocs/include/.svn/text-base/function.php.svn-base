<?php
/***************************************************************************
 * 
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file function.php
 * @author Yuncong Zhang(zhangyuncong@baidu.com)
 * @date 2012/04/09 11:18:55
 * @version 1.0 
 * @brief 
 *  
 **/

function show_msg($msg,$url="",$second=3){
	die($msg);
}
function json_exit($msg,$success=true){
	echo '{"success":'.($success?"true":"false").',"message":'."\"$msg\"}";
	exit(0);
}


// show pager
function showpages($page,$total_page,$showpage,$prefix_of_pager="?page="){
       $start_page=$page-intval($showpage/2);
       $start_page=$start_page <= 0? 1 : $start_page;
    $end_page=min($start_page+$showpage-1,$total_page);
?>
<div class="btn-group">
<a class="btn" href="<?php echo $prefix_of_pager."1";?>">&lt;&lt;</a>
<?php for($i=$start_page;$i<=$end_page;$i++){?>
<a class="btn <?php if ($page==$i) echo "primary";?>" href="<?php echo $prefix_of_pager.$i;?>"><?php echo $i;?></a>
<?php }?>
<a class="btn" href="<?php echo $prefix_of_pager.$total_page;?>">&gt;&gt;</a>
</div>
<?php
}	
function url_exists($url) { 
    $hdrs = @get_headers($url); 
    return is_array($hdrs) ? preg_match('/^HTTP\\/\\d+\\.\\d+\\s+2\\d\\d\\s+.*$/',$hdrs[0]) : false; 
} 

function get_pn_install_list($zk_root){
	$arr = get_zk_info("$zk_root/machines/pn_machines");
	return $arr["$zk_root/machines/pn_machines"]['pn_list'];
}
function assocs2table($assoc_arr){
	$ret = "";
	if ($assoc_arr){
		$cols = array();
		$ret = '<table class="table table-striped"><thead><tr>';
		if($assoc_arr[0]) {
			foreach($assoc_arr[0] as $colname=>$val){
				$ret .= "<th class=\"header\">$colname</th>";
				$cols[]=$colname;
			}
		}
		$ret .= "</tr></thead><tbody>"; 
		foreach($assoc_arr as $assoc){
			$ret .= "<tr>";
			foreach($cols as $colname){
				$ret .= "<td>".$assoc[$colname]."</td>";
			}
			$ret .= "</tr>";
		}
	}
	return $ret;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
?>
