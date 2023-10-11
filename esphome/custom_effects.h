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
  static uint16_t black_dot_index = 0;
  static uint16_t black_dot_speed = 10;  /* 30 = 30 changes until next led, lower means faster */
  static bool invert_colors = false;
  static Color inverted_color = Color(255-selected_color.r, 255-selected_color.g, 255-selected_color.b);

  // use abs, maybe replace later in favour of moving leds
  invert_colors = watts < 0.0;
  watts = abs(watts);

  uint16_t kw = int(watts / watts_to_kw);
  uint16_t active_main_leds = kw;
  uint16_t active_sub_led = int((watts - float(kw * watts_to_kw)) / 100);
  if (initial_run){
    it.all() = Color::BLACK;
    progress = 0;
    black_dot_index = 0;
    invert_colors = false;
  }

  // Layer 0. Black
  it.all() = Color::BLACK;

  // Layer 1: Background Layer sub LEDs

  for (int k = 0; k < it.size() - 1; k++){
    if (active_sub_led > k){
      int sub_r;
      int sub_g;
      int sub_b;
      float dim_divider = 1.5; // 2
      if (invert_colors){
        sub_r = inverted_color.r - inverted_color.r / dim_divider;
        sub_g = inverted_color.g - inverted_color.g / dim_divider;
        sub_b = inverted_color.b - inverted_color.b / dim_divider;
      } else {
        sub_r = selected_color.r - selected_color.r / dim_divider;
        sub_g = selected_color.g - selected_color.g / dim_divider;
        sub_b = selected_color.b - selected_color.b / dim_divider;
      }
      it[num_leds-k-1] = Color(sub_r, sub_g, sub_b);
    }
  }

  // Layer 2: Main LEDs

  for (int j = 0; j < it.size() - 1; j++){
    if (active_main_leds > j){
      if (invert_colors){
        it[j] = inverted_color;
      } else {
        it[j] = selected_color;
      }
    } else {
      //it[j] = Color::BLACK;
    }
  }

  // Layer 3: Black Dot rotating 

  progress++;

  Color black_dot_color = it[black_dot_index].get();
  it[black_dot_index] = black_dot_color.darken(progress * int(255/black_dot_speed));

  if (progress % black_dot_speed == 0){
    //Color black_dot_color = it[black_dot_index].get();
    //it[black_dot_index] = black_dot_color.darken(50);
    if (invert_colors) {
      black_dot_index--;
    } else {
      black_dot_index++;
    }
    //ESP_LOGD(TAG, "black_dot_index : %d", black_dot_index);
  }

  // Reset Layer 3
  /*
  if (black_dot_index == it.size()){
    if (invert_colors){
      black_dot_index = it.size();
    } else {
      black_dot_index = 0;
    }
    progress = 0;
  }
  */
  if (invert_colors && black_dot_index == 0){
    black_dot_index = it.size();
    progress = 0;
  }
  if (!invert_colors && black_dot_index == it.size()) {
    black_dot_index = 0;
    progress = 0;
  }

  //if (active_sub_led > black_dot_index){
  //  Color led_to_fade = it[black_dot_index].get();
  //  int black_r = led_to_fade.r - int(led_to_fade.r / 4);
  //  int black_g = led_to_fade.g - int(led_to_fade.g / 4);
  //  int black_b = led_to_fade.b - int(led_to_fade.b / 4);
  //  it[black_dot_index] = Color(black_r, black_g, black_b);
  //}

  /*
  if (watts > watts_to_kw){
    int sub_r = selected_color.r - selected_color.r / 2;
    int sub_g = selected_color.g - selected_color.g / 2;
    int sub_b = selected_color.b - selected_color.b / 2;
    it[active_sub_led - 1] = Color(sub_r, sub_g, sub_b);
  }
  */

  /*
  if (progress >= reset_progress){
    progress = 0;
    black_dot_index = 0;
    ESP_LOGD(TAG, "active_main_leds : %d", active_main_leds);
    ESP_LOGD(TAG, "active_sub_led : %d", active_sub_led);
    ESP_LOGD(TAG, "num_leds : %d", num_leds);
  }
  */

}

void power_leds_soc(AddressableLight &it, Color &selected_color, bool initial_run, float watts, float soc_percent){
  static auto TAG = "power_leds_soc";
  // Run watts program
  power_leds(it, selected_color, initial_run, watts);
  static uint16_t blink_progress = 255;
  static uint16_t steps_per_iteration = 5;
  static uint16_t progress_g = 0;
  static bool dir_up = true;
  if (initial_run){
    progress_g = 0;
    dir_up = true;
  };

  //int soc_dot_led = int(soc / 10) - 1;               // 100% soc = 10 LEDs
  int soc_dot_led = int((soc_percent / 10) * (it.size() / 10));  // 100% soc = 12 LEDs
  int soc_dot_led_index = soc_dot_led + 1;  // - 1;

  if (dir_up){
    progress_g = progress_g + steps_per_iteration;
  } else {
    progress_g = progress_g - steps_per_iteration;
  }

  it[soc_dot_led_index] = Color(0, progress_g, 0);

  if (progress_g >= blink_progress){
    dir_up = false;
    //ESP_LOGD(TAG, "soc_dot_led_index : %d", soc_dot_led_index);
    //ESP_LOGD(TAG, "soc_percent : %d", soc_percent);
  }
  if (progress_g <= 0){
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
