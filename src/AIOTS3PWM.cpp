#include <AIOTS3PWM.h>
#include "esp32-hal-ledc.h"

// initialize the class variable ServoCount
int AIOTS3PWM::PWMCount = -1;              // the total number of attached servos
bool  AIOTS3PWM::explicateAllocationMode=false;
AIOTS3PWM * AIOTS3PWM::ChannelUsed[NUM_PWM]; // used to track whether a channel is in service
long AIOTS3PWM::timerFreqSet[4] = { -1, -1, -1, -1 };
int AIOTS3PWM::timerCount[4] = { 0, 0, 0, 0 };
// The ChannelUsed array elements are 0 if never used, 1 if in use, and -1 if used and disposed
// (i.e., available for reuse)
/**
 * allocateTimer
 * @param a timer number 0-3 indicating which timer to allocate in this library
 * Switch to explicate allocation mode
 *
 */
void AIOTS3PWM::allocateTimer(int timerNumber){
	if(timerNumber<0 || timerNumber>3)
		return;
	if(AIOTS3PWM::explicateAllocationMode==false){
		AIOTS3PWM::explicateAllocationMode=true;
		for(int i=0;i<4;i++)
			AIOTS3PWM::timerCount[i]=4;// deallocate all timers to start mode
	}
	AIOTS3PWM::timerCount[timerNumber]=0;
}

AIOTS3PWM::AIOTS3PWM() {
	resolutionBits = 8;
	pwmChannel = -1;
	pin = -1;
	myFreq = -1;
	if (PWMCount == -1) {
		for (int i = 0; i < NUM_PWM; i++)
			ChannelUsed[i] = NULL; // load invalid data into the storage array of pin mapping
		PWMCount = PWM_BASE_INDEX; // 0th channel does not work with the PWM system
	}
}

AIOTS3PWM::~AIOTS3PWM() {
	if (attached()) {
		ledcDetachPin(pin);
	}
	deallocate();
}

double AIOTS3PWM::_ledcSetupTimerFreq(uint8_t chan, double freq,
		uint8_t bit_num) {
	return ledcSetup(chan, freq, bit_num);

}

int AIOTS3PWM::timerAndIndexToChannel(int timerNum, int index) {
	int localIndex = 0;
	for (int j = 0; j < NUM_PWM; j++) {
		if (((j / 2) % 4) == timerNum) {
			if (localIndex == index) {
				return j;
			}
			localIndex++;
		}
	}
	return -1;
}
int AIOTS3PWM::allocatenext(double freq) {
	long freqlocal = (long) freq;
	if (pwmChannel < 0) {
		for (int i = 0; i < 4; i++) {
			bool freqAllocated = ((timerFreqSet[i] == freqlocal)
					|| (timerFreqSet[i] == -1));
			if (freqAllocated && timerCount[i] < 4) {
				if (timerFreqSet[i] == -1) {
					//Serial.println("Starting timer "+String(i)+" at freq "+String(freq));
					timerFreqSet[i] = freqlocal;
				}
				//Serial.println("Free channel timer "+String(i)+" at freq "+String(freq)+" remaining "+String(4-timerCount[i]));

				timerNum = i;
				for (int index=0; index<4; ++index)
				{
					int myTimerNumber = timerAndIndexToChannel(timerNum,index);
					if ((myTimerNumber >= 0)  && (!ChannelUsed[myTimerNumber]))
					{
						pwmChannel = myTimerNumber;
// 						Serial.println(
// 							"PWM on ledc channel #" + String(pwmChannel)
// 									+ " using 'timer " + String(timerNum)
// 									+ "' to freq " + String(freq) + "Hz");
						ChannelUsed[pwmChannel] = this;
						timerCount[timerNum]++;
						PWMCount++;
						myFreq = freq;
						return pwmChannel;
					}
				}
			} else {
//				if(timerFreqSet[i]>0)
//					Serial.println("Timer freq mismatch target="+String(freq)+" on timer "+String(i)+" was "+String(timerFreqSet[i]));
//				else
//					Serial.println("Timer out of channels target="+String(freq)+" on timer "+String(i)+" was "+String(timerCount[i]));
			}
		}
	} else {
		return pwmChannel;
	}
	Serial.println(
			"ERROR All PWM timers allocated! Can't accomodate " + String(freq)
					+ "Hz\r\nHalting...");
	while (1)
		;
}
void AIOTS3PWM::deallocate() {
	if (pwmChannel < 0)
		return;
// 	Serial.println("PWM deallocating LEDc #" + String(pwmChannel));
	timerCount[getTimer()]--;
	if (timerCount[getTimer()] == 0) {
		timerFreqSet[getTimer()] = -1; // last pwn closed out
	}
	timerNum = -1;
	attachedState = false;
	ChannelUsed[pwmChannel] = NULL;
	pwmChannel = -1;
	PWMCount--;

}

