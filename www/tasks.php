<?php ?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"> 
<? include("database.php"); ?>

<head>
<title>Untitled Document</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
<style type="text/css">
@import "style.css";
</style>
</head>

<body>
<div id="container">

<div id="logo"><img src="img/DiskTransparantSmall.png"></div> 
<div id="header"><p>Forensic Sparse Imager</p>
</div>

<?include ("navigation.php") ?>
<?include ("help.php") ?>

<div id="content">
<h1>Tasks</h1>

<table>

<?

$query="SELECT * FROM tasks,defaulttasks where tasks.defaulttask_id=defaulttasks.id order by name,priority";
$result=mysql_query($query);

while ($row = mysql_fetch_assoc($result)) {
?>
<TR>
<TD><? echo $row["name"]?></TD>
<TD><? echo $row["command"]?></TD>
<TD><? echo $row["state"]?></TD>
<TD><? echo $row["finished"]?></TD>
</TR>
<?
}

?>
</table>


</div>

</div>

</body>
</html>



