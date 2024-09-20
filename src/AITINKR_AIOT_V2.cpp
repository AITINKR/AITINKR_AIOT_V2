#include "AITINKR_AIOT_V2.h"

/**
 * @brief Initialize the motor GPIO pins and set default states.
 */
void motor::init() {
    pinMode(LEFT_A, OUTPUT);
    pinMode(LEFT_B, OUTPUT);
    pinMode(RIGHT_A, OUTPUT);
    pinMode(RIGHT_B, OUTPUT);
    stop();          // Ensure both motors are stopped initially
    setSpeed(200);   // Set a default speed for the motors
}

/**
 * @brief Set the speed of the motors.
 * 
 * @param sp The speed value (0 to 255).
 */
void motor::setSpeed(int sp) {
    speed = sp;
}

/**
 * @brief Set an offset value to fine-tune motor movement.
 * 
 * @param of The offset value.
 */
void motor::setOffset(int of) {
    offset = of;
}

/**
 * @brief Move both motors forward.
 */
void motor::forward() {
    analogWrite(LEFT_A, constrain(speed + offset, 0, 255));    // Left motor forward
    analogWrite(LEFT_B, 0);                                    // Stop reverse direction on Left motor
    analogWrite(RIGHT_A, constrain(speed - offset, 0, 255));   // Right motor forward
    analogWrite(RIGHT_B, 0);                                   // Stop reverse direction on Right motor
}
// Move both motors forward (old method alias)
void motor::forword() {
    forward();  // Call the new method, forward()
}
/**
 * @brief Move both motors backward.
 */
void motor::backward() {
    analogWrite(LEFT_A, 0);                                     // Stop forward direction on Left motor
    analogWrite(LEFT_B, constrain(speed + offset, 0, 255));    // Left motor backward
    analogWrite(RIGHT_A, 0);                                    // Stop forward direction on Right motor
    analogWrite(RIGHT_B, constrain(speed - offset, 0, 255));   // Right motor backward
}
// Move both motors backward (old method alias)
void motor::backword() {
    backward();  // Call the new method, backward()
}
/**
 * @brief Turn right by moving left motor forward and right motor backward.
 */
void motor::right() {
    analogWrite(LEFT_A, constrain(speed + offset, 0, 255));    // Left motor forward
    analogWrite(LEFT_B, 0);                                    // Stop reverse direction on Left motor
    analogWrite(RIGHT_A, 0);                                   // Stop forward direction on Right motor
    analogWrite(RIGHT_B, constrain(speed - offset, 0, 255));   // Right motor backward
}

/**
 * @brief Turn left by moving right motor forward and left motor backward.
 */
void motor::left() {
    analogWrite(LEFT_A, 0);                                     // Stop forward direction on Left motor
    analogWrite(LEFT_B, constrain(speed + offset, 0, 255));    // Left motor backward
    analogWrite(RIGHT_A, constrain(speed - offset, 0, 255));   // Right motor forward
    analogWrite(RIGHT_B, 0);                                    // Stop reverse direction on Right motor
}

/**
 * @brief Stop both motors.
 */
void motor::stop() {
    analogWrite(LEFT_A, 0);    // Stop Left motor forward direction
    analogWrite(LEFT_B, 0);    // Stop Left motor backward direction
    analogWrite(RIGHT_A, 0);   // Stop Right motor forward direction
    analogWrite(RIGHT_B, 0);   // Stop Right motor backward direction
}

// DigitalControl class methods

/**
 * @brief Initialize the GPIO pins for digital control based on the selected channel.
 * 
 * @param channel Enum value (LEFT or RIGHT) to select the channel.
 */
void DigitalControl::init(Channel channel) {
    if (channel == LEFT) {
        controlPin = LEFT_A;
    } else if (channel == RIGHT) {
        controlPin = RIGHT_A;
    } else {
        Serial.println("ERROR: Invalid channel. Use LEFT or RIGHT.");
        return;
    }

    pinMode(controlPin, OUTPUT);  // Set the selected control pin as output
    off();                        // Set initial state to off
}

/**
 * @brief Turn the selected channel on (HIGH).
 */
void DigitalControl::on() {
    digitalWrite(controlPin, HIGH);  // Turn the device on
}

/**
 * @brief Turn the selected channel off (LOW).
 */
void DigitalControl::off() {
    digitalWrite(controlPin, LOW);   // Turn the device off
}

// SingleChannelControl class methods

/**
 * @brief Initialize the motor control pins for PWM and direction control based on the selected channel.
 * 
 * @param channel Enum value (LEFT or RIGHT) to select the motor.
 */
void SingleChannelControl::init(Channel channel) {
    if (channel == LEFT) {
        motorPinA = LEFT_A;
        motorPinB = LEFT_B;
    } else if (channel == RIGHT) {
        motorPinA = RIGHT_A;
        motorPinB = RIGHT_B;
    } else {
        Serial.println("ERROR: Invalid channel. Use LEFT or RIGHT.");
        return;
    }

    pinMode(motorPinA, OUTPUT);
    pinMode(motorPinB, OUTPUT);
    stop();  // Ensure the motor is stopped initially
}

/**
 * @brief Set the motor speed using PWM (0-255).
 * 
 * @param pwm The PWM value to set the motor speed.
 */
void SingleChannelControl::setSpeed(int pwm) {
    pwmValue = constrain(pwm, 0, 255);  // Store the PWM value within the valid range
}

/**
 * @brief Move the motor forward with the current PWM speed.
 */
void SingleChannelControl::forward() {
    analogWrite(motorPinA, pwmValue);  // Apply PWM to move forward
    digitalWrite(motorPinB, LOW);      // Ensure backward direction is off
}

/**
 * @brief Move the motor backward with the current PWM speed.
 */
void SingleChannelControl::backward() {
    analogWrite(motorPinB, pwmValue);  // Apply PWM to move backward
    digitalWrite(motorPinA, LOW);      // Ensure forward direction is off
}

/**
 * @brief Stop the motor.
 */
void SingleChannelControl::stop() {
    analogWrite(motorPinA, 0);  // Stop forward direction
    analogWrite(motorPinB, 0);  // Stop backward direction
}

// OnBoardButtons class methods

/**
 * @brief Initialize the button reading from fixed analog pin 5.
 */
void OnBoardButtons::init() {
    analogReadResolution(12);  // Set analog read resolution to 12 bits
}

/**
 * @brief Read the button press and return the corresponding Button enum.
 * 
 * @return Button The button state.
 */
Button OnBoardButtons::readButton() {
    buttonData = analogRead(analogPin);  // Read the analog input from pin 5

    // Determine the value of 'button' based on the value of 'buttonData'
    if (buttonData > 4000) {
        return NO_BUTTON;          // No button pressed
    } else if (buttonData > 2600) {
        return LEFT_BUTTON;        // Left button pressed
    } else if (buttonData > 2200) {
        return MIDDLE_BUTTON ;       // Middle button pressed
    } else if (buttonData > 1660) {
        return RIGHT_BUTTON;      // Right button pressed
    } else if (buttonData > 1200) {
        return TOP_BUTTON;         // Top button pressed
    } else {
        return BOTTOM_BUTTON;      // Bottom button pressed
    }
}
