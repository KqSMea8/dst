<h1><a href="index.php?path=<?php echo $app_path;?>">Dstream Job Display</a></h1>
<h2>ZooKeeper Addr : <?php system("cd bin;sh get_zk_addr.sh");?></h2>
<h3>PM infomation</h3>
<table class="table table-striped">
  <thead>
    <tr>
      <th class="header headerSortDown">Path</th>
      <th class="header">host</th>
      <th class="header">submit port</th>
      <th class="header">report port</th>
      <th class="header">httpd port</th>
    </tr>
  </thead>
    <tr>
      <td><?php echo htmlspecialchars($app_path.'/PM');?></td>
      <td><?php echo $arr['pm']['host']?(gethostbyaddr($arr['pm']['host'])."<a href=\"ls.php?type=PM&path=$app_path&dir=/\">show files</a>"):"NO RUNNING PM";?></td>
      <td><?php echo $arr['pm']['submit_port'];?></td> 
      <td><?php echo $arr['pm']['report_port'];?></td>
      <td><?php echo $arr['pm']['httpd_port'];?></td>
    </tr>
    <?php if ($arr['pmbackup']) foreach($arr['pmbackup'] as $backpm){?>
    <tr>
        <td><?php echo htmlspecialchars($app_path.'/BackupPM');?></td>
      <td><?php echo $backpm['host']?(gethostbyaddr($backpm['host'])):"NO RUNNING PM";?></td>
      <td><?php echo empty($backpm['submit_port']) ? '0' : $backpm['submit_port'];?></td> 
      <td><?php echo empty($backpm['report_port']) ? '0' : $backpm['report_port'];?></td> 
      <td><?php echo empty($backpm['httpd_port']) ? '0' : $backpm['httpd_port'];?></td> 
	
    </tr>
    <?php }?></table>
<h3>Applications [total:<?php echo count($arr['app']);?>]</h3>
<br/>
<table class="table table-striped">
  <thead>
    <tr>
      <th class="header headerSortDown">AppID</th>
      <th class="header">AppName</th>
      <th class="header">User</th>
      <th class="header">SubmitTime</th>
      <th class="header">Status</th>
      <th class="header" style="width:60px">PE number</th>
    </tr>
  </thead>
  <tbody>
<?php
if($arr['app']) foreach($arr['app'] as $app){
?>
<?php
    $pe_count = 0;
    foreach($app['topology']['processors'] as $processor) {
        $pe_count += $processor['parallism'];
    }
?>
<tr><td><?php echo $app['id']['id'];?></td><td><a href="processorlist.php?appid=<?php echo $app['id']['id'];?><?php if($app_path) echo "&path=$app_path";?>"><?php echo $app['name'];?></a></td><td><?php echo $app['user']['username'];?></td><td><?php echo date("Y-m-d H:i:s",$app['submit_time'] / 1000);?></td><td><?php echo $app_stat[$app['status']];?></td><td><?php echo $pe_count;?></td>
</tr

<?php
}
?>
  </tbody>
</table>
<h3>PN list [total:<?php echo count($arr['pn']);?>]</h3>
<?php $pns = $arr['pnmachines']['pn_list'];
$failpn=array();
if(!empty($pns)) {
    foreach($pns as $pn){
        $pnhost = gethostbyname($pn);
        $failpn[$pnhost] = true;
    }
}
?>
<table class="table table-striped">
<thead>
<th class="header">debugger</th>
<th class="header headerSortDown">PNID</th>
<th class="header">host</th>
<th class="header">pn_pm port</th>
<th class="header">pn_pe port</th>
<th class="header">pn_pn port</th>
<th class="header">debug port</th>
<th class="header">httpd port</th>
<th class="header">pe count</th>
</thead>
<tbody>
<?php 
if(!empty($arr['pn'])) {
foreach($arr['pn'] as $pn){ 
$pnhostname = gethostbyaddr($pn['host']);
$failpn[$pn['host']] = false;
$pnid = $pn['pn_id']['id'];
?>
<tr id="<?php echo $pnid;?>" class="pn_list">
<td><a href="pn_debugger.php?host=<?php echo $pn['host'];?>&port=<?php echo $pn['httpd_port'];?>&path=<?php echo $app_path;?>">debug</a></td>
<td><?php echo $pnid;?> <a href="<?php echo "ls.php?path=$app_path&id=$pnid&type=PN&dir=/";?>">files</a></td>
<td><?php echo $pnhostname; ?></td>
<td><?php echo $pn['rpc_server_port'];?></td>
<td><?php echo $pn['pub_port'];?></td>
<td><?php echo $pn['sub_port'];?></td>
<td><?php echo $pn['debug_port'];?></td>
<td><?php echo $pn['httpd_port'];?></td>
<td class="load-cnt" id="<?php  echo $pn['host'].":".$pn['debug_port'];?>"><i class="icon-refresh"/></td>
</tr>
<?php }
}?>
</tbody>
</table>
<pre>
Fail PNs :
<?php if ($failpn) foreach($failpn as $host=>$fail) {
if($fail) echo "$host(".gethostbyaddr($host).")<br/>";
} ?>
</pre>
<h4><a href="get_zk_info.php">Click to Show Infos in ZK</a></h4>
<div id="status_panel"></div>
<script>
$(document).ready(function() {
	$("table").each(function(i) { $(this).tablesorter();});
	$(".load-cnt").each(function(){
		$(this).load("ajax/count_pe.php?addr="+$(this).attr("id"));
	})	
});
</script>
