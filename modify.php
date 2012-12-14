<html>

<head><title>CS 564 PHP Project Modify Result Page</title></head>

<body>

<?php
  // make sure the animal name is set
  if (!isset($_POST['aname']) ) {
	echo "  <h3><i>Error, name not set to an acceptable value</i></h3>\n".
	     " <a href=\"https://cs564.cs.wisc.edu/nmurray/index.html\">Back to index</a>\n".
 	     " </body>\n</html>\n";
	exit();
  }

  //grab values from form
  $aname = $_POST['aname'];
  $num = $_POST['num'];
  $sqft = $_POST['sqft'];
  $wateramt = $_POST['wateramt'];
  $exhibit = $_POST['exhibit'];

  //connect to database
  pg_connect('dbname=cs564_f12 host=postgres.cs.wisc.edu')
	or die ("Couldn't Connect ".pg_last_error());
  
  //Make the query
  if( strlen($num) > 0 || strlen($sqft) > 0 || strlen($wateramt) > 0 || strlen($exhibit) > 0) {
	$commacheck = 0;
	$query = "update zoolander_schema.species set";
	if(strlen($num)) {
	$query .= " num=".$num;
	$commacheck = 1;
	}
	if($commacheck == 1 && strlen($sqft)){
	$query .= ",";
	$commacheck = 0;
	}
	if(strlen($sqft)){
	$query .= " sqftneed=".$sqft;
	$commacheck = 1;
	}
	if($commacheck == 1 && strlen($wateramt)){
	$query .= ",";
	$commacheck = 0;
	}
	if(strlen($wateramt)){
	$query .= " wateramt=".$wateramt;
	$commacheck = 1;
	}
	if($commacheck == 1 && strlen($exhibit)){
	$query .= ",";
	$commacheck = 0;
	}
	if(strlen($exhibit)){
	$query .= " eid=".$exhibit;
	$commacheck = 1;
	}
	$query .= " where sname='".$aname."'";

  }else{
	echo "  <h3><i>no field to be updated</i></h3>\n".
	     " <a href=\"https://cs564.cs.wisc.edu/nmurray/update.html\">Back to update page</a>\n".
	     " </body>\n</html>\n";
	exit();
  }

  // Execute query
  $result = pg_query($query);
  if(!$result) {
    $errormessage = pg_last_error();
    echo "Error with query: " . $errormessage;
    exit();
  }
  echo "  <h3>Update Successful</h3>";
  
  pg_close();
?>
	<?php echo "<a href=\"https://cs564.cs.wisc.edu/nmurray/update.html\">Back to update page</a>\n"?>

</body>
</html>