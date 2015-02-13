<h1>Show File</h1>
<table>
<tr>
<th>FileName</th>
<th>FilePath</th>
<th>Download</th>
</tr>
<tr>
<td>
<?php echo $xml->Data->FileName;?>
</td>
<td>
<?php echo $xml->Data->FilePath;?>
</td>
<td><a href="<?php echo $down_url;?>">click to download</a></td>
</tr>

</table>
<pre>
<?php if($xml->Data->Content->Line) foreach($xml->Data->Content->Line as $line){
  echo htmlspecialchars($line);
}
else{
  echo "Can't show it in the web for some reason,Please <a href=\"$down_url\">download it</a>";
}
?>
</pre>
