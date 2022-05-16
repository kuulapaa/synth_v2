// DAC+Phas0r

// Keskeneräinen ja buginen, mutta toisaalta se tässä oli myös idea :D

// Muuttaa sisääntulevan signaalin amplitudia signaaligeneraattorin tahdissa
// Valittavissa sini, kolmio, kantti ja saha!
// DAC muuntimen output yhdistetty lediin ja ledi kammiossa (mustan kutistemuovin sisällä) valovastuksien kanssa. -> Valomäärän mukaan muuttuva amplipudi

// Valkoinen = INPUT
// Punainen = OUTPUT

// Vaatii toimiakseen 9-12V DC, siis Arduinoon kiinni. USB voi olla samaan aikaan kiinni, ei haittaa

// OCR0A -> "Nopeus" -> 1-255
// TCCR0B -> Prescaler "Karkeampi nopeudensäätö" -> 0x03 - 0x05
// DAC = 10bit (overkill)
// Pitäisi näillä esiasetuksilla tuottaa jo jonkinlaista phaser-efektiä.

// Generaatiot tapahtuu keskeytysfunktiossa ISR(TIMER0_COMPA_vect) joida varmaan pystyy optimoimaan, mutta tuo oli parasta mihin itse kykenin...
// summing on kahden eri generaation summa eli ikään kuin mudulaatio, en tiedä toimiiko. Varmaan jänniä ääniä saa aikaan, mutta ei hyviä. Ei taida löytyä omaa säätöä edes sille moduloivalle vielä.

// Keskimmäiset potikat tekevät jotain, volume ja mixaus
// Reunassa olevat vain yhdistetty ADC-tuloihin, voi käyttää miten lystää (POTR ja POTL)
// Rotary encoder kytkimellä toimii ja ei toimi, en oikeen jaksanut ihmetellä sitä paremmin

// Load ja Save eivät tee mitään mutta ajatus oli että voi tallentaa "hyvän" setupin EEPROMille.

#include "FastPWMdac.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include "menu.h"
// OLED
#define I2C_ADDRESS 0x3C
#define RST_PIN -1
SSD1306AsciiAvrI2c oled;

// ROTARY ENCODER
#define outputA 3
#define outputB 2
#define buttonE 4
uint8_t r_state;
uint8_t r_last;
int8_t r_counter;

#define POTL A0
#define POTR A1

#define U10MAX 1023
#define U8MAX 255

