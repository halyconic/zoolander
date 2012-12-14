<html>

<head><title>CS 564 PHP Project Delete Result Page</title></head>

<body>

<?php
  // make sure the animal name is set
  if(!isset($_POST['aname'])) {
	echo "  <h3><i>Error, name not set to an acceptable value</i></h3>\n".
	     " <a href=\"https://cs564.cs.wisc.edu/nmurray/update.html\">Back to update page</a>\n".
	     " </body>\n</html>\n";
	exit();
  }

  //grab values from form
  $aname = $_POST['aname'];

  //connect to database
  pg_connect('dbname=cs564_f12 host=postgres.cs.wisc.edu')
	or die ("Couldn't Connect ".pg_last_error());

  // Make the query
  $query = "DELETE FROM zoolander_schema.species where sname='".$aname."'";

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