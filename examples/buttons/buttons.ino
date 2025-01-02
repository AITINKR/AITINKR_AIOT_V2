
#include <AITINKR_AIOT_V2.h>

// Create an instance of OnBoardButtons
OnBoardButtons buttons;

// Variable to store the current button state
Button currentButton = NO_BUTTON;

void setup() {
  Serial.begin(9600);

  // Initialize the button system
  buttons.init();

  // Configure debounce time
  buttons.setDebounceTime(200);

  // Enable or disable ignoring repeated button presses
  buttons.setIgnoreSameButton(true);

  Serial.println("OnBoardButtons example initialized.");
}

void loop() {
  // Read the button state
  currentButton = buttons.readButton();

  // If a button is pressed, print its state
  if (currentButton != NO_BUTTON) {
    Serial.print("Button pressed: ");
    switch (currentButton) {
      case LEFT_BUTTON:
        Serial.println("Left");
        break;
      case MIDDLE_BUTTON:
        Serial.println("Middle");
        break;
      case RIGHT_BUTTON:
        Serial.println("Right");
        break;
      case TOP_BUTTON:
        Serial.println("Top");
        break;
      case BOTTOM_BUTTON:
        Serial.println("Bottom");
        break;
      default:
        Serial.println("Unknown");
        break;
    }
  }

  // Add a short delay for better Serial Monitor readability
  delay(50);
}
