#include "music.h"
#include "leds.h"
#include "colour.h"

#define NUM_LAYERS (2)

#define CUSTOM_LAYER (0xb000)
#define CUSTOM_CANTINA (0xb001)

const uint8_t pin_row[] = {0, 5, 6, 7, 8, 9, 11};
const uint8_t pin_col[] = {3, 2, 1, 4};

const uint8_t n_rows = sizeof(pin_row) / sizeof(pin_row[0]);
const uint8_t n_cols = sizeof(pin_col) / sizeof(pin_col[0]);

uint8_t buttons[n_rows][n_cols];
uint8_t old_buttons[n_rows][n_cols];

uint16_t keys[NUM_LAYERS][n_rows][n_cols] = {
  {
    {KEY_MEDIA_VOLUME_DEC, KEY_MEDIA_VOLUME_INC, KEY_MEDIA_PREV_TRACK, KEY_MEDIA_NEXT_TRACK},
    {KEY_MEDIA_PLAY_PAUSE, 0xB000, KEY_ESC, KEY_MEDIA_STOP},
    {KEY_NUM_LOCK, KEYPAD_SLASH, KEYPAD_ASTERIX, KEYPAD_MINUS},
    {KEYPAD_7, KEYPAD_8, KEYPAD_9, KEYPAD_PLUS},
    {KEYPAD_4, KEYPAD_5, KEYPAD_6, KEYPAD_PLUS},
    {KEYPAD_1, KEYPAD_2, KEYPAD_3, KEYPAD_ENTER},
    {KEYPAD_0, KEYPAD_0, KEYPAD_PERIOD, KEYPAD_ENTER},
  },
  {
    {KEY_MEDIA_VOLUME_DEC, KEY_MEDIA_VOLUME_INC, KEY_MEDIA_PREV_TRACK, KEY_MEDIA_NEXT_TRACK},
    {KEY_MEDIA_PLAY_PAUSE, 0xB000, MODIFIERKEY_GUI, KEY_MEDIA_STOP},
    {0xB001, KEYPAD_SLASH, KEYPAD_ASTERIX, KEYPAD_MINUS},
    {KEY_7, KEY_8, KEY_9, KEYPAD_PLUS},
    {KEY_4, KEY_5, KEY_6, KEYPAD_PLUS},
    {KEY_1, KEY_2, KEY_3, KEY_ENTER},
    {KEY_0, KEY_0, KEY_PERIOD, KEY_ENTER},
  },
};

uint8_t breathe() {
  float val = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0;
  return (uint8_t)val;
}

void setup() {
  pinMode(13, OUTPUT);
  initialize_leds();
  
  for (uint8_t i = 0; i < n_rows; ++i) {
    pinMode(pin_row[i], INPUT_PULLUP);
  }
  for (uint8_t i = 0; i < n_cols; ++i) {
    pinMode(pin_col[i], INPUT_PULLUP);
  }

  //cantina();
  Serial.begin(57600);
  Serial.println("Hello");
}

void read_matrix() {
  // save our current matrix state so we can detect transitions
  for(uint8_t ri = 0; ri < n_rows; ++ri) {
    for(uint8_t ci = 0; ci < n_cols; ++ci) {
      old_buttons[ri][ci] = buttons[ri][ci];
    }
  }
  
  // scan down the matrix
  for (uint8_t ri = 0; ri < n_rows; ++ri) {
    // pull the row low
    uint8_t row_pin = pin_row[ri];
    pinMode(row_pin, OUTPUT);
    digitalWrite(row_pin, LOW);

    // scan across the matrix
    for (uint8_t ci = 0; ci < n_cols; ++ci) {
      // see if the column has been pulled low by a keypress
      // bridging the row pin
      uint8_t col_pin = pin_col[ci];
      buttons[ri][ci] = 1 - digitalRead(col_pin);
    }

    // set the row high again
    pinMode(row_pin, INPUT_PULLUP);
  }
}


uint32_t colour = 0xffffff;
uint8_t enca_old = 0;
uint8_t encb_old = 0;
int8_t enc_count[2] = {0, 0};
uint8_t layer = 0;

void loop() {
  read_matrix();

  // check quadrature
  if(buttons[0][0] != old_buttons[0][0] || buttons[0][1] != old_buttons[0][1]) {
    // left dial moved
    uint8_t enca = (buttons[0][0] << 1) | buttons[0][1];
    uint8_t sum = (enca_old << 2) | enca;
    switch(sum) {
      case 0b0001:
      case 0b0111:
      case 0b1110:
      case 0b1000:
        enc_count[0]--;
        if(enc_count[0] <= -4) {
          Keyboard.press(keys[layer][0][0]);
          Keyboard.release(keys[layer][0][0]);
          enc_count[0] = 0;
        }
        break;
      case 0b0010:
      case 0b1011:
      case 0b1101:
      case 0b0100:
        enc_count[0]++;
        if(enc_count[0] >= 4) {
          Keyboard.press(keys[layer][0][1]);
          Keyboard.release(keys[layer][0][1]);
          enc_count[0] = 0;
        }
        break;
      default:
        //colour = 0xff0000;
        break;
    }
    enca_old = enca;
  }
  else if(buttons[0][2] != old_buttons[0][2] || buttons[0][3] != old_buttons[0][3]) {
    // right dial moved
    uint8_t encb = (buttons[0][2] << 1) | buttons[0][3];
    uint8_t sum = (encb_old << 2) | encb;
    switch(sum) {
      case 0b0001:
      case 0b0111:
      case 0b1110:
      case 0b1000:
        enc_count[1]--;
        if(enc_count[1] <= -4) {
          Keyboard.press(keys[layer][0][2]);
          Keyboard.release(keys[layer][0][2]);
          enc_count[1] = 0;
        }
        break;
      case 0b0010:
      case 0b1011:
      case 0b1101:
      case 0b0100:
        enc_count[1]++;
        if(enc_count[1] >= 4) {
          Keyboard.press(keys[layer][0][3]);
          Keyboard.release(keys[layer][0][3]);
          enc_count[1] = 0;
        }
        break;
      default:
        //colour = 0xff0000;
        break;
    }
    encb_old = encb;
  }

  // go through our matrix to detect presses / releases
  // skip the first row as that is 100% encoders
  for(uint8_t ri = 1; ri < n_rows; ++ri) {
    for(uint8_t ci = 0; ci < n_cols; ++ci) {
      if(buttons[ri][ci] == 1 && old_buttons[ri][ci] == 0) {
        switch(keys[layer][ri][ci]) {
          case 0xb000:
            layer = 1;
            break;
          case 0xb001:
            cantina();
            break;
          default:
            Keyboard.press(keys[layer][ri][ci]);
            break;
        }
      }
      else if(buttons[ri][ci] == 0 && old_buttons[ri][ci] == 1) {
        switch(keys[layer][ri][ci]) {
          case 0xb000:
            layer = 0;
            break;
          case 0xb001:
            break;
          default:
            Keyboard.release(keys[layer][ri][ci]);
            break;
        }
      }
    }
  }

  uint8_t b = breathe();
  set_leds(((uint32_t)b << 24) | colour);
  //delay(1);
}
