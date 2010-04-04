<?php 
header('Location: status.php');

include("database.php");

$query="UPDATE systems SET requestedstate='dead'  where name=\"".$_GET["name"]."\"";
$result=mysql_query($query); 

usleep(5000000);

$query="DELETE FROM systems where currentstate='dead'  and name=\"".$_GET["name"]."\"";
$result=mysql_query($query);
$query="DELETE FROM tasks where name=\"".$_GET["name"]."\"";
$result=mysql_query($query);



?>
