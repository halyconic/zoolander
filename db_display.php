<?php

#TODO: Change the queries!

# This file performs all the queries. Call these functions after connecting to the database and pass in the connection

# This prints into a javascript array. This may not be what you need, but its a good example
# You can also print out into html for a simpler presentation
function get_basic_js($pg_conn)
{
    $result = pg_query($pg_conn, "SELECT query, stuff FROM table");

    if (!$result) {
        echo "An error occured.\n";
        exit;
    }

    # print items side-by-side into an array, assumes at least one entry exists already
    print("var stuffArray = [");
    $first_row = pg_fetch_row($result);
    print("'$first_row[0] $first_row[1]'");
    while ($row = pg_fetch_row($result)) {
        print(",'$row[0] $row[1]'");
    }
    print("];");
}

# Print into HTML
# Basic query
function get_basic($pg_conn)
{
    $result = pg_query($pg_conn, "SELECT query, stuff FROM table WHERE table.id = 1");

    if (!$result) {
        echo "An error occured.\n";
        exit;
    }

    # print here
    print("Successful!");
}

# Advanced query
# http://www.postgresql.org/docs/9.1/static/sql-select.html
function get_advanced($pg_conn, $table_args, $table_commands)
{
    $query = 'SELECT sname, sqftneed, wateramt, num FROM zoolander_schema.species';

    $param_num = 1;

    $stack = array();

    # check if both params exist before appending to query
    if (isset($table_commands['sname'])) {
        if (isset($table_args['operator_name']) && $table_args['operator_name'] != 'none') {
            if ($param_num == 1) {
                $query = $query . ' WHERE';
            }
            else {
                $query = $query . ' AND';
            }

            if ($table_args['operator_name'] == 'exactly') {
                $query = $query . ' sname=$' . $param_num;
            }
            else if ($table_args['operator_name'] == 'like') {
                # TODO            
                $query = $query . ' sname=$' . $param_num;
            }
            else if ($table_args['operator_name'] == 'unlike') {
                # TODO            
                $query = $query . ' sname=$' . $param_num;
            }

            $query = $query . '$' . $param_num;

            # Add to array
            $stack[] = $table_commands['sname'];
            $param_num++;
        }
    }
    if (isset($table_commands['sqftneed'])) {
        if (isset($table_args['operator_sqft']) && $table_args['operator_sqft'] != 'none') {
            if ($param_num == 1) {
                $query = $query . ' WHERE';
            }
            else {
                $query = $query . ' AND';
            }

            $query = $query . ' sqftneed' . $table_args['operator_sqft'] . ' $' . $param_num;

            # Add to array
            $stack[] = $table_commands['sqftneed'];
            $param_num++;
        }
    }
    if (isset($table_commands['wateramt'])) {
        if (isset($table_args['operator_water']) && $table_args['operator_water'] != 'none') {
            if ($param_num == 1) {
                $query = $query . ' WHERE';
            }
            else {
                $query = $query . ' AND';
            }

            $query = $query . ' wateramt' . $table_args['operator_water'] . ' $' . $param_num;

            # Add to array
            $stack[] = $table_commands['wateramt'];
            $param_num++;
        }
    }
    if (isset($table_commands['num'])) {
        if (isset($table_args['operator_num']) && $table_args['operator_num'] != 'none') {
            if ($param_num == 1) {
                $query = $query . ' WHERE ';
            }
            else {
                $query = $query . ' AND ';
            }

            $query = $query . ' num' . $table_args['operator_num'] . ' $' . $param_num;

            # Add to array
            $stack[] = $table_commands['num'];
            $param_num++;
        }
    }

    $result = pg_query_params($pg_conn, $query, $stack);

    if (!$result) {
        echo "An error occured.\n";
        echo $query;
        $errormessage = pg_last_error();
	    echo "Error with query: " . $errormessage;
        exit();
    }

    # print here

    while($row = pg_fetch_array($result,NULL,PGSQL_ASSOC)) {
        echo "        <tr>";
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
        echo "\n        </tr>";
    }

    pg_close();
}

# Update query
# http://www.postgresql.org/docs/9.1/static/sql-insert.html
function get_update($pg_conn)
{
    $result = pg_query($pg_conn, "INSERT INTO yourmom VALUES ('me', 'alex')");

    if (!$result) {
        echo "An error occured.\n";
        exit;
    }

    # print here
}

# Browse query
# http://www.postgresql.org/docs/9.1/static/sql-insert.html
function get_browse($pg_conn)
{
    $result = pg_query($pg_conn, "SELECT id FROM bookshelf");

    if (!$result) {
        echo "An error occured.\n";
        exit;
    }

    # print here
}

?>

