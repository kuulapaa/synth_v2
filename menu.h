// Menusysteemi, melkeen toimii

#define CW      1
#define CCW     2
#define BUTTON  3
#define CLEAR   0

enum SUBMENUS
{
  NA,
  RING
};

enum Waveforms {
SINE,
TRI,
SAW,
SQU  
};

const char * waveforms[4][10] {
  {"Sine"},
  {"Triangle"},
  {"Sawtooth"},
  {"Square"}
};

typedef struct {
  uint8_t range [2];
  int16_t value;
  const char * desc;
  uint8_t submenu;
} MenuItem;

MenuItem items [] =
{
  //    range           value       description         submenu
  {{ 1, 255 },        0,          "Speed",            NA},
  {{ 0, 3 },          0,          "Waveform",         NA},
  {{ 0, 0 },          0,          "RANDOM!",          NA},
  {{ 0, 0 },          0,          "Ring mod",         RING},
  {{ 0, 0 },          0,          "Save",             NA},
  {{ 0, 0 },          0,          "Load",             NA}
};

void item_adjust(MenuItem *content, uint8_t, uint8_t);
uint8_t selected = 255;
int8_t item;
int8_t input;
  



void item_adjust (MenuItem *content, uint8_t input) {

  switch (input)
  {
    case CW:
      content->value++;
      if (content->value > content->range[1]) content->value = content->range[0];
      break;
    case CCW:
      content->value--;
      if (content->value < content->range[0]) content->value = content->range[1];
      break;
    case BUTTON:
      break;

    default:;
  }
}
