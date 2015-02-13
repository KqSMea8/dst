<h1><a href="index.php?path=<?php echo $app_path;?>">Dstream Job Display</a></h1>
<table class="table table-striped">
  <thead>
    <tr>
      <th >AppID</th>
      <th >AppName</th>
      <th >User</th>
      <th >Status</th>
    </tr>
  </thead>
  <tbody>
<?php
if($arr['apps']) foreach($arr['apps'] as $id => $app){
?>
<tr><td><?php echo $id;?></td><td><?php echo $app['name'];?></td><td><?php echo $app['user'];?></td><td><?php echo $app['status'];?></td>
</tr>

<?php
}
?>

  </tbody>
</table>
<table class="table table-striped">
  <thead>
    <tr>
      <th class="header headerSortDown" style="width:75px">Status</th>
      <th class="header" style="width:75px">IsPrimary</th>
      <th class="header">PEID</th>
      <th class="header">ProcessorID</th>
      <th class="header">ProcessorName</th>
      <th class="header">ProcessorRole</th>
      <th class="header">PrimaryID</th>
      <th class="header">SchedulerTime</th>
      <th class="header">PNID</th>
      <th class="header">Revision</th>
      <th class="header">receive tuples count</th>
      <th class="header">send tuples count</th>
    </tr>
  </thead>
  <tbody>
<?php if($cur_app['pelist']) 
        foreach($cur_app['pelist'] as $pe){
           if ($processorid >= 0 && $pe['processorid'] != $processorid) {
               continue;
           } 
           foreach($pe['backups'] as $backpe){
?>
  
    <tr> 
       <td><a href="pestatus.php?backuppeid=<?php echo $backpe['backup_pe_id'];?>&path=<?php echo $app_path;?>">click</a></td>
       <td><?php echo $backpe['backup_pe_id']==$pe['peid']?"Yes":"No";?></td>
       <td><?php echo $pe['peid'];?>  <span><a href="<?php echo $hdfs_web_url_prefix.(get_hdfs_dir_from_url($cur_app['hdfs_log_path'])."/PE/$pe[peid]")."&namenodeInfoPort=8402&ugi=3F91D4465C56066ED107EB0B775FB028";?>">logs</a><?php if($backpe['pn_id']){?> <a href="<?php echo "ls.php?path=$app_path&id=$backpe[pn_id]&type=PN&dir=/";?>">files</a> <?php }?> </span></td>
       <td><?php echo $pe['processorid'];?></td>
       <td><?php echo $pe['processor']['name'];?></td>
       <td><?php echo $pe['processor']['role'];?></td>
       <td><?php echo $pe['primaryid'];?></td>
       <td><?php echo date("Y-m-d H:i:s", $backpe['last_assign_time']/1000);?></td>
<td>

<?php
	if(empty($backpe['pn_id'])) 
	{
		echo "<a href=\"create_pe_history.php?peid=$backpe[backup_pe_id]\">";
		system("grep $backpe[backup_pe_id] ./logpull/.cache.history | tail -n 1 | awk '{print \"[\"$3\"]\",$2 }'");
		echo "</a><br />Warn: this info is not found in ZK..";
	}else{
		print ($arr['pns'][$backpe['pn_id']]['host'])?gethostbyaddr($arr['pns'][$backpe['pn_id']]['host']):"PN Not found in ZK";
		echo "<a href=\"create_pe_history.php?peid=$backpe[backup_pe_id]\">:$backpe[pn_id]</a>";
	}
?></td>   
<?php
$backuppeid = $backpe['backup_pe_id'];
?>
<td class="revision" id="<?php echo "revision_".$backuppeid;?>"><i class="icon-refresh" /></td>
<td class="recvShow" id="<?php echo "recv_".$backuppeid;?>"><i class="icon-refresh" /></td>
<td class="sendShow" id="<?php echo "send_".$backuppeid;?>"><i class="icon-refresh" /></td>
  </tr>

<?php } } ?>
     
  </tbody>
</table>

<script>$(document).ready(function() {
$("table").each(function(i) { $(this).tablesorter({ sortList: [[0,0]], headers: {}}); });

$.ajax({ type:"POST", 
         url:"ajax/pe_metrics.php", 
         data:{"peid":"<?php echo $peidlist;?>","path":"<?php echo $app_path;?>"},
         dataType: "json",
         success:function(metrics){
           $(".recvShow").text(function(){
             return (metrics[$(this).attr("id").substring(5)]||{"dstream.pe.recvTuplesCounter":""})["dstream.pe.recvTuplesCounter"];
             });
           $(".sendShow").text(function(){
             return (metrics[$(this).attr("id").substring(5)]||{"dstream.pe.sendTuplesCounter":""})["dstream.pe.sendTuplesCounter"];
             });
           $(".revision").text(function(){
               return (metrics[$(this).attr("id").substring(9)]||{"revision":""})["revision"];
           });
         } 
         } // end of ajax
  );

});
</script>
