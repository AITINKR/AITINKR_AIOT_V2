#include <AIOTS3PWM.h>
#include "esp32-hal-ledc.h"

 
int AIOTS3PWM::PWMCount = -1;             
bool  AIOTS3PWM::explicateAllocationMode=false;
AIOTS3PWM * AIOTS3PWM::ChannelUsed[NUM_PWM];  
long AIOTS3PWM::timerFreqSet[4] = { -1, -1, -1, -1 };
int AIOTS3PWM::timerCount[4] = { 0, 0, 0, 0 };

static const char* TAG = "AIOTS3PWM";

 
void AIOTS3PWM::allocateTimer(int timerNumber){
	if(timerNumber<0 || timerNumber>3)
		return;
	if(AIOTS3PWM::explicateAllocationMode==false){
		AIOTS3PWM::explicateAllocationMode=true;
		for(int i=0;i<4;i++)
			AIOTS3PWM::timerCount[i]=4; 
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
			ChannelUsed[i] = NULL;  
		PWMCount = PWM_BASE_INDEX;  
	}
}

AIOTS3PWM::~AIOTS3PWM() {
	if (attached()) {
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
		ledcDetach(pin);
#else
		ledcDetachPin(pin);
#endif
#else
		ledcDetachPin(pin);
#endif
	}
	deallocate();
}

double AIOTS3PWM::_ledcSetupTimerFreq(uint8_t pin, double freq,
		uint8_t bit_num) {

#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
	return ledcAttach(pin, freq, bit_num);
#else
	return ledcSetup(pin, freq, bit_num);
#endif
#else
	return ledcSetup(pin, freq, bit_num);
#endif

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
	ESP_LOGE(TAG, 
			"ERROR All PWM timers allocated! Can't accomodate %d Hz\r\nHalting...", freq);
	while (1)
		;
}
void AIOTS3PWM::deallocate() {
	if (pwmChannel < 0)
		return;
	ESP_LOGE(TAG, "PWM deallocating LEDc #%d",pwmChannel);
	timerCount[getTimer()]--;
	if (timerCount[getTimer()] == 0) {
		timerFreqSet[getTimer()] = -1; 
	}
	timerNum = -1;
	attachedState = false;
	ChannelUsed[pwmChannel] = NULL;
	pwmChannel = -1;
	PWMCount--;

}

int AIOTS3PWM::getChannel() {
	if (pwmChannel < 0) {
		ESP_LOGE(TAG, "FAIL! must setup() before using get channel!");
	}
	return pwmChannel;
}

double AIOTS3PWM::setup(double freq, uint8_t resolution_bits) {
	checkFrequencyForSideEffects(freq);

	resolutionBits = resolution_bits;
	if (attached()) {
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
		ledcDetach(pin);
		double val = ledcAttach(getPin(), freq, resolution_bits);
#else
		ledcDetachPin(pin);
		double val = ledcSetup(getChannel(), freq, resolution_bits);
#endif
#else
		ledcDetachPin(pin);
		double val = ledcSetup(getChannel(), freq, resolution_bits);
#endif

		attachPin(pin);
		return val;
	}
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
	return ledcAttach(getPin(), freq, resolution_bits);
#else
	return ledcSetup(getChannel(), freq, resolution_bits);
#endif
#else
	return ledcSetup(getChannel(), freq, resolution_bits);
#endif
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
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
	ledcWrite(getPin(), duty);
#else
	ledcWrite(getChannel(), duty);
#endif
#else
	ledcWrite(getChannel(), duty);
#endif
}
void AIOTS3PWM::adjustFrequencyLocal(double freq, double dutyScaled) {
	timerFreqSet[getTimer()] = (long) freq;
	myFreq = freq;
	if (attached()) {
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
		ledcDetach(pin);
 		_ledcSetupTimerFreq(getPin(), freq, resolutionBits);
		writeScaled(dutyScaled);
		ledcAttach(getPin(), freq, resolutionBits);  
#else
		ledcDetachPin(pin);
 		_ledcSetupTimerFreq(getChannel(), freq, resolutionBits);
		writeScaled(dutyScaled);
		ledcAttachPin(pin, getChannel()); 
#endif
#else
		ledcDetachPin(pin);
 		_ledcSetupTimerFreq(getChannel(), freq, resolutionBits);
		writeScaled(dutyScaled);
		ledcAttachPin(pin, getChannel());  
#endif

	} else {
		_ledcSetupTimerFreq(getPin(), freq, resolutionBits);
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
			write(1 << (resolutionBits-1));  
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
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
	return ledcRead(getPin());
#else
	return ledcRead(getChannel());
#endif
#else
	return ledcRead(getChannel());
#endif

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
		bool success=true;
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
		success=ledcAttach(pin, readFreq(), resolutionBits);
#else
		ledcAttachPin(pin, getChannel());
#endif
#else
		ledcAttachPin(pin, getChannel());
#endif
		if(success)
			return;
		ESP_LOGE(TAG, "ERROR PWM channel failed to configure on!",pin);
		return;
	}
		
#if  defined(CONFIG_IDF_TARGET_ESP32S3)
			    ESP_LOGE(TAG, "This pin can not be a servo: %d Servo available on: 0,2,4,5,43,44,45,46,47", pin);

#else
				ESP_LOGE(TAG, "This pin can not be a servo: %d Servo available on: 0,2,4,5,43,44,45,46,47", pin);
#endif




}
void AIOTS3PWM::attachPin(uint8_t pin, double freq, uint8_t resolution_bits) {

	if (hasPwm(pin)){
		int ret=setup(freq, resolution_bits);
		ESP_LOGW(TAG, "Pin Setup %d with code %d",pin,ret);
	}
	else
		ESP_LOGE(TAG, "ERROR Pin Failed %d ",pin);
	attachPin(pin);
}
void AIOTS3PWM::detachPin(int pin) {
#ifdef ESP_ARDUINO_VERSION_MAJOR
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)

	ledcDetach(pin);
#else
	ledcDetachPin(pin);
#endif
#else
	ledcDetachPin(pin);
#endif
	deallocate();
}
 

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
					ESP_LOGW(TAG, 
							"\tWARNING PWM channel %d	\
							 shares a timer with channel %d\n	\
							\tchanging the frequency to %d		\
							Hz will ALSO change channel %d	\
							\n\tfrom its previous frequency of %d Hz\n "
								,pwmChannel, pwm, freq,pwm, ChannelUsed[pwm]->myFreq);
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
