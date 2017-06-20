//--test 12bits 16pwm--//
//--c.herault--tux--//

#include <PWMservodriver.h>
#include <Wire.h>

#define WIRE Wire
#define ENABLE_DEBUG_OUTPUT false

PWMservodriver::PWMservodriver(uint8_t addr) 
{
	_i2caddr = addr;
}

void PWMservodriver::begin(void) 
{
	WIRE.begin();
	reset();
}

void PWMservodriver::reset(void) 
{
	write8(PCA9685_MODE1, 0x0);
}

void PWMservodriver::setPWMFreq(float freq) 
{
	freq *= 0.9;  
	float prescaleval = 25000000;
	
	prescaleval /= 4096;
	prescaleval /= freq;
	prescaleval -= 1;

	uint8_t prescale = floor(prescaleval + 0.5);
  
	uint8_t oldmode = read8(PCA9685_MODE1);
	uint8_t newmode = (oldmode&0x7F) | 0x10; 
	write8(PCA9685_MODE1, newmode); 
	write8(PCA9685_PRESCALE, prescale); 
	write8(PCA9685_MODE1, oldmode);
	
	delay(5);
	
	write8(PCA9685_MODE1, oldmode | 0xa1);  
}

void PWMservodriver::setPWM(uint8_t num, uint16_t on, uint16_t off) 
{
	WIRE.beginTransmission(_i2caddr);
	WIRE.write(LED0_ON_L+4*num);
	WIRE.write(on);
	WIRE.write(on>>8);
	WIRE.write(off);
	WIRE.write(off>>8);
	WIRE.endTransmission();
}

void PWMservodriver::setPin(uint8_t num, uint16_t val, bool invert)
{
	val = min(val, 4095);
	
  	if (invert) 
	{
  		if (val == 0) 
		{
  			setPWM(num, 4096, 0);
    	}
    	else if (val == 4095) 
		{
    		setPWM(num, 0, 4096);
    	}
    	else 
		{
    		setPWM(num, 0, 4095-val);
    	}
  	}
  	else 
	{
  		if (val == 4095) 
		{
  			setPWM(num, 4096, 0);
    	}
    	else if (val == 0) 
		{
    		setPWM(num, 0, 4096);
    	}
    	else 
		{
    		setPWM(num, 0, val);
    	}
  	}
}

uint8_t PWMservodriver::read8(uint8_t addr) 
{
	WIRE.beginTransmission(_i2caddr);
	WIRE.write(addr);
	WIRE.endTransmission();

	WIRE.requestFrom((uint8_t)_i2caddr, (uint8_t)1);
	
	return WIRE.read();
}

void PWMservodriver::write8(uint8_t addr, uint8_t d) 
{
	WIRE.beginTransmission(_i2caddr);
	WIRE.write(addr);
	WIRE.write(d);
	WIRE.endTransmission();
}
