#ifndef AITINKR_AIOT_V2_H
#define AITINKR_AIOT_V2_H

#include "Arduino.h"
#include "AITINKRMQTT.h"
#include "AIOTS3Servo.h"
#include "AIOTCamera.h"
#include <AITINKR_JSON_FIELDS.h>
#include <Wire.h>

// Define the GPIO pins for controlling the left and right motors
#define LEFT_A 39   // Left motor, direction A
#define LEFT_B 40   // Left motor, direction B
#define RIGHT_A 41  // Right motor, direction A
#define RIGHT_B 42  // Right motor, direction B

// Enum to define LEFT and RIGHT channels
enum Channel { LEFT, RIGHT };

// Enum to define button states
enum Button {
    NO_BUTTON,        // No button is pressed
    BOTTOM_BUTTON,    // Bottom button pressed
    LEFT_BUTTON,      // Left button pressed
    RIGHT_BUTTON,     // Right button pressed
    MIDDLE_BUTTON,    // Middle button pressed
    TOP_BUTTON        // Top button pressed
};

// Default I2C pins for AITINKR AIOT V2
#define AIOT_V2_SDA_PIN 21
#define AIOT_V2_SCL_PIN 14

/**
 * @brief Initializes the I2C interface.
 * @param sda SDA pin (default is AIOT_V2_SDA_PIN)
 * @param scl SCL pin (default is AIOT_V2_SCL_PIN)
 */
void initI2C(uint8_t sda = AIOT_V2_SDA_PIN, uint8_t scl = AIOT_V2_SCL_PIN);

/**
 * @brief Scans for I2C devices and prints their addresses.
 * @return True if devices are found, false otherwise.
 */
bool scanI2C();

/**
 * @brief motor class: Provides methods to control two motors for mobility (forward, backward, left, right).
 */
class motor {
public:
    void init();                    // Initialize the motor control pins
    void setSpeed(int sp);          // Set motor speed
    void setOffset(int of);         // Set motor offset for fine-tuning movement
    void forward();                 // Move both motors forward (new method)
    void forword();                 // Move both motors forward (old method)
    void backward();                // Move both motors backward (new method)
    void backword();                // Move both motors backward (old method)
    void right();                   // Turn right
    void left();                    // Turn left
    void stop();                    // Stop both motors

private:
    int speed = 200;                // Default motor speed
    int offset = 0;                 // Offset value to adjust motor control
};


/**
 * @brief DigitalControl class: Provides methods to control a device using simple on/off signals.
 */
class DigitalControl {
public:
    void init(Channel channel);   // Initialize the pins for digital control with enum channel selection (LEFT or RIGHT)
    void on();                    // Turn the selected channel on (HIGH)
    void off();                   // Turn the selected channel off (LOW)

private:
    int controlPin = -1;          // GPIO pin for the selected channel
};

/**
 * @brief SingleChannelControl class: Provides methods to control a motor using PWM for speed and direction control.
 */
class SingleChannelControl {
public:
    void init(Channel channel);   // Initialize the motor control pins with enum channel selection (LEFT or RIGHT)
    void setSpeed(int pwm);       // Set the speed of the motor using PWM
    void forward();               // Set the motor direction to forward
    void backward();              // Set the motor direction to backward
    void stop();                  // Stop the motor

private:
    int motorPinA = -1;           // GPIO pin for the motor direction A (forward)
    int motorPinB = -1;           // GPIO pin for the motor direction B (backward)
    int pwmValue = 255;           // Default PWM value (max speed)
};

/**
 * @brief OnBoardButtons class: Reads the button press state from a fixed analog pin (pin 5) and determines which button is pressed.
 */
class OnBoardButtons {
private:
    unsigned long debounceTime = 200;        // Default debounce time in milliseconds
    unsigned long lastDebounceTime = 0;     // Tracks the last debounce time
    Button lastButtonState = NO_BUTTON;     // Tracks the last button state
    int analogPin = 5;                      // Default analog pin for buttons
    int buttonData = 0;                     // Stores the analog read data
    bool ignoreSameButton = false;          // Flag to ignore repeated presses of the same button

public:
    void init();
    void setDebounceTime(unsigned long debounceTime);
    void setIgnoreSameButton(bool enable);
    bool isIgnoreSameButtonEnabled();
    Button readButton();
};

#endif  // AITINKR_AIOT_V2_H
