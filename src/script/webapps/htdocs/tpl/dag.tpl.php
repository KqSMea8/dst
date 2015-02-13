<h2>Topology</h2>
<script type="text/javascript" src="chrome-extension://kajfghlhfkcocafkcjlajldicbikpgnp/catcher.js"><!-- script injected by Request Maker --></script><head><meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
        
    <!-- setup extjs 4.1 -->
    <link rel="stylesheet" type="text/css" href="../assets/js/ext-all.css">   
    <script type="text/javascript" charset="utf-8" src="../assets/js/ext-all.js"></script><style type="text/css"></style>
      
    <!-- our libs -->
    <script type="text/javascript" src="../assets/js/utils.js"></script>
    <script type="text/javascript" src="../assets/js/daglayout.js"></script>
    <script type="text/javascript" src="../assets/js/DagDrawer.js"></script>
<script type="text/javascript" src="chrome-extension://ggfnopabjhlbcmchfajkadagogmohglj/js/page.js"></script></head>

<script type="text/javascript" charset="utf-8" id="example">	
<?php
foreach ($node_info_array as $node_info) {
    $arr = explode(",", $node_info);
    if(count($arr) != 4) {
        continue;
    }
    $id = $arr[0];
    $app_id = $arr[1];
    $processor_id = $arr[2];
    $processor_name = $arr[3];
    $node_assemble_array[] =
        "'$id':{'link':'./pelist.php?appid=$app_id&processorid=$processor_id', 'label':'$processor_name'}";
}
$node_str = implode(",", $node_assemble_array);
echo "var nodes = { $node_str};\n";

echo "var edges = $topo;\n";

?>
	var dag = {'nodes':nodes, 'edges':edges};
	var dag_settings = {
		nodes: {
			label: {
				marginWidth: 10,
				marginHeight: 5,
			},
			rect: {
				width: 50,
				height: 50,
				stroke: 'gray',
			},
		},
        edges: {
            color: "red",
            arrow_height: 6,
        },
		interaction: {
			onMouseOver: function(label, rect, mask, settings) {
				var group = Ext.create('Ext.draw.CompositeSprite', {
					surface: this.surface
				});
				group.add(label);
				group.add(rect);
				
				group.animate({
					to: {
					    rotate: {
                            degrees:360
                        },
						scale: {
							x: 1.2,
							y: 1.2
						}
					}
				});
				label.animate({
					to: {
						fill: (rect.attr.fill == 'orange') ? 'gray' : 'CornflowerBlue'
					},
				});
			},
			onMouseOut: function(label, rect, mask, settings) {
				var group = Ext.create('Ext.draw.CompositeSprite', {
					surface: this.surface
				});
				group.add(label);
				group.add(rect);
				
				group.animate({
					to: {
					    rotate: {
                            degrees:0
                        },
						scale: {
							x: 1,
							y: 1
						}
					}
				});
				label.animate({
					to: {
						fill: 'black'
					},
				});
			},
		}    
	};
	
	Ext.create("DagDrawer",{	
		settings: dag_settings,
		dag: dag, 
		computeLayout: maxUpOrDownLayerLayout,
		
		width : 1200,<?php
        $height = 150*count($node_assemble_array);
        $height = $height > 700 ? 700 : $height;
		echo "height : $height,"?>
		
		draggable: {
			constrain: true,
			constrainTo: Ext.getBody()
		},
		resizable: {
			constrain: true,
			constrainTo: Ext.getBody(),
			dynamic: true,
			pinned: true,
			handles: 'all'
		},
	 
		renderTo : Ext.getBody(),
	});
    </script>
