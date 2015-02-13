
<?php if($_GET['do']=='search_log'){?><form class="well form-inline">
<fieldset>

<label>Name:<input type="text" class="small" name="name" value="<?php echo $_GET['name'];?>"/></label>
<label>SenderIP<input type="text" class="small" name="sender_ip" value="<?php echo $_GET['sender_ip'];?>"/></label>
<label>Priority:
<select name="log_priority" class="small">
<option value="">unchoose</option>
<option value="ERROR" <?php if($_GET['log_priority']=="ERROR") echo "selected";?>>ERROR</option>
<option value="WARN" <?php if($_GET['log_priority']=="WARN") echo "selected";?>>WARN</option>
<option value="INFO" <?php if($_GET['log_priority']=="INFO") echo "selected";?>>INFO</option>
<option value="DEBUG" <?php if($_GET['log_priority']=="DEBUG") echo "selected";?>>DEBUG</option>
</select>
</label>
<label>time_begin<input type="text" name="log_time_begin" style="width:120px" value="<?php echo $_GET[log_time_begin];?>"/></label>
<label>time_end<input type="text" name="log_time_end" style="width:120px" value="<?php echo $_GET[log_time_end];?>"/></label>
<label>Sender:<input type="text" class="small" name="log_sender" value="<?php echo $_GET[log_sender];?>"/></label>
<label>Message:<input type="text"  name="log_message" value="<?php echo $_GET[log_message];?>"/></label>
<label>data0:<input type="text"  class="small" name="data0" value="<?php echo $_GET[data0];?>"/></label>
<label>data1:<input type="text"  class="small" name="data1" value="<?php echo $_GET[data1];?>"/></label>
<label>data2:<input type="text" class="small"  name="data2" value="<?php echo $_GET[data2];?>"/></label>
<label>data3:<input type="text"  class="small" name="data3" value="<?php echo $_GET[data3];?>"/></label>
<label>data4:<input type="text"  class="small" name="data4" value="<?php echo $_GET[data4];?>"/></label>
<label>columns:<input type="text" name="columns" value="<?php echo $_GET['columns'];?>"/>
</label><label>order by:<input type="text" name="orderby" value="<?php echo $_GET['orderby'];?>"/></label>
<label>elem number per page:<input type="text" name="perpage" value="<?php echo $_GET['perpage'];?>"/> </label>
<label><input type="hidden" name="do" value="search"/></label>
<label><input type="submit" value="search" class="btn"/></label>
</fieldset>
</form>
<label>
<input type="button" value="add search_result as a view" id= "add_view" class="btn"/>
</label>
<?php }?>
<?php if ($_GET['do']=='view'){
   if ($view) {
      echo '<h2>' . $view . '</h2>';
      echo '<h3>' . $view_create . '</h3>';
   }
?>
<?php showpages($page,$total_page,$showpage,$prefix_of_pager);
?>
<table class="table table-striped">
	<thead>
	<tr>
		<?php if ($show_add){?><th>Operate</th><?php }?>
		<?php while($field=$result->fetch_field()){  ?>
		<th><?php echo $field->name; ?></th>
		<?php }?>
	</tr>
	</thead>
<tbody>
		<?php while($row=$result->fetch_row()){ ?>
		<tr <?php  if($show_add) echo "id=\"view_".$row[0]."\"";?>>
<?php if ($show_add) {?><td><a class="btn small" href="?do=view&view=<?php echo urlencode($row[0]);?>" >ViewDetails</a><?php if ($row[1]=="VIEW"){;?><input type="button" class="btn small" value="DeleteView" onclick="if(confirm('confirm to delete this view?'))delete_view('<?php echo $row[0];?>')"><?php }?>  </td>  <?php }?>
		<?php foreach($row as $item) {?>
		<td><?php echo $item;?></td>
		<?php }?>
</tr>
		<?php }?>
</tbody>

</table>
<?php if($show_add){?>
<label>name:<input type="text" name="view" id="sql_view_name"/></label>
<label>SQL:<textarea name="cmd" id="sql_view_cmd"></textarea></label>
<input type="hidden" name="do" value="add"/>
<input type="button" class="btn" value="add" id="add_view_by_sql"/>
<?php }?>
<?php } else if ($_GET['do'] == 'search_counter') { // end $_GET['do'] == view?>
<form class="well form-inline">
<fieldset>
<label>Host:<input type="text" class="small" name="machine_name" value="<?php echo $_GET['machine_name'];?>"/></label>
<label>Cluster:<input type="text" class="small" name="cluster_name" value="<?php echo $_GET['cluster_name'];?>"/></label>
<label>Time Begin:<input type="text" name="c_time_begin" value="<?php echo $_GET['c_time_begin'];?>"/></label>
<label>Time End:<input type="text" name="c_time_end" value="<?php echo $_GET['c_time_end'];?>"/></label>
<label>Counter Name:<input type="text" class="small" name="c_name" value="<?php echo $_GET['c_name'];?>"/></label>
<label>Attr1:<input type="text"  name="c_attr1" value="<?php echo $_GET['c_attr1'];?>"/></label>
<label>Attr2:<input type="text"  class="small" name="c_attr2" value="<?php echo $_GET['c_attr2'];?>"/></label>
<label>Attr3:<input type="text"  class="small" name="c_attr3" value="<?php echo $_GET['c_attr3'];?>"/></label>
<label>Type:<input type="text" class="small"  name="type" value="<?php echo $_GET['c_type'];?>"/></label>
<label>order by:<input type="text" name="orderby" value="<?php echo $_GET['orderby'];?>"/></label>
<label>elem number per page:<input type="text" name="perpage" value="<?php echo $_GET['perpage'];?>"/> </label>
<label><input type="hidden" name="do" value="search_counter"/></label>
<label><input type="submit" value="search" class="btn"/></label>
</fieldset>
</form>

<input type="button" value="add search_result as a view" id= "add_view" class="btn"/>
</label>
<?php showpages($page,$total_page,$showpage,$prefix_of_pager);?>
<table class="table table-striped">
<thead>
<tr>
<?php while($field=$result->fetch_field()){  ?>
	<th><?php echo $field->name; ?></th>
		<?php }?>
</tr>
</thead>
<tbody>
<?php while($row=$result->fetch_row()){ ?>
<tr>
<?php foreach($row as $item) {?>
<td><?php echo $item;?></td>
<?php }?>
</tr>
<?php }?>
</tbody>

</table>
<?php } // end $_GET['do'] == search_counter?>
<script>
function handle_msg(success_function){
	return function(e){
		if(e.success) {
			alert(e.message);
			if(success_function!=null) success_function(e);
		}
		else alert(e.message);
		}
}
function delete_view(view_name)
{
	$.getJSON("ajax/log_metrics.php",{"do":"delete","view":view_name},handle_msg(function(){
		$("#view_"+view_name).hide();
	}));
}

	$(document).ready(function() {
		$("table").each(function(i) { $(this).tablesorter({ headers: {}}); });
<?php if($_GET['do']=='search_log' || $_GET['do']=='search_counter'){ ?>
		$("#add_view").click(function(){
			var view = window.prompt("please input a view name");
			if(view==null) return;
		$.getJSON("ajax/log_metrics.php",{"do":"add","view":view,"cmd":"<?php echo $sqlcmd?>"},handle_msg());
		});
<?php }?>
		$("#add_view_by_sql").click(function(){
			$.getJSON("ajax/log_metrics.php",{"do":"add","view":$("#sql_view_name").val(),"cmd":$("#sql_view_cmd").val()},handle_msg(function(){window.location.reload(true);}));
		});
	});

	</script>
