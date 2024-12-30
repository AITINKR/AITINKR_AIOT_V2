#ifndef LIBRARIES_AIOTS3Servo_SRC_AIOTS3PWM_H_
#define LIBRARIES_AIOTS3Servo_SRC_AIOTS3PWM_H_
#include "esp32-hal-ledc.h"
#if defined(ARDUINO)
	#include "Arduino.h"
#endif

#if defined(CONFIG_IDF_TARGET_ESP32C3)
#define NUM_PWM 6
#elif defined(CONFIG_IDF_TARGET_ESP32S2)   ||  defined(CONFIG_IDF_TARGET_ESP32S3)
#define NUM_PWM 8
#else 
#define NUM_PWM 16
#endif

#define PWM_BASE_INDEX 0
#define USABLE_ESP32_PWM (NUM_PWM-PWM_BASE_INDEX)
#include <cstdint>

class AIOTS3PWM {
private:

	void attach(int pin);
	int pwmChannel = 0;                          
	bool attachedState = false;
	int pin;
	uint8_t resolutionBits;
	double myFreq;
	int allocatenext(double freq);

	static double _ledcSetupTimerFreq(uint8_t pin, double freq,
			uint8_t bit_num);

	bool checkFrequencyForSideEffects(double freq);

	void adjustFrequencyLocal(double freq, double dutyScaled);
	static double mapf(double x, double in_min, double in_max, double out_min,
			double out_max) {
		if(x>in_max)
			return out_max;
		if(x<in_min)
			return out_min;
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	double setup(double freq, uint8_t resolution_bits=10);
 	void attachPin(uint8_t pin);
 	void deallocate();
public:
 	AIOTS3PWM();
	virtual ~AIOTS3PWM();


	void detachPin(int pin);
	void attachPin(uint8_t pin, double freq, uint8_t resolution_bits=10);
	bool attached() {
		return attachedState;
	}

 	void write(uint32_t duty);
 	void writeScaled(double duty);
 	double writeTone(double freq);
	double writeNote(note_t note, uint8_t octave);
	void adjustFrequency(double freq, double dutyScaled=-1);

 	uint32_t read();
	double readFreq();
	double getDutyScaled();

 	static int timerAndIndexToChannel(int timer, int index);
 
	static void allocateTimer(int timerNumber);
	static bool explicateAllocationMode;
	int getTimer() {
		return timerNum;
	}
	int timerNum = -1;
	uint32_t myDuty = 0;
	int getChannel();
	static int PWMCount;               
	static int timerCount[4];
	static AIOTS3PWM * ChannelUsed[NUM_PWM];  
	static long timerFreqSet[4];

 	int getPin() {
		return pin;
	}
	static bool hasPwm(int pin) {
#if defined(CONFIG_IDF_TARGET_ESP32S3)
		if ((pin >=0 && pin <= 21) || 
				(pin >= 35 && pin <= 45) || 
				(pin == 46) ||(pin == 47) || (pin == 48)) 

#else
		if ((pin >=0 && pin <= 21) || 
				(pin >= 35 && pin <= 45) || 
				(pin == 46) ||(pin == 47) || (pin == 48)) 
#endif
			return true;
		return false;
	}
	static int channelsRemaining() {
		return NUM_PWM - PWMCount;
	}
	static boolean DISABLE_DAC;


};

AIOTS3PWM* pwmFactory(int pin);

#endif /* LIBRARIES_AIOTS3Servo_SRC_AIOTS3PWM_H_ */
