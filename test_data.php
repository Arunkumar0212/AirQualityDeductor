 <?php
$hostname = "localhost:3308"; // Change this to your MySQL hostname
$username = "root"; // Change this to your MySQL username
$password = ""; // Change this to your MySQL password
$database = "airindex"; // Change this to your MySQL database name

// Establish database connection
$conn = mysqli_connect($hostname, $username, $password, $database);

// Check connection
if (!$conn) {	
    die("Connection failed: " . mysqli_connect_error());
}

echo "Database connection is ok";

// Check if POST data is received from Arduino
if (isset($_POST["smokeValue"])) {
    // Sanitize the input to prevent SQL injection
    $smokeValue = mysqli_real_escape_string($conn, $_POST["smokeValue"]);
    
    // Create SQL query to insert data into database
    $sql = "INSERT INTO mq135 (airquality) VALUES ('$smokeValue')";
    
    // Execute SQL query
    if (mysqli_query($conn, $sql)) {
        echo "New record created successfully";
    } else {
        echo "Error: " . $sql . "<br>" . mysqli_error($conn);
    }
} else {
    echo "No data received from Arduino.";
}

// Close database connection
mysqli_close($conn);
?>
