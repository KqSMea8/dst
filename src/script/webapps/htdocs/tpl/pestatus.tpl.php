<h1><a href="index.php?path=<?php echo $app_path;?>">Dstream Job Display</a></h1>
<table class="table table-striped">
<thead>
<th class="header headerSortDown">BackupPEID</th>
<th class="header">PEID</th>
<th class="header">app id</th>
<th class="header">cpu used</th>
<th class="header">cpu limit</th>
<th class="header">memory used</th>
<th class="header">memory limit</th>
<th class="header">network used</th>
<th class="header">network limit</th>
<th class="header">disk used</th>
<th class="header">disk limit</th>
<th class="header">tuples processed</th>
</thead>
<tbody>
<tr>
<td><?php echo $backuppeid;?></td>
<td><?php echo $status['pe_id']['id'][0];?></td>
<td><?php echo $status['app_id']['id'][0];?></td>
<td><?php echo $status['cpu_used'];?>%</td>
<td><?php echo $status['cpu_limit']*100;?>%</td>
<td><?php echo $status['memory_used'];?>KB</td>
<td><?php echo $status['memory_limit']/1024;?>KB</td>
<td><?php echo $status['network_used'];?></td>
<td><?php echo $status['network_limit'];?></td>
<td><?php echo $status['disk_used'];?></td>
<td><?php echo $status['disk_limit'];?></td>
<td><?php echo $status['tuples_processed'];?></td>
</tr>
</tbody>
</table>
<table class="table table-striped">
<thead>
<th class="header headerSortDown">metric name</th>
<th class="header">metric value</th>
</thead>
<tbody>

<?php if($status['metric_name']) foreach($status['metric_name'] as $key=>$name){ ?>
<tr>
<td><?php echo htmlspecialchars($name);?></td>
<td><?php echo htmlspecialchars($status['metric_value'][$key]);?></td>
</tr>
<? }?>
<tr><td>Revision</td><td><?php echo $status['revision'];?></td></tr>
</tbody>
</table>

<script>
$(document).ready(function() {
$("table").each(function(i) { $(this).tablesorter({ sortList: [[0,0]], headers: {}}); });
});
</script>
