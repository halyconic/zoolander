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
function get_advanced($pg_conn, $table_args)
{
    $query = 'SELECT sname, sqftneed, wateramt, num FROM species';

    $is_first = true;

    # check if both params exist before appending to query
    if (isset($table_args['sname'])) {
        if (isset($table_args['operator_name'])) {
            if ($is_first) {
                $query = $query . ' WHERE';
                $is_first = false;
            }
            else {
                $query = $query . ' AND';
            }
        }
    }
    if (isset($table_args['sqftneed'])) {
        if (isset($table_args['operator_sqft'])) {
            if ($is_first) {
                $query = $query . ' WHERE';
                $is_first = false;
            }
            else {
                $query = $query . ' AND';
            }
        }
    }
    if (isset($table_args['wateramt'])) {
        if (isset($table_args['operator_water'])) {
            if ($is_first) {
                $query = $query . ' WHERE';
                $is_first = false;
            }
            else {
                $query = $query . ' AND';
            }
        }
    }
    if (isset($table_args['num'])) {
        if (isset($table_args['operator_num'])) {
            if ($is_first) {
                $query = $query . ' WHERE';
                $is_first = false;
            }
            else {
                $query = $query . ' AND';
            }
        }
    }



    $result = pg_query($pg_conn, $pq_query);

    if (!$result) {
        echo "An error occured.\n";
        exit;
    }

    # print here
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

