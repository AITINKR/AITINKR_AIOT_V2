#ifndef AITINKR_AIOT_V2_H
#define AITINKR_AIOT_V2_H

#include "Arduino.h"
#include "AIOTS3Servo.h"

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
public:
    void init();                  // Initialize the button reading from pin 5
    Button readButton();          // Read the button press and return the corresponding Button enum

private:
    const int analogPin = 5;      // Fixed analog pin (5) for reading button states
    int buttonData;               // Variable to store the analog reading
};

#endif  // AITINKR_AIOT_V2_H
