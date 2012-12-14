<html>

<head>
	<title>CS 564 PHP Project: How Many Animals Do We Have To Take Care Of?</title>
</head>
<body>
  <tr>
    <td colspan="2" align="center" valign="top">
     List of all animals in the zoo!:<br>
	<table border="1" width="75%">
	  <tr>
	    <td align="center" bgcolor="#cccccc"><b>Name of Animal</b></td>
            <td align="center" bgcolor="#cccccc"><b>Number of Animal</b></td>
            <td align="center" bgcolor="#cccccc"><b>Square Footage Needed</b></td>
	    <td align="center" bgcolor="#cccccc"><b>Liters of water per day</b></td>
	  </tr>

<?php
	//Connect to database
	pg_connect('dbname=cs564_f12 host=postgres.cs.wisc.edu')
		or die ("Couldn't Connect ".pg_last_error());

	//Make query
	$query = "SELECT * FROM zoolander_schema.species";

	//Execute query
	$result = pg_query($query);
	if(!$result) {
	   $errormessage = pg_last_error();
	   echo "Error with query: " . $errormessage;
	   exit();
	}

	//Print results
	while($row = pg_fetch_array($result,NULL,PGSQL_ASSOC)) {
	   echo "        <tr>";
	   echo "\n         <td align=\"center\">";
	   echo "\n          ".$row['sid'];
	   echo "\n         </td>";
    	   echo "\n         <td align=\"center\">";
	   echo "\n          ".$row['sname'];
    	   echo "\n         </td>";
    	   echo "\n         <td align=\"center\">";
    	   echo "\n          ".$row['num'];
    	   echo "\n         </td>";
      	   echo "\n         <td align=\"center\">";
    	   echo "\n          ".$row['sqftneed'];
    	   echo "\n         </td>";
	   echo "\n         <td align=\"center\">";
	   echo "\n          ".$row['wateramt'];
	   echo "\n         </td>";
	   echo "\n         <td align=\"center\">";
	   echo "\n          ".$row['eid'];
	   echo "\n         </td>";
    	   echo "\n        </tr>";
	}
	pg_close();
?>
</table>
	</td>
	</tr>
	<?php echo "<a href=/"https://cs564.cs.wisc.edu/nmurray/index.html\">Return to index</a>\n"?>
</body>
</html>