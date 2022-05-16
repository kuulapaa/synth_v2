#include <TimerOne.h>
#include "FastPWMdac.h"
void FastPWMdac::init(uint8_t _timer1PWMpin, uint8_t resolution)
{ 
  timer1PWMpin = _timer1PWMpin;
  if(resolution == 8) Timer1.initialize(32);
  if(resolution == 10) Timer1.initialize(128);
  Timer1.pwm(timer1PWMpin, 0); // dummy, required before setPwmDuty()
}
void FastPWMdac::analogWrite8bit(uint8_t value8bit)
{ 
  Timer1.setPwmDuty(timer1PWMpin, value8bit*4); // faster than pwm()
}
void FastPWMdac::analogWrite10bit(uint16_t value10bit)
{
  Timer1.setPwmDuty(timer1PWMpin, value10bit); // faster than pwm()
}
