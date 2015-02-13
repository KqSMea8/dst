<h1>Directory <?php echo $dir?></h1>
<table class="table table-striped">
<thead>
<th class="header">FileType</th>
<th class="header headerSortDown">Name</th>
<th class="header">Permission</th>
<th class="header">Blocks</th>
<th class="header">Owner</th>
<th class="header">Group</th>
<th class="header">Size</th>
<th class="header">ModifyTime</th>
</thead>

<?php
if($xml->Data->FileItem) foreach($xml->Data->FileItem as $file) {
if($file->Name==".") continue;
?>
<tr>
<td><?php echo $file->FileType;?></td>
<td><a href="
<?php
  if($file->FileType=="Dir")
    echo "?appid=$_GET[appid]&path=$_GET[path]&type=$_GET[type]&id=$_GET[id]&dir=";
  else if($file->FileType=="Text")
    echo "cat.php?appid=$_GET[appid]&path=$_GET[path]&type=$_GET[type]&id=$_GET[id]&dir=";
  else echo "down.php?appid=$_GET[appid]&path=$_GET[path]&type=$_GET[type]&id=$_GET[id]&dir=";
  
  if($file->Name == "..") 
    echo dirname($req_dir);
  else echo $req_dir."/".$file->Name;    
?>
">
<?php echo $file->Name;?></a></td>
<td><?php echo $file->Permission;?></td>
<td><?php echo $file->Blocks;?></td>
<td><?php echo $file->Owner;?></td>
<td><?php echo $file->Group;?></td>
<td><?php echo $file->Size;?></td>
<td><?php echo $file->ModifyTime;?></td>
</tr>
<?php } ?>
</table>
<script>
$(document).ready(function() {
$("table").each(function(i) { $(this).tablesorter({ sortList: [[1,0]], headers: {}}); });
});
</script>