int AIOTS3PWM::getChannel() {
	if (pwmChannel < 0) {
		Serial.println("FAIL! must setup() before using get channel!");
	}
	return pwmChannel;
}

double AIOTS3PWM::setup(double freq, uint8_t resolution_bits) {
	checkFrequencyForSideEffects(freq);

	resolutionBits = resolution_bits;
	if (attached()) {
		ledcDetachPin(pin);
		double val = ledcSetup(getChannel(), freq, resolution_bits);
		attachPin(pin);
		return val;
	}
	return ledcSetup(getChannel(), freq, resolution_bits);
}
double AIOTS3PWM::getDutyScaled() {
	return mapf((double) myDuty, 0, (double) ((1 << resolutionBits) - 1), 0.0,
			1.0);
}
void AIOTS3PWM::writeScaled(double duty) {
	write(mapf(duty, 0.0, 1.0, 0, (double) ((1 << resolutionBits) - 1)));
}
void AIOTS3PWM::write(uint32_t duty) {
	myDuty = duty;
	ledcWrite(getChannel(), duty);
}
void AIOTS3PWM::adjustFrequencyLocal(double freq, double dutyScaled) {
	timerFreqSet[getTimer()] = (long) freq;
	myFreq = freq;
	if (attached()) {
		ledcDetachPin(pin);
		// Remove the PWM during frequency adjust
		_ledcSetupTimerFreq(getChannel(), freq, resolutionBits);
		writeScaled(dutyScaled);
		ledcAttachPin(pin, getChannel()); // re-attach the pin after frequency adjust
	} else {
		_ledcSetupTimerFreq(getChannel(), freq, resolutionBits);
		writeScaled(dutyScaled);
	}
}
void AIOTS3PWM::adjustFrequency(double freq, double dutyScaled) {
	if(dutyScaled<0)
		dutyScaled=getDutyScaled();
	writeScaled(dutyScaled);
	for (int i = 0; i < timerCount[getTimer()]; i++) {
		int pwm = timerAndIndexToChannel(getTimer(), i);
		if (ChannelUsed[pwm] != NULL) {
			if (ChannelUsed[pwm]->myFreq != freq) {
				ChannelUsed[pwm]->adjustFrequencyLocal(freq,
						ChannelUsed[pwm]->getDutyScaled());
			}
		}
	}
}
double AIOTS3PWM::writeTone(double freq) {
	for (int i = 0; i < timerCount[getTimer()]; i++) {
		int pwm = timerAndIndexToChannel(getTimer(), i);
		if (ChannelUsed[pwm] != NULL) {
			if (ChannelUsed[pwm]->myFreq != freq) {
				ChannelUsed[pwm]->adjustFrequencyLocal(freq,
						ChannelUsed[pwm]->getDutyScaled());
			}
			write(1 << (resolutionBits-1)); // writeScaled(0.5);
		}
	}

	return 0;
}
double AIOTS3PWM::writeNote(note_t note, uint8_t octave) {
	const uint16_t noteFrequencyBase[12] = {
			//   C        C#       D        Eb       E        F       F#        G       G#        A       Bb        B
			4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459,
			7902 };

	if (octave > 8 || note >= NOTE_MAX) {
		return 0;
	}
	double noteFreq = (double) noteFrequencyBase[note]
			/ (double) (1 << (8 - octave));
	return writeTone(noteFreq);
}
uint32_t AIOTS3PWM::read() {
	return ledcRead(getChannel());
}
double AIOTS3PWM::readFreq() {
	return myFreq;
}
void AIOTS3PWM::attach(int p) {
	pin = p;
	attachedState = true;
}
void AIOTS3PWM::attachPin(uint8_t pin) {

	if (hasPwm(pin)) {
		attach(pin);
		ledcAttachPin(pin, getChannel());
	} else {
		Serial.println(
				"ERROR PWM channel unavailable on pin requested! " + String(pin)
#if defined(CONFIG_IDF_TARGET_ESP32S2)
						+ "\r\nPWM available on: 1-21,26,33-42"
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
						+ "\r\nPWM available on: 1-21,35-45,47-48"
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
						+ "\r\nPWM available on: 1-10,18-21"
#else
						+ "\r\nPWM available on: 2,4,5,12-19,21-23,25-27,32-33"
#endif

// Possible PWM GPIO pins on the ESP32-S3: 0(used by on-board button),1-21,35-45,47,48(used by on-board LED)
// Possible PWM GPIO pins on the ESP32-C3: 0(used by on-board button),1-7,8(used by on-board LED),9-10,18-21
		);
		return;
	}
	//Serial.print(" on pin "+String(pin));
}
void AIOTS3PWM::attachPin(uint8_t pin, double freq, uint8_t resolution_bits) {

	if (hasPwm(pin))
		setup(freq, resolution_bits);
	attachPin(pin);
}
void AIOTS3PWM::detachPin(int pin) {
	ledcDetachPin(pin);
	deallocate();
}
/* Side effects of frequency changes happen because of shared timers
 *
 * LEDC Chan to Group/Channel/Timer Mapping
 ** ledc: 0  => Group: 0, Channel: 0, Timer: 0
 ** ledc: 1  => Group: 0, Channel: 1, Timer: 0
 ** ledc: 2  => Group: 0, Channel: 2, Timer: 1
 ** ledc: 3  => Group: 0, Channel: 3, Timer: 1
 ** ledc: 4  => Group: 0, Channel: 4, Timer: 2
 ** ledc: 5  => Group: 0, Channel: 5, Timer: 2
 ** ledc: 6  => Group: 0, Channel: 6, Timer: 3
 ** ledc: 7  => Group: 0, Channel: 7, Timer: 3
 ** ledc: 8  => Group: 1, Channel: 0, Timer: 0
 ** ledc: 9  => Group: 1, Channel: 1, Timer: 0
 ** ledc: 10 => Group: 1, Channel: 2, Timer: 1
 ** ledc: 11 => Group: 1, Channel: 3, Timer: 1
 ** ledc: 12 => Group: 1, Channel: 4, Timer: 2
 ** ledc: 13 => Group: 1, Channel: 5, Timer: 2
 ** ledc: 14 => Group: 1, Channel: 6, Timer: 3
 ** ledc: 15 => Group: 1, Channel: 7, Timer: 3
 */

