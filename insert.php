<html>

<head><title>CS 564 PHP Project Insert Result Page</title></head>

<body>

<?php
  // grab values from form
  $aname = $_POST['aname'];
  $num = $_POST['num'];
  $sqft = $_POST['sqft'];
  $wateramt = $_POST['wateramt'];
  $exhibit = $_POST['exhibit'];
  $id = $_POST['id'];

  // connect to database
  pg_connect('dbname=cs564_f12 host=postgres.cs.wisc.edu')
	or die ("Couldnt' Connect ".pg_last_error());

  //Make the query
  if( strlen($aname) > 0 && strlen($num) > 0 && strlen($sqft) > 0 && strlen($wateramt) > 0 && strlen($wateramt) > 0 && strlen($exhibit) > 0 && strlen($id) > 0) {
	$query = "insert into zoolander_schema.species values (".$id.", '".$aname."', ".$num.", ".$wateramt.", ".$sqft.", ".$exhibit.")";
  }else{
	echo "  <h3><i> not all fields filled in</i></h3>\n".
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