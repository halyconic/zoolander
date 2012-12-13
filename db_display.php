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
function get_advanced($pg_conn)
{
    $result = pg_query($pg_conn, "SELECT advanced, attribute FROM member WHERE member.id=1337");

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

