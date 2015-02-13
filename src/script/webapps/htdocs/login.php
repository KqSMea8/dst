<?php 
session_start();
$salt = "admin";
$password = "5bf373041b672679cdb01e03900376a4";
if($_GET["do"]=="login" && md5(md5($_GET['password']).$salt)==$password ){
	$_SESSION['admin']=true;
	header("location:index.php");
}else if($_GET['do']=="login"){
	$msg = "password incorrect";
}

?>
<html>
<head>
<title>
login for admin
</title>
</head>
<body>
<?php echo $msg;?>
<form action="" method="get">
<label>
<input type="password" name="password">
</label>
<input type="submit" name="do" value="login"/>
</form>
</body>
</html>
