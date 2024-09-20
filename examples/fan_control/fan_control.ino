#include "AITINKR_AIOT_V2.h"

// Instantiate the SingleChannelControl class for the fan
SingleChannelControl fanControl;

// Variables to control the fan speed
int fanSpeed = 0;       // Initial fan speed (PWM value)
bool fanOn = false;     // Fan on/off state

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);
    
    // Initialize fan control for the RIGHT channel (you can change to LEFT if needed)
    fanControl.init(RIGHT);

    // Start with fan off
    fanControl.stop();
    Serial.println("Fan OFF (Stopped)");
}

void loop() {
    // Check if the fan is ON
    if (fanOn) {
        // Gradually increase the fan speed
        for (fanSpeed = 0; fanSpeed <= 255; fanSpeed += 5) {
            fanControl.setSpeed(fanSpeed);
            fanControl.forward();
            Serial.print("Fan ON - Speed: ");
            Serial.println(fanSpeed);
            delay(200);  // Wait for 200 ms before increasing the speed
        }
        
        // Keep the fan at maximum speed for a while
        Serial.println("Fan running at maximum speed");
        delay(5000);  // Keep it at max speed for 5 seconds

        // Turn off the fan after it reaches maximum speed
        fanControl.stop();
        Serial.println("Fan OFF (Stopped)");
        delay(3000);  // Fan off for 3 seconds

        // Set fanOn to false to stop increasing speed in the next loop
        fanOn = false;
    } else {
        // Fan is off, turn it on for the next cycle
        Serial.println("Turning fan ON...");
        delay(2000);  // Wait for 2 seconds before turning on
        fanOn = true;
    }
}
