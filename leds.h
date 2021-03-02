#include <WS2812Serial.h>

const int n_leds = 4;
const int led_pin = 24;

byte drawing_mem[n_leds * 3];
DMAMEM byte display_mem[n_leds * 12];

WS2812Serial leds(n_leds, display_mem, drawing_mem, led_pin, WS2812_GRB);

void initialize_leds() {
  leds.begin();
}

void set_leds(uint32_t colour) {
  leds.setBrightness((uint8_t)((colour >> 24) & 0xff));
  for(uint32_t i = 0; i < n_leds; ++i) {
    leds.setPixel(i,
      (uint8_t)((colour >> 16) & 0xff),
      (uint8_t)((colour >> 8) & 0xff),
      (uint8_t)((colour) & 0xff)
    );
  }
  leds.show();
}
