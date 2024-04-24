<?php

$hostname = "localhost";
$username = "root";
$password = "";
$database = "esp32_sensor";

$conn = mysqli_connect($hostname, $username, $password, $database);

if(!$conn){
	die("connection failed:" . mysqli_connect_error());
}

echo "database connection is ok.";
if(isset($_POST["Temperature"])&&isset($_POST["Pulse"])){
  
  $temp=$_POST["Temperature"];
  $pulse=$_POST["Pulse"];

  $sql = "INSERT INTO data (Temperature, Pulse) VALUES (".$temp.", ".$pulse.")";
  
  if(mysqli_query($conn,$sql)){
    echo " New record created succesfully";
  }
  else{
    echo "error: " . $sql . "<br>" . mysqli_error($conn);
  }

}
?>