// SINE LOOK UP TABLE
const PROGMEM uint16_t sine_table[] = {512, 515, 518, 521, 524, 527, 530, 533, 537, 540, 543, 546, 549, 552, 555, 559, 562, 565, 568, 571, 574, 577, 580, 583, 587, 590, 593, 596, 599, 602, 605, 608, 611, 614, 617, 621, 624, 627, 630, 633, 636, 639, 642, 645, 648, 651, 654, 657, 660, 663, 666, 669, 672, 675, 678, 681, 684, 687, 690, 693, 696, 699, 701, 704, 707, 710, 713, 716, 719, 722, 725, 727, 730, 733, 736, 739, 741, 744, 747, 750, 753, 755, 758, 761, 764, 766, 769, 772, 774, 777, 780, 782, 785, 788, 790, 793, 796, 798, 801, 803, 806, 809, 811, 814, 816, 819, 821, 824, 826, 829, 831, 834, 836, 838, 841, 843, 846, 848, 850, 853, 855, 857, 860, 862, 864, 866, 869, 871, 873, 875, 878, 880, 882, 884, 886, 888, 890, 893, 895, 897, 899, 901, 903, 905, 907, 909, 911, 913, 915, 917, 919, 920, 922, 924, 926, 928, 930, 931, 933, 935, 937, 939, 940, 942, 944, 945, 947, 949, 950, 952, 953, 955, 957, 958, 960, 961, 963, 964, 966, 967, 968, 970, 971, 973, 974, 975, 977, 978, 979, 980, 982, 983, 984, 985, 986, 988, 989, 990, 991, 992, 993, 994, 995, 996, 997, 998, 999, 1000, 1001, 1002, 1003, 1004, 1004, 1005, 1006, 1007, 1008, 1008, 1009, 1010, 1011, 1011, 1012, 1013, 1013, 1014, 1014, 1015, 1015, 1016, 1017, 1017, 1017, 1018, 1018, 1019, 1019, 1020, 1020, 1020, 1021, 1021, 1021, 1021, 1022, 1022, 1022, 1022, 1022, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1022, 1022, 1022, 1022, 1022, 1021, 1021, 1021, 1021, 1020, 1020, 1020, 1019, 1019, 1018, 1018, 1017, 1017, 1017, 1016, 1015, 1015, 1014, 1014, 1013, 1013, 1012, 1011, 1011, 1010, 1009, 1008, 1008, 1007, 1006, 1005, 1004, 1004, 1003, 1002, 1001, 1000, 999, 998, 997, 996, 995, 994, 993, 992, 991, 990, 989, 988, 986, 985, 984, 983, 982, 980, 979, 978, 977, 975, 974, 973, 971, 970, 968, 967, 966, 964, 963, 961, 960, 958, 957, 955, 953, 952, 950, 949, 947, 945, 944, 942, 940, 939, 937, 935, 933, 931, 930, 928, 926, 924, 922, 920, 919, 917, 915, 913, 911, 909, 907, 905, 903, 901, 899, 897, 895, 893, 890, 888, 886, 884, 882, 880, 878, 875, 873, 871, 869, 866, 864, 862, 860, 857, 855, 853, 850, 848, 846, 843, 841, 838, 836, 834, 831, 829, 826, 824, 821, 819, 816, 814, 811, 809, 806, 803, 801, 798, 796, 793, 790, 788, 785, 782, 780, 777, 774, 772, 769, 766, 764, 761, 758, 755, 753, 750, 747, 744, 741, 739, 736, 733, 730, 727, 725, 722, 719, 716, 713, 710, 707, 704, 701, 699, 696, 693, 690, 687, 684, 681, 678, 675, 672, 669, 666, 663, 660, 657, 654, 651, 648, 645, 642, 639, 636, 633, 630, 627, 624, 621, 617, 614, 611, 608, 605, 602, 599, 596, 593, 590, 587, 583, 580, 577, 574, 571, 568, 565, 562, 559, 555, 552, 549, 546, 543, 540, 537, 533, 530, 527, 524, 521, 518, 515, 512, 508, 505, 502, 499, 496, 493, 490, 486, 483, 480, 477, 474, 471, 468, 464, 461, 458, 455, 452, 449, 446, 443, 440, 436, 433, 430, 427, 424, 421, 418, 415, 412, 409, 406, 402, 399, 396, 393, 390, 387, 384, 381, 378, 375, 372, 369, 366, 363, 360, 357, 354, 351, 348, 345, 342, 339, 336, 333, 330, 327, 324, 322, 319, 316, 313, 310, 307, 304, 301, 298, 296, 293, 290, 287, 284, 282, 279, 276, 273, 270, 268, 265, 262, 259, 257, 254, 251, 249, 246, 243, 241, 238, 235, 233, 230, 227, 225, 222, 220, 217, 214, 212, 209, 207, 204, 202, 199, 197, 194, 192, 189, 187, 185, 182, 180, 177, 175, 173, 170, 168, 166, 163, 161, 159, 157, 154, 152, 150, 148, 145, 143, 141, 139, 137, 135, 133, 130, 128, 126, 124, 122, 120, 118, 116, 114, 112, 110, 108, 106, 104, 103, 101, 99, 97, 95, 93, 92, 90, 88, 86, 84, 83, 81, 79, 78, 76, 74, 73, 71, 70, 68, 66, 65, 63, 62, 60, 59, 57, 56, 55, 53, 52, 50, 49, 48, 46, 45, 44, 43, 41, 40, 39, 38, 37, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 19, 18, 17, 16, 15, 15, 14, 13, 12, 12, 11, 10, 10, 9, 9, 8, 8, 7, 6, 6, 6, 5, 5, 4, 4, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 6, 7, 8, 8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 38, 39, 40, 41, 43, 44, 45, 46, 48, 49, 50, 52, 53, 55, 56, 57, 59, 60, 62, 63, 65, 66, 68, 70, 71, 73, 74, 76, 78, 79, 81, 83, 84, 86, 88, 90, 92, 93, 95, 97, 99, 101, 103, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 133, 135, 137, 139, 141, 143, 145, 148, 150, 152, 154, 157, 159, 161, 163, 166, 168, 170, 173, 175, 177, 180, 182, 185, 187, 189, 192, 194, 197, 199, 202, 204, 207, 209, 212, 214, 217, 220, 222, 225, 227, 230, 233, 235, 238, 241, 243, 246, 249, 251, 254, 257, 259, 262, 265, 268, 270, 273, 276, 279, 282, 284, 287, 290, 293, 296, 298, 301, 304, 307, 310, 313, 316, 319, 322, 324, 327, 330, 333, 336, 339, 342, 345, 348, 351, 354, 357, 360, 363, 366, 369, 372, 375, 378, 381, 384, 387, 390, 393, 396, 399, 402, 406, 409, 412, 415, 418, 421, 424, 427, 430, 433, 436, 440, 443, 446, 449, 452, 455, 458, 461, 464, 468, 471, 474, 477, 480, 483, 486, 490, 493, 496, 499, 502, 505, 508};

