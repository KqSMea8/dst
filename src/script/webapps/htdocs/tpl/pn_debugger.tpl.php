<h2>Transporter Queues</h2>

<table class="table table-striped">
<thead>
  <tr>
    <th class="header headerSortDown">PEID</th>
    <th class="header">RecvQueueFree</th>
    <th class="header">SendQueueFree(AVG)</th>
    <th class="header">Branch</th>
    <th class="header">MaxDownQueueSize</th>
    <th class="header">MaxDownQueuePE</th>
  </tr>
</thead>
<?php foreach ($counter as $pe => $cntr) { ?>
<tr>
  <td><?php echo $pe;?></td>
  <td>
    <?php 
      if (isset($cntr['RecvQueue'])) { 
        $free = 100.0 - $cntr['RecvQueue']['Load'];
        echo "<div class=\"progress custom-progress\"><div class=\"bar bar-success\" style=\"width: $free%;\">";
        printf("Used: %d KB / Free: %.2f", $cntr['RecvQueue']['Size']/1024, $free);
        echo '</div></div>';
      } else {
	    echo '<b>NULL</b>';
      }
    ?>
  </td>
  <td>
	<?php 
      if (isset($cntr['SendQueue'])) { 
        $free = 100.0 - $cntr['SendQueue']['LoadAvg'];
        echo "<div class=\"progress custom-progress\"><div class=\"bar bar-success\" style=\"width: $free%;\">";
        printf("Used: %d KB / Free: %.2f", $cntr['SendQueue']['SizeAvg']/1024, $free);
        echo '</div></div>';
      } else {
	    echo '<b>NULL</b>';
      }
	?>
  </td>
  <td>
	<?php 
      if (isset($cntr['SendQueue']['Branch'])) { 
	    echo '<b>' . $cntr['SendQueue']['Branch'] . '</b>';
      }
    ?>
  </td>
  <td>
	<?php 
      if (isset($cntr['SendQueue'])) { 
	    printf("<b>%.2f KB</b>", $cntr['SendQueue']['SizeMax'] / 1024.0);
      }
    ?>
  </td>
  <td>
	<?php 
      if (isset($cntr['SendQueue'])) { 
	    printf("<b>%d</b>", $cntr['SendQueue']['SizeMaxDownPE']);
      }
    ?>
  </td>
</tr>
<?php } // end foreach?>
</table>
