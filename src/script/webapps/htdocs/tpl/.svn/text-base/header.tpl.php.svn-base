<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title><?php echo "$TPL[title] - $TPL[site_name]";?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
	<meta name="description" content="<?php echo $TPL['description'];?>">
    <!-- Le styles -->
    <link href="/assets/css/bootstrap.css" rel="stylesheet">
    <link href="/assets/css/bootstrap-responsive.css" rel="stylesheet">

    <!-- Le HTML5 shim, for IE6-8 support of HTML5 elements -->
    <!--[if lt IE 9]>
      <script src="http://html5shim.googlecode.com/svn/trunk/html5.js"></script>
    <![endif]-->

	<!-- Le fav and touch icons -->
	<?php if($TPL['icon']){ ?>
	<link rel="shortcut icon" href="$TPL['icon']">
	<?php }?>

	<?php
		if($TPL['import_css']) 
			foreach($TPL['import_css'] as $css){
	?>
		<link type="text/css" rel="stylesheet" href="<?php echo $css;?>"/>
	<?php }?> 
	<script type="text/javascript" src="/assets/js/jquery-1.7.2.min.js"></script>
	<script type="text/javascript" src="/assets/js/bootstrap.min.js"></script>
	<script type="text/javascript" src="/assets/js/jquery.tablesorter.min.js"></script>
	<script type="text/javascript" src="/assets/js/bootstrap-dropdown.js"></script>
	<?php if($TPL['import_js'])
		foreach($TPL['import_js'] as $js)
	{?>
		<script type="text/javascript" src="<?php echo $js;?>"></script>
	<?php }?>

  </head>
  
<body data-spy="scroll" data-target=".subnav" data-offset="50">
  <!-- Navbar
    ================================================== -->
    <div class="navbar navbar-fixed-top">
      <div class="navbar-inner">
        <div class="container">
          <a class="btn btn-navbar" data-toggle="collapse" data-target=".nav-collapse">
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </a>
		  <a class="brand" href="<?php echo $TPL['menu_title_link'];?>"><?php echo $TPL['menu_title'];?></a>
          <div class="nav-collapse collapse">
			<ul class="nav">
			<?php if($TPL['menu']) foreach($TPL['menu'] as $item=>$link){ ?>
			<li class="<?php echo $TPL['active_menu']==$item?"active":"";?>">
			  <a href="<?php echo $link;?>"><?php echo $item;?></a>
			</li>
			<?php  }?>
				</ul>
			<ul class="nav pull-right">
			<li class="dropdown" id="menu1">
			<a class="dropdown-toggle" data-toggle="dropdown" href="#menu1">
				Change Sub System<b class="caret"></b>
			</a>
			<ul class="dropdown-menu">
			<?php if($TPL['right_menu']) foreach($TPL['right_menu'] as $item=>$link){ ?>
				<li class="<?php echo $TPL['active_right_menu']==$item?"active":"";?>">
				<a href="<?php echo $link;?>"><?php echo $item;?></a>
				</li>
			<?php  }?>
			</ul>
				<?php if($TPL['menu_search_form']){?>
			<li style="height:40px;line-height:40px">
			<form action="<?php echo $TPL['menu_search_form']['action'];?>" method="<?php echo $TPL['menu_search_form']['method'];?>" class="form-search">
			<input type="text" class="input-medium search-query" name="<?php echo $TPL['menu_search_form']['search_name']; ?>" value="<?php echo $TPL['menu_search_form']['default_text'];?>"/>
			<?php if($appid){?><input type="hidden" name="appid" value="<?php echo $appid;?>" /><?php }?>
			<input type="submit" value="go to this path"/>
			</form></li>
			<?php }?>
	</ul>
          </div>
        </div>
      </div>
    </div>

<div style="height:40px">
</div>
