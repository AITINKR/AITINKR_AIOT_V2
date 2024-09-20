 
#include <AITINKR_AIOT_V2.h> //Install 2.0.11 version of ESP32 board for servo examples

Servo myservo;  

int pos = 0;    
int servoPin = 45;


void setup() {
	// Allow allocation of all timers
	AIOTS3PWM::allocateTimer(0);
	AIOTS3PWM::allocateTimer(1);
	AIOTS3PWM::allocateTimer(2);
	AIOTS3PWM::allocateTimer(3);
	myservo.setPeriodHertz(50);    // standard 50 hz servo
	myservo.attach(servoPin, 1000, 2000); // attaches the servo on pin 18 to the servo object
	// using default min/max of 1000us and 2000us
	// different servos may require different min/max settings
	// for an accurate 0 to 180 sweep
}

void loop() {

	for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		myservo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
	for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
}
