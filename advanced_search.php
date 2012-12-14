<?php

require_once('db_connect.php');
require_once('db_display.php');

?>
<html>

<head>
	<title>Advanced Search Page</title>
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
                # Displays the result of the query
                get_advanced($pg_conn);
            ?>
        </table>
    </td>
</tr>

</body>

</html>
