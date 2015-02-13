<h1><a href="index.php?path=<?php echo $app_path;?>">Dstream Job Display</a></h1>
<table class="table table-striped">
<thead>
<th class="header headerSortDown">total cpu</th>
<th class="header">cpu utilize</th>
<th class="header">total memory</th>
<th class="header">memory utilize</th>
<th class="header">total network</th>
<th class="header">network utilize</th>
<th class="header">total disk</th>
<th class="header">disk utilize</th>
</thead>
<tbody>

<tr>
<td><?php echo $pn['total_cpu'];?></td>
<td><?php echo $pn['cpu_utilize'];?></td>
<td><?php echo $pn['total_memory'];?></td>
<td><?php echo $pn['memory_utilize'];?></td>
<td><?php echo $pn['total_network'];?></td>
<td><?php echo $pn['network_utilize'];?></td>
<td><?php echo $pn['total_disk'];?></td>
<td><?php echo $pn['disk_utilize'];?></td>
</tr>
</tbody>
</table>
<h3>Tuple Queues Status</h3>
<table class="table table-striped">
<thead>
<th class="header headerSortDown">queue_id</th>
<th class="header">pe_id</th>
<th class="header">drop_tuples</th>
<th class="header">buffer_tuples</th>
<th class="header">memory_used</th>
</thead>
<tbody>
<?php if($arr['queues']) foreach($arr['queues'] as $queue ){ ?>
<tr>
<td><?php echo $queue['queue_id'][0];?></td>
<td><?php echo $queue['pe_id'][0]['id'][0];?></td>
<td><?php echo $queue['drop_tuples'][0];?></td>
<td><?php echo $queue['buffer_tuples'][0];?></td>
<td><?php echo $queue['memory_used'][0];?></td>
</tr>
<?php }?>


</tbody>
</table>
<script>
$(document).ready(function() {
$("table").each(function(i) { $(this).tablesorter({ sortList: [[0,0]], headers: {}}); });
});
</script>

