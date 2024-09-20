#include "AITINKR_AIOT_V2.h"

// Instantiate the OnBoardButtons class
OnBoardButtons buttonReader;

void setup() {
    // Initialize serial communication for debugging
    Serial.begin(115200);
    
    // Initialize the on-board button control
    buttonReader.init();
}

void loop() {
    // Read which button is pressed
    Button pressedButton = buttonReader.readButton();

    switch (pressedButton) {
        case BOTTOM_BUTTON:
            Serial.println("BOTTOM BUTTON PRESSED");
            break;
        case LEFT_BUTTON:
            Serial.println("LEFT BUTTON PRESSED");
            break;
        case RIGHT_BUTTON:
            Serial.println("RIGHT BUTTON PRESSED");
            break;
        case MIDDLE_BUTTON:
            Serial.println("MIDDLE BUTTON PRESSED");
            break;
        case TOP_BUTTON:
            Serial.println("TOP BUTTON PRESSED");
            break;
        case NO_BUTTON:
        default:
            Serial.println("No button pressed");
            break;
    }

    delay(100);  // Small delay between reads
}