// ARDUINO PWM(DAC) OUTPUT
const uint8_t dacPin = 9;

FastPWMdac dac;



uint8_t summing = 0;


volatile uint8_t gen_last = SINE;

struct Generator {
  uint16_t output;
  uint16_t freq;
  uint16_t step;
};

volatile struct Generator gen[2];

enum Gen{
  PRI,
  MOD
};

void setup()
{
  pinMode (POTL, INPUT);
  pinMode (POTR, INPUT);
  Serial.begin(115200);
  Serial.println("OK");
  Serial.println(analogRead(POTR));
  Serial.println(analogRead(POTL));
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(Verdana12_bold);
  oled.clear();
  oled.print("Great.");
  delay(1000);
  pinMode (outputA, INPUT);
  pinMode (outputB, INPUT);
  pinMode (buttonE, INPUT);

  digitalWrite(outputA, HIGH);
  digitalWrite(outputB, HIGH);
  digitalWrite(buttonE, HIGH);
  dac.init(dacPin, 10); // initialization for 10 bit resolution
  
  items[1].value = 0; // <- 0 = SINE
  TCCR0A = 0x01;
  TIMSK0 = 0x02; // Start timer from zero when interrupt occurs
  OCR0A = 20;    // Interrupt compare match -> change frequency with this!

  TCCR0B = 0x04; // Prescaler 0x03 = Too fast; 0x05 = Slow enough? :)
  

  items[0].value = OCR0A;
  gen[PRI].freq = 2;
  item++;
}

void loop()
{
  // For serial adjust, use s=select, d=done, +, - to change things in the menu
  if (Serial.available() > 0) {
    input = Serial.read();
    if (input == 's') selected = item;
    if (input == 'd') selected = 255;
    if (selected != 255) {
      if (input == '+') item_adjust(&items[item], CW);
      else if (input == '-') item_adjust(&items[item], CCW);
      Serial.print(items[item].desc);
      Serial.print("   ");
      Serial.println(items[item].value);
      if (items[0].value < 256) OCR0A = (uint8_t)items[0].value;
    }
    if (selected == 255) {
      if (input == '+') item++;
      else if (input == '-') item--;
      if (item < 0) item = 5;
      else if (item > 5) item = 0;
      Serial.print(items[item].desc);
      Serial.print("   ");
      Serial.println(items[item].value);
    }
  }

  // Rotary switch encoder readout
  // BUGI ALLA! Rotary toimii vain joskus ja ehkä myötäpäivää. 
  r_state = digitalRead(outputA);
  if ((r_last == LOW) && (r_state == HIGH)) {
    if (selected == 255) {
      if (digitalRead(outputB) == LOW) item++;
      else item--;
      if (item < 0) item = 5;  
      else if (item > 5) item = 0;
        oled.clear();
        oled.print(items[item].desc);
        oled.print("   ");
        oled.println(items[item].value);
        if (items[0].value < 256) OCR0A = (uint8_t)items[0].value;  // Nopeus ei kuitenkaan muutu?
    }
    if (selected != 255) {
      if (digitalRead(outputB) == LOW) item_adjust(&items[item], CW);
      else item_adjust(&items[item], CCW);
      oled.clear();
      if (selected != 255) oled.print(">");
      oled.print(items[item].desc);
      oled.print("   ");
      oled.println(items[item].value);
    }
  }
  r_last = r_state;

  // Rotary encoder button readout
  if (!digitalRead(buttonE)){
    delay(100);
    if (!digitalRead(buttonE)){ 
      while(!digitalRead(buttonE));
      if (selected == 255) selected = item;
      else if (selected != 255) {
        selected = 255;
        oled.print(" OK");
      }
      delay(500);
    }
  }
}

