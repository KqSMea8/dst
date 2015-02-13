<?php
define("LEVEL", 0);
define("ERROR_LEVEL",5);
define ("LOGFILEPATH","log/dstream_php.log");
date_default_timezone_set('Asia/Shanghai');
$LogFile=fopen(LOGFILEPATH,"aw+");
if(!$LogFile)
{
	show_msg("open ".LOGFILEPATH." failed");
}
define ("LOGFILE",$LogFile);
function WriteLog($message,$level = 1){
	if($level>LEVEL){
		fprintf(LOGFILE,"[%s]%s\n",date("Y-m-d H:i:s"),$message);
		if($level>=ERROR_LEVEL){
			die("error:".$message."\n");
		}
	}
}
//before this , they are log function and defines
//============================================================//

/*
all=>apps + pms + pns + clients
apps => apps_pre + apps_without_pre
apps_without_pre => app + apps_without_pre
app => id + name + user + status + PEList
*/
function parse_all(&$str,&$arr){
	$arr=array();
	if($str=="") return ;
	$arr['path']=array();
	parse_apps($str,$arr['apps'],$arr['path']['app']);
	parse_pm($str,$arr['pm'],$arr['path']['pm'] );
	parse_back_pm($str,$arr['backpm'],$arr['path']['backpm']);
	parse_pns($str,$arr['pns'],$arr['path']['pn'] );
	parse_clients($str,$arr['clients'],$arr['path']['client'] );
	WriteLog("parse sucessful",2);
	//print_r($arr);
}
function parse_apps(&$str,&$apps,&$path){
	sscanf($str,"%s%n",$path,$start);
	$str=substr($str,$start);
	parse_apps_without_pre($str,$apps);
}
function parse_apps_without_pre(&$str,&$apps){
	if(parse_app($str,$apps)){
		parse_apps_without_pre($str,$apps);
	}
}

