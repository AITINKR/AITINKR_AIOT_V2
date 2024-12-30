#ifndef ESP32_Servo_h
#define ESP32_Servo_h

#include "AIOTS3PWM.h"

#define ENFORCE_PINS
#define DEFAULT_uS_LOW 544
#define DEFAULT_uS_HIGH 2400



#ifdef ARDUINO_ESP32C3_DEV
#define MINIMUM_TIMER_WIDTH 10
#define MAXIMUM_TIMER_WIDTH 14
#define DEFAULT_TIMER_WIDTH 10
#else
#define MINIMUM_TIMER_WIDTH 10
#define MAXIMUM_TIMER_WIDTH 20
#define DEFAULT_TIMER_WIDTH 10
#endif
#define DEFAULT_TIMER_WIDTH_TICKS 1024

#define ESP32_Servo_VERSION           1    
#define MIN_PULSE_WIDTH       500      
#define MAX_PULSE_WIDTH      2500     
#define DEFAULT_PULSE_WIDTH  1500     
#define DEFAULT_PULSE_WIDTH_TICKS 4825
//#define REFRESH_CPS            50
#define REFRESH_USEC         20000

#define MAX_SERVOS              16    


class Servo {

public:
	Servo();
	 
	int attach(int pin);  
	int attach(int pin, int min, int max);  
	void detach();
	void write(int value);  
	void writeMicroseconds(int value);     
	void writeTicks(int value);   
	void release();
	int read();  
	int readMicroseconds();  
	int readTicks();  
	bool attached();  

	 
	void setTimerWidth(int value);     
	int readTimerWidth();              
	void setPeriodHertz(int hertz){
		REFRESH_CPS=hertz;
		setTimerWidth(this->timer_width);
	}
private:
	int usToTicks(int usec);
	int ticksToUs(int ticks);
 

	int min = DEFAULT_uS_LOW;           
	int max = DEFAULT_uS_HIGH;           
	int pinNumber = 0;                       
	int timer_width = DEFAULT_TIMER_WIDTH;  
	int ticks = DEFAULT_PULSE_WIDTH_TICKS;  
	int timer_width_ticks = DEFAULT_TIMER_WIDTH_TICKS;  
	AIOTS3PWM * getPwm(); 
	AIOTS3PWM pwm;
	int REFRESH_CPS = 50;

};
#endif
