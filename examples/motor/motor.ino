#include "AITINKR_AIOT_V2.h"

// Instantiate the motor class for robot mobility
motor motor;

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);
    
    // Initialize the motors
    motor.init();

    // Set a default speed for the motors (0-255)
    motor.setSpeed(200);
}

void loop() {
    // Move forward
    Serial.println("Moving forward...");
    motor.forward();
    delay(3000);  // Move forward for 3 seconds

    // Turn left
    Serial.println("Turning left...");
    motor.left();
    delay(1000);  // Turn left for 1 second

    // Move forward again
    Serial.println("Moving forward...");
    motor.forward();
    delay(3000);  // Move forward for another 3 seconds

    // Turn right
    Serial.println("Turning right...");
    motor.right();
    delay(1000);  // Turn right for 1 second

    // Move backward
    Serial.println("Moving backward...");
    motor.backward();
    delay(3000);  // Move backward for 3 seconds

    // Stop the robot
    Serial.println("Stopping...");
    motor.stop();
    delay(2000);  // Stop for 2 seconds

    // Repeat the loop
}