function parse_app(&$str,&$apps){
	//WriteLog($str,2);
	sscanf($str,"%s",$tmp);
	if($tmp[0]=='/') return false;
	if(sscanf($str,"%*[^-]---%d%n",$id,$start)<=1) {
		return false;
	}
	WriteLog("start parse app{id:[$id]}",2);
	//WriteLog($str,2);
	$str=substr($str,$start);
	sscanf($str,"%*[^:]:%[^\n]%n",$apps[$id]['name'],$start);
	$str=substr($str,$start);
	sscanf($str,"%*[^:]:%[^\n]%n",$apps[$id]['user'],$start);
	$str=substr($str,$start);
	sscanf($str,"%*[^:]:%[^\n]%n",$apps[$id]['status'],$start);
	$str=substr($str,$start);
    sscanf($str,"%*[^:]:%[^\n]%n",$apps[$id]['hdfs_log_path'],$start);
    $str=substr($str,$start);
	sscanf($str,"%*[^:]:%[^\n]%n",$apps[$id]['submit_time'],$start);
    $str=substr($str,$start);
	parse_pe_list($str,$apps[$id]['pelist'],$id,$apps[$id]['name']);
	return true;
}
function parse_pe_list(&$str,&$pelist,$id,$name){
	if(sscanf($str,"%*[^-]---PE List%n",$start)<=1){
		echo $str;
		WriteLog("when parse app{id:[$id],name:[$name]},excepted \"---PE List\",but not found ",ERROR_LEVEL);
	};
	$str=substr($str,$start);
	parse_pe_list_without_pre($str,$pelist);
}
function parse_pe_list_without_pre(&$str,&$pelist){
	if(parse_pe($str,$pelist)){
		parse_pe_list_without_pre($str,$pelist);
	}
}
function parse_processor(&$str,&$processor){
	if(sscanf($str,"%*[^-]---Processor: ID:[%[0-9]],Name:[%[^]]],Role:[%[^]]],cpu_limit:[%f],memory_limit:[%d],network_limit:[%d],disk_limit:[%d]%n",$processor['id'],$processor['name'],$processor['role'],$processor['cpu_limit'],$processor['memory_limit'],$processor['network_limit'],$processor['disk_limit'],$start)<=3){
		return false;
	}
	$str = substr($str, $start);
	return true;
}
function parse_app_processor_without_head(&$str) {
        sscanf($str, "App ID : %[0-9]\n%n", $app_id, $start);
	$str = substr($str, $start);
}
function parse_app_processor(&$str,&$processor){
	if(sscanf($str,"  Processor ID : %[0-9]\n  ProcessorName : %s\n  ProcessorRole : %s\n     avg SEND QPS : %f\n     SEND QPS ALL : %[0-9]\n     RECV QPS ALL : %[0-9]\n     RECV ALL : %[0-9]\n     SEND ALL : %[0-9]\n     PE NUM : %[0-9]\n     avg RECV QPS : %f\n     avg MEM : %f\n     avg CPU : %f\n%n", $processor['processor id'], $processor['processor name'], $processor['processor role'], $processor['avg send QPS'], $processor['send QPS'], $processor['recv QPS'],$processor['recv counter'], $processor['send counter'], $processor['pe num'], $processor['avg recv QPS'], $processor['avg mem'], $processor['avg cpu'], $start)<=9){
	    return false;
	}
	$str = substr($str, $start);
	return true;
}
function parse_pe(&$str,&$pelist){
	$pe=array();
	if(sscanf($str,"%*[^-]---AppID:%d, ProcessorID:%d, PEID:%[0-9], Serial:%d%n",$pe['appid'],$pe['processorid'],$pe['peid'],$pe['serial'],$start)<=1){
		return false;
	}
	WriteLog("start parse pe, PEID:[$pe[peid]],AppID:$pe[appid]",2);
	$pelist[$pe["peid"]]=$pe;
	$id = $pe["peid"];
	$str = substr($str,$start);
	parse_processor($str,$pelist[$id]['processor']);
	parse_backups($str,$pelist[$id]['backups']);
	if(sscanf($str,"%*[^-]---primary id:[%[0-9]]%n",$pelist[$id]['primaryid'],$start)<=1){
		echo "read primaryid error:{ [$str] }";
		WriteLog("read primaryid error",ERROR_LEVEL);
	}
	$str = substr($str,$start);
	return true;
}
function parse_backups(&$str,&$backups){
	if(sscanf($str,"%*[^-]---backups[%d]:%n",$id,$start)<=1){
		return false;
	}
	$str = substr($str,$start);
	sscanf($str,"%*[^:]:[%[0-9]], pn_id:[%[0-9._]],last_assignment_time:[%[0-9._]]%n",$backups[$id]['backup_pe_id'],$backups[$id]['pn_id'],$backups[$id]['last_assign_time'],$start);
	$str = substr($str,$start);
	parse_backups($str,$backups);
}
function parse_back_pm(&$str,&$backpms,&$backpm_path){
	sscanf($str,"\n%[^\n]%n",$path,$start);
	if (substr(trim($path),-8,8)!="BackupPM") return false;
	$str = substr($str,$start);
	$backpm_path=$path;
	while(parse_pm_without_pre($str,$backpm)) $backpms[]=$backpm;
}
function parse_pm(&$str, &$pm,&$pm_path){
	sscanf($str,"\n%[^\n]%n",$pm_path,$start);
	$str = substr($str,$start);
	parse_pm_without_pre($str,$pm);
}
function parse_pm_without_pre(&$str,&$pm){
	sscanf($str,"%s",$tmp);
	if($tmp[0]=='/'){
		return false;
	}
	if(sscanf($str,"%*[^-]---PM: host:[%[^]]] submit_port:[%d] report_port:[%d]%n",$pm['host'],$pm['submit_port'],$pm['report_port'],$start)<=1){
	
	WriteLog("read PM failed");
	}
	$str = substr($str,$start);
	return true;

}

function parse_pns(&$str, &$pns,&$pn_path){
	sscanf($str,"\n%[^\n]%n",$pn_path,$start);
	$str = substr($str,$start);
	if (substr($pn_path,-3,3)=="/PN")
		parse_pns_without_pre($str,$pns);
	else parse_pns($str,$pns,$pn_path);
}
function parse_pns_without_pre(&$str,&$pns){
	while(parse_pn($str,$pns));
}
function parse_pn(&$str,&$pns){
	sscanf($str,"%s",$tmp);
	if($tmp[0]=='/'){
		return false;
	}
	sscanf($str,"%*[^-]---%[^\n]%n",$pnid,$start);
	$pns[$pnid]=array();
	$str = substr($str,$start);
	$succ = sscanf($str,"%*[^-]---host:[%[0-9.]], pn_pm_port:[%d], pn_pe_port:[%d], pn_pn_port:[%d], debug_port:[%d], status:[%[^]]]%n",$pns[$pnid]['host'],$pns[$pnid]['pn_pm_port'],$pns[$pnid]['pn_pe_port'],$pns[$pnid]['pn_pn_port'],$pns[$pnid]['debug_port'],$pns[$pnid]['status'],$start);
	if($succ <= 1) return false;
	$str = substr($str,$start);
	return true;
}
function parse_clients(&$str,&$start){

}

