<?php

#require_once('db_connect.php');
require_once('db_display.php');

?>
<html>

<head>
	<title>Advanced Search Page</title>
</head>

<body>

<h1>CS 564 PHP Project: How Many Animals Do We Have To Take Care Of? (Advanced)</h1>

<tr>
    <td colspan="2" align="center" valign="top">
        Basic search results (searched by name):<br>
        <table border="1" width="75%">
            <tr>
                <td align="center" bgcolor="#cccccc"><b>Name of Animal</b></td>
                <td align="center" bgcolor="#cccccc"><b>Number of Animal</b></td>
                <td align="center" bgcolor="#cccccc"><b>Square Footage Needed</b></td>
                <td align="center" bgcolor="#cccccc"><b>Liters of water per day</b></td>
            </tr>
            <?php
                # Displays the result of the query

                $table_commands = array($_POST['sname'], $_POST['sqftneed'], $_POST['wateramt'], $_POST['num']);
                $table_args = array($_POST['operator_name'], $_POST['operator_sqft'], $_POST['operator_water'], $_POST['operator_num']);      

                $pg_conn = pg_connect('dbname=cs564_f12 host=postgres.cs.wisc.edu')
		            or die ("Couldn't Connect ".pg_last_error());

                get_advanced($pg_conn, $table_args, $table_commands);
            ?>
        </table>
    </td>
</tr>

<?php echo "<a href=\"https://cs564.cs.wisc.edu/nmurray/advanced_search.html\">Return to advanced search</a>\n"?>

</body>

</html>