bool AIOTS3PWM::checkFrequencyForSideEffects(double freq) {

	allocatenext(freq);
	for (int i = 0; i < timerCount[getTimer()]; i++) {
		int pwm = timerAndIndexToChannel(getTimer(), i);

		if (pwm == pwmChannel)
			continue;
		if (ChannelUsed[pwm] != NULL)
			if (ChannelUsed[pwm]->getTimer() == getTimer()) {
				double diff = abs(ChannelUsed[pwm]->myFreq - freq);
				if (abs(diff) > 0.1) {
					Serial.println(
							"\tWARNING PWM channel " + String(pwmChannel)
									+ " shares a timer with channel "
									+ String(pwm) + "\n"
											"\tchanging the frequency to "
									+ String(freq)
									+ " Hz will ALSO change channel "
									+ String(pwm)
									+ " \n\tfrom its previous frequency of "
									+ String(ChannelUsed[pwm]->myFreq) + " Hz\n"
											" ");
					ChannelUsed[pwm]->myFreq = freq;
				}
			}
	}
	return true;
}

AIOTS3PWM* pwmFactory(int pin) {
	for (int i = 0; i < NUM_PWM; i++)
		if (AIOTS3PWM::ChannelUsed[i] != NULL) {
			if (AIOTS3PWM::ChannelUsed[i]->getPin() == pin)
				return AIOTS3PWM::ChannelUsed[i];
		}
	return NULL;
}