function array2ul($array)
{	
	$out="<ul>";
	foreach($array as $key => $elem){
		if(!is_array($elem)){
				$out=$out."<li><span>$key:[$elem]</span></li>";
		}
		else $out=$out."<li><span>$key</span>".array2ul($elem)."</li>";
	}
	$out=$out."</ul>";
	return $out;
}
function strip_quote($str){
	if(strlen($str)>=2 && $str[0]==$str[strlen($str)-1] && ($str[0]=='"' || $str[0]=='\'')) return substr($str,1,-1);
	return $str;	
}
function parse_protobuf_elems(&$str,&$elem_arr){
	while(parse_protobuf_elem($str,$elem_arr));
}
function parse_protobuf_elem(&$str,&$elem_arr){
	if(sscanf($str,"%*[ \n\t]%n",$start) == 2){
		$str = substr($str, $start);
	}
	if(sscanf($str,"%[a-zA-Z_]%n",$name,$start)<=0){
		if(sscanf($str,"%s%n",$right_bracket,$start)<0) return false;
		if($right_bracket != "}" ){
			return false;
			//WriteLog("execpted [}] ,but meet [$right_bracket]",ERROR_LEVEL);
		}
		return false;
	}
	$str = substr($str, $start);
	sscanf($str,"%s%n",$delim,$start);
	$str = substr($str, $start);
	switch($delim)
	{
	case ":": 
		sscanf($str,"%[^\n]%n",$value,$start);
		$elem_arr[$name][]=strip_quote(trim($value));
		$str = substr($str,$start);
		break;
	case "{":
		parse_protobuf_elems($str,$elem_arr[$name][]);
		sscanf($str,"%s%n",$right_bracket,$start);
		if($right_bracket != "}")
		{
			WriteLog("execpted right bracket [}], but meet string [$right_bracket].",ERROR_LEVEL);
		}
		$str = substr($str, $start);
		break;
	default:
		print_r($elem_arr);
	WriteLog("When parse [$name]:execpted  [:] or [{],but meet [$delim]",ERROR_LEVEL); 
	
	break;
	}
	return true;
}

//  function below should not have been in this file...but for now... you know..
function get_zk_data($app_path,$appid,&$arr){
	$str = shell_exec("cd bin;/bin/bash ./dumpshell.sh \"$app_path\" $appid ");
	parse_all($str,$arr); 
	if($arr['apps']) foreach($arr['apps'] as $app){
		if($app['pelist']) foreach($app['pelist'] as $pe)
			if($pe['backups']) foreach($pe['backups'] as $backpe) $arr['backpe'][$backpe['backup_pe_id']]["pn"]=$backpe["pn_id"];
	}
}
 
function get_path_dir($type,$id,$zk_data){
	global $cfg_pn_file_server_port;
	global $cfg_pm_file_server_port;
	global $cfg_pndir;
	global $cfg_pmdir;
	$ret = array();
	if($type=="PM"){
		$ret['host'] = $zk_data['pm']['host'];
		$ret['port'] = $cfg_pm_file_server_port;
		$ret['dir']  = $cfg_pmdir;
	}else if($type=="PN"){
		$ret['host'] = $zk_data['pns'][$id]['host'];
		$ret['port'] = $cfg_pn_file_server_port;
		$ret['dir']  = $cfg_pndir;
	}else if($type=="PE"){
		$ret['host'] = $zk_data["pns"][$zk_data['backpe'][$id]["pn_id"]];
		$ret["port"] = $cfg_pn_file_server_port;
		$ret["dir"]  = $cfg_pndir;
	}
	return $ret;
}
function comb_dir($rootdir,$dir){
	return $rootdir."/".str_replace("..","",$dir);
}

function get_pe_on_pn_by_pn_debugger($pn_url){
	$str = shell_exec("cd bin;sh ./pn_debugger.sh tcp://$pn_url pe_id_list");
	$pes = explode(" ",$str);
	$ret=array();
	foreach($pes as $peid){
		if (!is_numeric($peid)) continue;
		$ret[]=$peid;
	}
	return $ret;
}

function parse_zk_info(&$str,&$arr){
	if (sscanf($str,"%s\n%n", $line, $start) < 1 ){
		return false;
	}
	$str = substr($str, $start);
	sscanf($str, "[%[^]]]%n", $path,$start);
	$str = substr($str, $start);
	if (sscanf($str,"%s\n%n", $line, $start) < 1){
		return false;
	}
	parse_protobuf_elems($str,$arr[$path]);
	return true;
}
function parse_zk_infos(&$str,&$arr){
	while(parse_zk_info($str,$arr));
}
function get_zk_info($path,$max_depth = ""){
	$str = shell_exec("cd bin;/bin/bash ./get_zk_info.sh \"$path\" $max_depth");
	$arr = array();
	parse_zk_infos($str,$arr);
	return $arr;	
}
?>
