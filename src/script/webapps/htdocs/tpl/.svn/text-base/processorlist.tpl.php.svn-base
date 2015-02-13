<h1><a href="index.php?path=<?php echo $app_path;?>">Dstream Job Display</a></h1>
<table class="table table-striped">
  <thead>
    <tr>
      <th class="header headerSortDown" style="width:75px">ProcessorID</th>
      <th class="header">ProcessorName</th>
      <th class="header">ProcessorRole</th>
      <th class="header">PE Num</th>
      <th class="header">avg recv QPS</th>
      <th class="header">recv count</th>
      <th class="header">total recv QPS</th>
      <th class="header">avg send QPS</th>
      <th class="header">send count</th>
      <th class="header">total send QPS</th>
    </tr>
  </thead>
  <tbody>
<?php if($all_processor) 
        foreach($all_processor as $processor){ 
?>
  
    <tr> 
       <td><?php echo $processor['processor id'];?></td>
       <td><a href="pelist.php?appid=<?php echo $appid; ?>&processorid=<?php echo $processor['processor id']; ?>"><?php echo $processor['processor name'];?></a></td>
       <td><?php echo $processor['processor role'];?></td>
       <td><?php echo $processor['pe num'];?></td>
       <td><?php echo $processor['avg recv QPS'];?></td>
       <td><?php echo $processor['recv counter'];?></td>
       <td><?php echo $processor['recv QPS'];?></td>
       <td><?php echo $processor['avg send QPS'];?></td>
       <td><?php echo $processor['send counter'];?></td>
       <td><?php echo $processor['send QPS'];?></td>
    </tr>

<?php } ?>

</tbody>
</table>

<script>$(document).ready(function() {
$("table").each(function(i) { $(this).tablesorter({ sortList: [[0,0]], headers: {}}); });
});
</script>
