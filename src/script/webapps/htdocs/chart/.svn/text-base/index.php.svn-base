<?php
////////////////// Edit Area Begin//////////////////////
require_once('../conf/config.php');

$path=$app_path = $_GET['path']?$_GET['path']:"/DStream";
$url_params = "appid=${_GET['appid']}&stime=${_GET['stime']}&etime=${_GET['etime']}&step=${_GET['step']}";
$TPL['menu']=array();
$TPL['menu']['QPS'] = "/chart/index.php?$url_params";
$TPL['menu']['Latency'] = "/chart/latency.php?$url_params";
$TPL['active_menu'] = 'QPS';

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

$tpl_name='qps';
$tpl_dir=$GLOBALS['tpl_dir'];
include("$tpl_dir/header.tpl.php");
?>
<div><p><p>
<form action="<?php echo $_SERVER['PHP_SELF'];?>">
AppID:<input name="appid" value="<?php echo empty($_GET['appid'])?'':trim($_GET['appid']);?>" />
Start-time:<input name="stime" value="<?php echo empty($_GET['stime'])?'':trim($_GET['stime']);?>"/>
Stop-time:<input name="etime"  value="<?php echo empty($_GET['etime'])?'':trim($_GET['etime']);?>"/>
Step:<input name="step"  value="<?php echo empty($_GET['step'])?'':trim($_GET['step']);?>"/></br>
PEID:<textarea name="peid" style="margin: 0px 0px 9px; width: 783px; height: 36px;">
<?php echo empty($_GET['peid'])?'':trim($_GET['peid']);?></textarea>
<input type="submit" value="Let's Go!" />
</form>
</div>

<!-- jQuery and Highchart JS -->
<script type="text/javascript" src="http://libs.baidu.com/jquery/1.7.0/jquery.min.js"></script>
<script src="http://libs.baidu.com/highcharts/2.2.5/highcharts.js" type="text/javascript"></script>

<?php
require_once('function.php');
$start_time = date("Y-m-d H:i:s", strtotime('-20 minutes'));
if (isset($_GET['stime']) && !empty($_GET['stime'])) {
	$start_time = trim($_GET['stime']);
}

$end_time = date("Y-m-d H:i:s", strtotime('-1 minutes'));
if (isset($_GET['etime']) && !empty($_GET['etime'])) {
	$end_time = trim($_GET['etime']);
}

$step = 10;
if (isset($_GET['step']) && !empty($_GET['step'])) {
	$step = trim($_GET['step']);
	$step = intval($step) > 0 ? intval($step) : 10;
}

$peid = 'all';
if (isset($_GET['peid']) && !empty($_GET['peid'])) {
	$peid = trim($_GET['peid']);
}

$appid = 1;
if (isset($_GET['appid']) && !empty($_GET['appid'])) {
	$appid = trim($_GET['appid']);
	$appid = intval($appid) > 0 ? intval($appid) : 1;
}

mysql_connect($db_host,$db_user,$db_password) or die(mysql_error());
mysql_select_db($db_name) or die(mysql_error());
$sql_cmd = "select c_time, c_attr1, c_attr3, c_value from counter where "; 
$sql_cmd .= "c_time>='$start_time' and c_time<='$end_time' and (c_time%$step)=0 and c_name='pe.RecvTuples' ";
if (isset($_GET['appid'])) {
	$sql_cmd .= "and c_attr2=$appid ";
}

if ($peid != 'all') {
	$sql_cmd .= "and (";
	$peid_list = explode(',', $peid);
	foreach ($peid_list as $pe) {
		$sql_cmd .= "c_attr1=$pe or ";
	}
	$sql_cmd .= '1=0) ';
}

$sql_cmd .= "order by c_time, c_attr3";
$time_array = array();
$pe_qps_array = array();

$result = mysql_query($sql_cmd) or die(mysql_error());
while ($row = mysql_fetch_assoc($result)) {
	if (!in_array($row['c_time'], $time_array)) {
		$time_array[] = $row['c_time'];
	}
	$processor = intval($row['c_attr3']) & 0xFF;
	if (!array_key_exists($processor, $pe_qps_array)) {
		$pe_qps_array[$processor] = array();
	} else {
		if (!array_key_exists($row['c_attr1'], $pe_qps_array[$processor])) {
			$pe_qps_array[$processor][$row['c_attr1']] = array();
		} else {
			$pe_qps_array[$processor][$row['c_attr1']][] = intval($row['c_value']);
		}
	}
}

$max_xaxis_num = 25;
$time_array_num = count($time_array);
$x_per = $time_array_num / $max_xaxis_num;
if ($time_array_num > $max_xaxis_num) {
	for ($i = 0; $i < $time_array_num; $i++) {
		if ($i % $x_per) {
			$time_array[$i] = '';
		}
	}
}

// title
echo "<h2>start time: $start_time.</h2>";
// gen chart
$container = array();
$container[0] = 'importer';
$container[1] = 'exporter';
$container[2] = 'task';
$container[3] = 'merger_ubs_union';
$container[4] = 'merger_cpro';
$container[5] = 'merger_ubs_ps_yd';
$container[6] = 'merger_tcfront_yd';
$container[7] = 'merger_ubs_union_yd';
foreach($pe_qps_array as $processor => $processor_qps) {
	gen_chart($container[$processor], $time_array, $processor_qps);
}
////////////////// Edit Area End  //////////////////////
include("$tpl_dir/footer.tpl.php");
