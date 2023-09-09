/*

Usage:

esphome:
  includes:
    - /config/ckw-homeassistant-assets/esphome/custom_effects.h

sensor:
  - id: pv
    platform: homeassistant
    entity_id: sensor.helper_pv_sum_yaml
    internal: True

light:
  - platform: neopixelbus
    type: GRB
    variant: WS2811
    pin: GPIO5
    num_leds: 12
    name: "NeoPixel Light D1"
    effects: 
      - addressable_lambda:
          name: "github power_leds"
          update_interval: 10ms
          lambda: |-
            int watts = id(pv);
            power_leds(it, current_color, initial_run, watts);

*/



void power_leds(AddressableLight &it, Color &selected_color, bool initial_run, float watts){
  static auto TAG = "power_leds";
  static uint16_t num_leds = it.size();
  static uint16_t progress = 0;
  static uint16_t reset_progress = 1000;
  static uint16_t watts_to_kw = 1000;
  static uint16_t kw = int(watts / watts_to_kw);
  static uint16_t active_main_leds = kw;
  if (initial_run){
    it.all() = Color::BLACK;
    progress = 0;
  }
  
  for (int j = 0; j < it.size(); j++){
    if (active_main_leds > j){
      it[j] = selected_color;
    } else {
      it[j] = Color::BLACK;
    }
  }  

  progress++;
  if (progress >= reset_progress){
    progress = 0;
    ESP_LOGD(TAG, "active_main_leds : %d", active_main_leds);
  }
  
}


void blink_leds(AddressableLight &it, Color &selected_color, bool initial_run, float watts){
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
