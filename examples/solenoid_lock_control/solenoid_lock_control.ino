#include "AITINKR_AIOT_V2.h"

// Instantiate the DigitalControl class for the solenoid lock
DigitalControl solenoidLock;

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);
    
    // Initialize digital control for the LEFT channel (you can change to RIGHT if needed)
    solenoidLock.init(LEFT);
}

void loop() {
    // Unlock the solenoid lock (turn it ON)
    solenoidLock.on();
    Serial.println("Solenoid Lock ON (Unlocked)");
    delay(5000); // Keep it unlocked for 5 seconds
    
    // Lock the solenoid lock (turn it OFF)
    solenoidLock.off();
    Serial.println("Solenoid Lock OFF (Locked)");
    delay(5000); // Keep it locked for 5 seconds
}
