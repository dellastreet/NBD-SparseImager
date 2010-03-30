<?php 
header('Location: status.php');

include("database.php"); 

$query="DELETE FROM systems where name=\"".$_GET["name"]."\"";
$result=mysql_query($query);


?>
