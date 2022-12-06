<?php
require('config.php');

$base = new PDO('mysql:host='.$db_host.';dbname='.$db_database,$db_username,$db_password);
$base->exec("SET NAMES UTF8");
?>