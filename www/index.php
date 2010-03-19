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

<div id="sub_content">
<p><span class="caps">NBD</span>SparseImager is a solution for imaging medium quantities of hard-disks for forensic analysis.</p>
<p>The solution is intended to solve the folowing problems:</p>
<ul>
	<li>Disks get bigger all the time</li>
	<li>Full imaging of several 1T disks becomes inpractical in terms of storage</li>

	<li>Full imaging of several 1T disks becomes inpractical in terms of time</li>
	<li>Depending on the type of investigation not all data is as relevant (e.g. content of dll&#8217;s and executables in general are not that relevant)</li>
</ul>
</div>

<div id="content">
<h1>Status</h1>

<table>

<?
$db->expire("2");

$query="SELECT * FROM systems";
$result=mysql_query($query);

while ($row = mysql_fetch_assoc($result)) {
?>
<TR>
<TD><? echo $row["name"]?></TD>
<TD><? echo $row["ipadress"]?></TD>
<TD><? echo $row["currentstate"]?></TD>
<TD><? echo $row["requestedstate"]?></TD>
</TR>
<?
}


//DEBUG STATEMENT! Should be done from task-running app
$db->changestates();
?>
</table>


</div>

</div>

</body>
</html>



