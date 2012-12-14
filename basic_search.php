<html>

<head>
	<title>CS 564 PHP Project Basic Search</title>
</head>

 <body>
   <tr>
     <td colspan="2" align="center" valign="top">
      Basic search results (searched by name):<br>
       <table border="1" width="75%">
        <tr>
	 <td align="center" bgcolor="#cccccc"><b>Species ID</b></td>
         <td align="center" bgcolor="#cccccc"><b>Name of Animal</b></td>
         <td align="center" bgcolor="#cccccc"><b>Number of Animal</b></td>
         <td align="center" bgcolor="#cccccc"><b>Square Footage Needed</b></td>
	 <td align="center" bgcolor="#cccccc"><b>Liters of water per day</b></td>
	 <td align="center" bgcolor="#cccccc"><b>Exhibit ID</b></td>
        </tr>

<?php
	// make sure what was submitted is good
	if(! isset($_POST['aName'])) {
	  echo "  <h3><i>Error, name not set to an acceptable value</i></h3>\n".
	       " <a href=\"https://cs564.cs.wisc.edu/nmurray/index.html\">Back to index</a>\n".
	       " </body>\n</html>\n";
	  exit();
	}
	
	$aname = $_POST['aName'];
	// Connect to database

	pg_connect('dbname=cs564_f12 host=postgres.cs.wisc.edu')
		or die ("Couldn't Connect ".pg_last_error());

	// make query
	
	$query = "SELECT * FROM zoolander_schema.species where sname='".$aname."'";
	
	// Execute query

	$result = pg_query($query);
	if(!$result) {
	   $errormessage = pg_last_error();
	   echo "Error with query: " . $errormessage;
	   exit();
	}

	// Print results

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
	<?php echo "<a href=\"https://cs564.cs.wisc.edu/nmurray/index.html\">Return to index</a>\n"?>
</body>
</html>