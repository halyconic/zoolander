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
        # May want Javascript here if need scrollable

        # Displays the result of the query
        get_browse($pg_conn);
    ?>
    </p>
</body>

</html>
