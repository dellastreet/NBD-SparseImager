<?php

include("config.php");

class ImageDatabase
{
    // property declaration
    public $var = 'a default value';

    function __construct()
    {
	global $username,$password,$database;
	mysql_connect(localhost,$username,$password) or die ( "Unable to oped connection");
	@mysql_select_db($database) or die( "Unable to select database");
    }

    // method declaration
    public function displayVar() {
        echo $this->var;
    }

    public function expire() {
        global $expiretime;
        $query="UPDATE systems SET requestedstate='dead' WHERE (NOW()-seenat)>".$expiretime.";";  
	$result=mysql_query($query) or die ("Failed ".$query);
	echo $result;
    }

    public function changestates() {
        $query="UPDATE systems SET currentstate='dead' WHERE requestedstate='dead';";  
	$result=mysql_query($query) or die ("Failed ".$query);
    }
    public function closedb()
    {
	mysql_close();
    }
}


$db= new ImageDatabase();


?>

