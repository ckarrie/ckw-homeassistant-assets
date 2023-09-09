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
  uint16_t kw = int(watts / watts_to_kw);
  uint16_t active_main_leds = kw;
  uint16_t active_sub_led = int((watts - float(kw * watts_to_kw)) / 100);
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

  for (int k = 0; k < it.size(); k++){
    if (active_sub_led > k){
      int sub_r = selected_color.r - selected_color.r / 2;
      int sub_g = selected_color.g - selected_color.g / 2;
      int sub_b = selected_color.b - selected_color.b / 2;
      it[num_leds-k] = Color(sub_r, sub_g, sub_b);
    }
  }  

  /*
  if (watts > watts_to_kw){
    int sub_r = selected_color.r - selected_color.r / 2;
    int sub_g = selected_color.g - selected_color.g / 2;
    int sub_b = selected_color.b - selected_color.b / 2;
    it[active_sub_led - 1] = Color(sub_r, sub_g, sub_b);
  }
  */

  progress++;
  if (progress >= reset_progress){
    progress = 0;
    ESP_LOGD(TAG, "active_main_leds : %d", active_main_leds);
    ESP_LOGD(TAG, "active_sub_led : %d", active_sub_led);
    ESP_LOGD(TAG, "num_leds : %d", num_leds);
  }
}

void power_leds_soc(AddressableLight &it, Color &selected_color, bool initial_run, float watts, float soc){
  power_leds(it, selected_color, initial_run, watts);
  static uint16_t blink_progress = 255;
  static uint16_t progress = 0;
  bool dir_up = true;
  if (initial_run){
    progress = 0;
    dir_up = true;
  };
  
  int soc_dot_led = int(soc / 10) - 1;
  
  if (dir_up){
    progress++;
  } else {
    progress--;
  }
  
  it[soc_dot_led] = Color(0, progress, 0);
  
  if (progress == blink_progress){
    dir_up = false;
  }
  if (progress == 0){
    dir_up = true;
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
