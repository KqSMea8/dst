<br/>
<div class="container">
<form class="form">
<label>ZooKeeper Addr :
<input type="text" name="addr" value="<?php echo $addr; ?>" class="input"/>
</label>
<label>Path :
<input type="text" name="zk_path" value="<?php echo $zk_path;?>" class="input"/>
(You can use the path like "<font color="red"><a href="?addr=<?php echo $addr;?>&depth=<?php echo $max_depth;?>&zk_path=/dstream_press/Config">/dstream_press/Config</a></font>" to see the info who is under /dstream_press/Config )
</label>
<label>Max Depth :
<input type="text" name="depth" value="<?php echo $max_depth;?>" class="input"/>
<input type="submit" value="show" class="btn">
</label>
</form>
<pre>
<?php echo $output;?>
</pre>
</div>
