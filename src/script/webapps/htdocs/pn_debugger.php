<?php
$no_db = true;
include ("include/common.php");

$path=$app_path = $_GET['path']?$_GET['path']:"/DStream";
$TPL['menu']=array();
$TPL['menu_title']="Job Display";
$TPL['menu_title_link']="index.php?path=$path";

$TPL['title']="Job Display";
$TPL['right_menu']['Job Display']='index.php';
$TPL['right_menu']['Log Metrics Display']='log_metrics.php';
$TPL['active_right_menu']='Job Display';

$TPL['menu_search_form']=array();
$TPL['menu_search_form']['action']="";
$TPL['menu_search_form']['method']="GET";
$TPL['menu_search_form']['search_name']="path";
$TPL['menu_search_form']['default_text']="$path";
$TPL['tpl_name']='pn_debugger';
////////////////////
$pnid = isset($_GET['host']) ? $_GET['host'] : Null;
$port = isset($_GET['port']) ? $_GET['port'] : Null;
if (!$pnid || !$port) {
    die('pnid or port should not empty!!!');
}

$transporter = file_get_contents("http://$pnid:$port/transporter") or die('WRONG PN HOST OR PORT!!!');

$transo = json_decode($transporter, true);
$counter = array();
foreach ($transo as $flow) {
	foreach($flow as $peid => $item) {
		if (!is_array($item)) {
			continue;
		}
		//var_dump("==========================================");
		//var_dump($peid);
		//var_dump($item);
		//continue;
		if (array_key_exists('Inflow', $item)) {
			$queue = $item['Inflow']['Queue'];
			$counter[$peid]['RecvQueue']['Size'] = $queue['cnt_data_size'];
			$counter[$peid]['RecvQueue']['Load'] = 100.0 * $queue['cnt_data_size'] / $queue['cnt_capability'];
		}
		if (array_key_exists('Branch', $item['AllOutflows']['Outflow'])) {
			$branchs = $item['AllOutflows']['Outflow']['Branch'];
			$counter[$peid]['SendQueue']['SizeSum'] = 0;
			$counter[$peid]['SendQueue']['LoadSum'] = 0;
			$counter[$peid]['SendQueue']['Branch'] = 0;
			$queue_min = 2<<30; $queue_max = -1; // 2GB
			$queue_min_down_pe = Null; $queue_max_down_pe = Null;
			foreach ($branchs as $down_pe => $brc) {
				$queue = $brc['Queue'];
				if ($queue['cnt_data_size'] > $queue_max) {
					$queue_max = $queue['cnt_data_size'];
					$queue_max_down_pe = $down_pe;
				}	
				if ($queue['cnt_data_size'] < $queue_min) {
					$queue_min = $queue['cnt_data_size'];
					$queue_min_down_pe = $down_pe;
				}	
				$counter[$peid]['SendQueue']['SizeSum'] += $queue['cnt_data_size'];
				$counter[$peid]['SendQueue']['LoadSum'] += $queue['cnt_data_size']/$queue['cnt_capability'];
				$counter[$peid]['SendQueue']['Branch'] += 1;
			}
			$counter[$peid]['SendQueue']['SizeAvg'] = $counter[$peid]['SendQueue']['SizeSum']/$counter[$peid]['SendQueue']['Branch'];
			$counter[$peid]['SendQueue']['LoadAvg'] = $counter[$peid]['SendQueue']['LoadSum']/$counter[$peid]['SendQueue']['Branch'];
			$counter[$peid]['SendQueue']['SizeMax'] = $queue_max;
			$counter[$peid]['SendQueue']['SizeMin'] = $queue_min;
			$counter[$peid]['SendQueue']['SizeMaxDownPE'] = $queue_max_down_pe;
			$counter[$peid]['SendQueue']['SizeMinDownPE'] = $queue_min_down_pe;
		}
	}
}
//var_dump($counter);


////////////////////

$tpl_name=$TPL['tpl_name'];
$tpl_dir=$GLOBALS['tpl_dir'];
include("$tpl_dir/header.tpl.php");
include("$tpl_dir/$tpl_name.tpl.php");
include("$tpl_dir/footer.tpl.php");
