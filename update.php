<?php

require_once('db_connect.php');
require_once('db_display.php');

?>
<html>

<head>
	<title>Update Page</title>
</head>

<body>
    <p>
    <?php
        # Displays the result of the query
        get_update($pg_conn);
    ?>
    </p>
</body>

</html>
