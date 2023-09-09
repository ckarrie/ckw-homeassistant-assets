/*

Usage:

esphome:
  includes:
    - /config/ckw-homeassistant-assets/esphome/custom_effects.h

light:
  - platform: neopixelbus
    type: GRB
    variant: WS2811
    pin: GPIO5
    num_leds: 12
    name: "NeoPixel Light D1"
    effects: 
      - addressable_lambda:
          name: "github update_power_leds"
          update_interval: 10ms
          lambda: |-
            update_power_leds(it, current_color, initial_run, 50.0);

*/



void update_power_leds(AddressableLight &it, Color &selected_color, bool initial_run, float watts){
  static uint16_t num_leds = it.size();
  static uint16_t progress = 0;
  static uint16_t reset_progress = 10;
  if (initial_run){
    it.all() = Color::BLACK;
    progress = 0;
  }
  progress++;
  if (progress > reset_progress){
    it.all() = selected_color;
    progress = 0;
  } else {
    it.all() = Color::BLACK;
  }
}