// Waveform generators
ISR(TIMER0_COMPA_vect)
{
  uint8_t gen_active = items[1].value;
  if (gen_active != gen_last) {
    gen[PRI].step = 0;
    gen_last = gen_active;
    Serial.println("Gen change");
  }
  switch (gen_active) {
/*-----------------------------------------------------------------------*/
    // PRIMARY GENERATORS
    // SINE WAVE GENERATOR
    case SINE:;
      gen[PRI].step += gen[PRI].freq;
      if (gen[PRI].step > U10MAX) gen[PRI].step = 0;
      gen[PRI].output = pgm_read_word_near(sine_table + gen[PRI].step);
      break;

    // TRIANGLE GENERATOR
    case TRI:;
      if (gen[PRI].step > (2042)) gen[PRI].step = 0;
      gen[PRI].step += (gen[PRI].freq * 2);
      if (gen[PRI].step > 1023) gen[PRI].output = (gen[PRI].step & 0x3FF) ^ 0x3FF; // XOR to ramp down
      else gen[PRI].output = gen[PRI].step;
      break;

    // SAWTOOTH GENERATOR
    case SAW:;
      gen[PRI].step += gen[PRI].freq;
      if (gen[PRI].step > U10MAX) gen[PRI].step = 0;
      gen[PRI].output = gen[PRI].step;
      break;

    // SQUARE GENERATOR
    case SQU:;
      gen[PRI].step += gen[PRI].freq;
      if (gen[PRI].step > U10MAX) gen[PRI].step = 0;
      if (gen[PRI].step < U10MAX / 2) gen[PRI].output = U10MAX;
      else gen[PRI].output = 0;
      break;
  }
  /*-----------------------------------------------------------------------------*/
  if (summing) {
    switch (items[1].value) {
      
    // MODULATING GENERATORS
    // SINE WAVE GENERATOR
    case SINE:;

      gen[MOD].step += gen[MOD].freq;
      if (gen[MOD].step > U10MAX) gen[MOD].step = 0;
      gen[MOD].output = pgm_read_word_near(sine_table + gen[MOD].step);
      break;

    // TRIANGLE GENERATOR
    case TRI:;
      if (gen[MOD].step > (U10MAX * 2)) gen[MOD].step = 0;
      gen[MOD].step += gen[MOD].freq * 2;
      if (gen[MOD].step > U10MAX) gen[MOD].output = (gen[MOD].step & U10MAX) ^ U10MAX; // XOR to ramp down
      else gen[MOD].output = gen[MOD].step;
      break;

    // SAWTOOTH GENERATOR
    case SAW:;
      gen[MOD].step += gen[MOD].freq;
      if (gen[MOD].step > U10MAX) gen[MOD].step = 0;
      gen[MOD].output = gen[MOD].step;
      break;

    // SQUARE GENERATOR
    case SQU:;
      gen[MOD].step += gen[MOD].freq;
      if (gen[MOD].step > U10MAX) gen[MOD].step = 0;
      if (gen[MOD].step < U10MAX / 2) gen[MOD].output = U10MAX;
      else gen[MOD].output = 0;
      break;
  }
    dac.analogWrite10bit(gen[PRI].output >> 1 + gen[MOD].output >> 1);
  }
  else dac.analogWrite10bit(gen[PRI].output);
  
  TCNT0 =0x00;                // If not reset here, COMPA fires all over the place ehkä
  TIFR0 = (1<<OCF0A);         // Clear interrupt flag
  TIMSK0 = (1<<OCIE0A);       // Enable COMPA
}
