#ifndef FastPWMdac_H
#define FastPWMdac_H
class FastPWMdac
{
public:
  void init(uint8_t _timer1PWMpin, uint8_t resolution);
  void analogWrite8bit(uint8_t value8bit);
  void analogWrite10bit(uint16_t value10bit);
private:
  uint8_t timer1PWMpin;
};
#endif
