#include <AIOTS3Servo.h>
#if defined(ARDUINO)
	#include "Arduino.h"
#endif

static const char* TAG = "AIOTS3Servo";

Servo::Servo()
{		
	REFRESH_CPS = 50;
	this->ticks = DEFAULT_PULSE_WIDTH_TICKS;
	this->timer_width = DEFAULT_TIMER_WIDTH;
	this->pinNumber = -1;     
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
    ESP_LOGW(TAG, "Attempting to Attach servo on pin=%d min=%d max=%d",pin,min,max);

#ifdef ENFORCE_PINS
              if (pwm.hasPwm(pin))
        {
#endif

        
            if (this->pinNumber < 0)  
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

#if  defined(CONFIG_IDF_TARGET_ESP32S3)
			    ESP_LOGE(TAG, "This pin can not be a servo: %d Servo available on: 0,2,4,5,43,44,45,46,47", pin);

#else
				ESP_LOGE(TAG, "This pin can not be a servo: %d Servo available on: 0,2,4,5,43,44,45,46,47", pin);
#endif
            return 0;
        }
#endif

 
        if (min < MIN_PULSE_WIDTH)           
            min = MIN_PULSE_WIDTH;
        if (max > MAX_PULSE_WIDTH)
            max = MAX_PULSE_WIDTH;
        this->min = min;      
        this->max = max;   
      
        pwm.attachPin(this->pinNumber,REFRESH_CPS, this->timer_width );   
        ESP_LOGW(TAG, "Success to Attach servo : %d on PWM %d",pin,pwm.getChannel());

        return pwm.getChannel();
}

void Servo::detach()
{
    if (this->attached())
    {
        pwm.detachPin(this->pinNumber);

        this->pinNumber = -1;
    }
}

void Servo::write(int value)
{
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
    writeTicks(usToTicks(value));  
}

void Servo::writeTicks(int value)
{
    
    if (this->attached())   
    {
        if (value < usToTicks(this->min))      
            value = usToTicks(this->min);
        else if (value > usToTicks(this->max))
            value = usToTicks(this->max);
        this->ticks = value;
        
        pwm.write( this->ticks);
    }
}

void Servo::release()
{
    if (this->attached())    
        pwm.write(0);
}

int Servo::read()  
{
    return (map(readMicroseconds(), this->min, this->max, 0, 180));
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

int Servo::readTicks()
{
    return this->ticks;
}

bool Servo::attached()
{
    return (pwm.attached());
}

void Servo::setTimerWidth(int value)
{
  
    if (value < MINIMUM_TIMER_WIDTH )
        value = MINIMUM_TIMER_WIDTH;
    else if (value > MAXIMUM_TIMER_WIDTH)
        value = MAXIMUM_TIMER_WIDTH;
      
    int widthDifference = this->timer_width - value;
     
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
    
   
    if (this->attached())
    {
        
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

 
