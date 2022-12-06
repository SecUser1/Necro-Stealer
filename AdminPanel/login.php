<?php
require("mysql.php");
require("config.php");

if(isset($_POST['pass'])){

	setcookie('pwd', md5($_POST['pass']));
	header("location: index.php");
}

?>
<html><head>
<link href="bootstrap.css" rel="stylesheet" id="bootstrap-css">
<link rel="stylesheet" href="w3.css">
<script src="//maxcdn.bootstrapcdn.com/bootstrap/3.3.0/js/bootstrap.min.js"></script>
<script src="//code.jquery.com/jquery-1.11.1.min.js"></script>
<link href="bar.css" rel="stylesheet" id="bootstrap-css">

<link href="https://fonts.googleapis.com/css?family=Open+Sans:300,400,500,600,700&amp;amp;subset=cyrillic" rel="stylesheet">

</head>

<style>
#inner {
  display: table;
  margin: 0 auto;
}

#outer {
  position: absolute;
  width: 100%;
  bottom: 40%;
}
</style>

<span id="warning-container"></span>
<body class="w3-gray"><section>    
        
<div id="outer">
 <div id="inner" class="row">
  <div class="w3-panel w3-border w3-border-gray">
     <span class="w3-xxlarge w3-text-gray">Panel</span>
     <form action="login.php" method="POST" class="login-form">
        <input type="password" class="form-control" placeholder="Password" requigray name="pass">
        <input class="btn w3-round-large w3-border w3-border-gray" style="margin-top:12px;width:325px;height:40px" type="submit" value="Sign in">
     </form>
  </div>
 </div>

</div></section></body></html>