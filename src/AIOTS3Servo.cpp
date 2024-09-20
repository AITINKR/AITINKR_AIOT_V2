#include <AIOTS3Servo.h>
#include "Arduino.h"

//
Servo::Servo()
{		// initialize this channel with plausible values, except pin # (we set pin # when attached)
	REFRESH_CPS = 50;
	this->ticks = DEFAULT_PULSE_WIDTH_TICKS;
	this->timer_width = DEFAULT_TIMER_WIDTH;
	this->pinNumber = -1;     // make it clear that we haven't attached a pin to this channel
	this->min = DEFAULT_uS_LOW;
	this->max = DEFAULT_uS_HIGH;
	this->timer_width_ticks = pow(2,this->timer_width);

}
AIOTS3PWM * Servo::getPwm(){

	return &pwm;
}

int Servo::attach(int pin)
{

    return (this->attach(pin, DEFAULT_uS_LOW, DEFAULT_uS_HIGH));
}

int Servo::attach(int pin, int min, int max)
{

#ifdef ENFORCE_PINS
        // ESP32 Recommend only the following pins 2,4,12-19,21-23,25-27,32-33
		// ESP32-S2 only the following pins 1-21,26,33-42
        if (pwm.hasPwm(pin))
        {
#endif

            // OK to proceed; first check for new/reuse
            if (this->pinNumber < 0) // we are attaching to a new or previously detached pin; we need to initialize/reinitialize
            {
                this->ticks = DEFAULT_PULSE_WIDTH_TICKS;
                this->timer_width = DEFAULT_TIMER_WIDTH;
                this->timer_width_ticks = pow(2,this->timer_width);
            }
            this->pinNumber = pin;
#ifdef ENFORCE_PINS
        }
        else
        {
#ifdef __XTENSA_esp32s3__
if(
#endif
        	Serial.println("This pin can not be a servo: "+String(pin)+
#if defined(CONFIG_IDF_TARGET_ESP32S2)
				"\r\nServo available on: 1-21,26,33-42"
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
			        "\r\nPWM available on: 1-21,35-45,47-48"
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
				"\r\nPWM available on: 1-10,18-21"
#else
				"\r\nServo available on: 2,4,5,12-19,21-23,25-27,32-33"
#endif
			);
            return 0;
        }
#endif


        // min/max checks 
        if (min < MIN_PULSE_WIDTH)          // ensure pulse width is valid
            min = MIN_PULSE_WIDTH;
        if (max > MAX_PULSE_WIDTH)
            max = MAX_PULSE_WIDTH;
        this->min = min;     //store this value in uS
        this->max = max;    //store this value in uS
        // Set up this channel
        // if you want anything other than default timer width, you must call setTimerWidth() before attach
        pwm.attachPin(this->pinNumber,REFRESH_CPS, this->timer_width );   // GPIO pin assigned to channel
        //Serial.println("Attaching servo : "+String(pin)+" on PWM "+String(pwm.getChannel()));
        return pwm.getChannel();
}

void Servo::detach()
{
    if (this->attached())
    {
        //keep track of detached servos channels so we can reuse them if needed
        pwm.detachPin(this->pinNumber);

        this->pinNumber = -1;
    }
}

void Servo::write(int value)
{
    // treat values less than MIN_PULSE_WIDTH (500) as angles in degrees (valid values in microseconds are handled as microseconds)
    if (value < MIN_PULSE_WIDTH)
    {
        if (value < 0)
            value = 0;
        else if (value > 180)
            value = 180;

        value = map(value, 0, 180, this->min, this->max);
    }
    this->writeMicroseconds(value);
}

void Servo::writeMicroseconds(int value)
{
    // calculate and store the values for the given channel
    if (this->attached())   // ensure channel is valid
    {
        if (value < this->min)          // ensure pulse width is valid
            value = this->min;
        else if (value > this->max)
            value = this->max;

        value = usToTicks(value);  // convert to ticks
        this->ticks = value;
        // do the actual write
        pwm.write( this->ticks);
    }
}

void Servo::release()
{
    if (this->attached())   // ensure channel is valid
        pwm.write(0);
}

int Servo::read() // return the value as degrees
{
    return (map(readMicroseconds()+1, this->min, this->max, 0, 180));
}

int Servo::readMicroseconds()
{
    int pulsewidthUsec;
    if (this->attached())
    { 
        pulsewidthUsec = ticksToUs(this->ticks);
    }
    else
    {
        pulsewidthUsec = 0;
    }

    return (pulsewidthUsec);
}

bool Servo::attached()
{
    return (pwm.attached());
}

void Servo::setTimerWidth(int value)
{
    // only allow values between 10 and 14 for ESP32-C3
    // only allow values between 16 and 20 for other ESP32
    if (value < MINIMUM_TIMER_WIDTH )
        value = MINIMUM_TIMER_WIDTH;
    else if (value > MAXIMUM_TIMER_WIDTH)
        value = MAXIMUM_TIMER_WIDTH;
        
    // Fix the current ticks value after timer width change
    // The user can reset the tick value with a write() or writeUs()
    int widthDifference = this->timer_width - value;
    // if positive multiply by diff; if neg, divide
    if (widthDifference > 0)
    {
        this->ticks = widthDifference * this->ticks;
    }
    else if (widthDifference < 0)
    {
        this->ticks = this->ticks/-widthDifference;
    }
    
    this->timer_width = value;
    this->timer_width_ticks = pow(2,this->timer_width);
    
    // If this is an attached servo, clean up
    if (this->attached())
    {
        // detach, setup and attach again to reflect new timer width
    	pwm.detachPin(this->pinNumber);
    	pwm.attachPin(this->pinNumber, REFRESH_CPS, this->timer_width);
    }        
}

int Servo::readTimerWidth()
{
    return (this->timer_width);
}

int Servo::usToTicks(int usec)
{
    return (int)((double)usec / ((double)REFRESH_USEC / (double)this->timer_width_ticks)*(((double)REFRESH_CPS)/50.0));
}

int Servo::ticksToUs(int ticks)
{
    return (int)((double)ticks * ((double)REFRESH_USEC / (double)this->timer_width_ticks)/(((double)REFRESH_CPS)/50.0));
}

 