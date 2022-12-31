/*
    klassyLights.h - built from 'lib_template.h' on 12/27/2022
    This library is intended to act as a safegaurd for
    writing various LED lighting functions by various team members
    and to prevent naming conflicts during the distributed development.

    The intention is that each 'user' will have their own light library
    to minimize changes made to the common 'main' file running the lighting loops.

    To allow the 'main.cpp' file to call function prototypes from an array, the following
    standards must be applied to each light function desired to be called externally:
        1) The function must defined within the class below (to prevent naming conflicts with other users)
        2) The public class-function must be public
        3) The public class-function must be of the type 'void' (i.e. - it must not return any value)
        4) The public class-function must not take any parameters as an input
        5) The public class-function must be static

    Other supporting functions (not needed to be called by 'main.cpp') may not have the requirements above.

    Note: there might be some uses of a #ifndef ONLINE_SIMULATION  --> these are to support a custom
    script that will concatenate all libraries directly into the main.cpp, which allows the use of
    online simulators to test code executions without the need of physical hardware
*/

/* Included header file, unless this is the online simulation */
#ifndef ONLINE_SIMULATION
  #include <klassyLights.h>
#endif

/* Initialize static class variables defined in the header file */
lightTools *klassyLights::_lightTools = NULL;
CRGB *klassyLights::_led_arr = NULL;
uint16_t klassyLights::_led_qty = 0;
uint16_t klassyLights::_led_midpoint = 0;
uint16_t klassyLights::_rotating_light_index = 0;
uint16_t klassyLights::_travelling_light_start_pos = 0;
uint16_t klassyLights::_travelling_light_end_pos = 0;

/* Constructor of the class - pass the LED array pointer + qty of used LEDs + lightTools class member */
klassyLights::klassyLights(CRGB *led_arr, uint16_t led_qty, lightTools *lightTools) {
    /* Point to the provided lightTools member */
    _lightTools = lightTools;
    
    /* update the class-bound pointers / variables */
    _led_arr = led_arr;
    _led_qty = led_qty;

    /* calculate the midpoint */
    _led_midpoint = _led_qty / 2;

    /* initialize the travelling lights */
    reset_travelling_lights();
}

/* Function to fill the entire LED array with a rainbow pattern */
void klassyLights::rainbow_pattern() {
    /* Initialize the persistent hue */
    static uint8_t rainbow_hue = 0;

    /* Increment the hue - since it's a uint8_t, and FastLED hue is 8b, it will auto-wrap */
    EVERY_N_MILLISECONDS(20) {rainbow_hue++;}

    /* Run the FastLED rainbow function */
    fill_rainbow(_led_arr, _led_qty, rainbow_hue,  7);
}

/* Draw a simple red/white pattern to resemble a candy cane */
/* Return the size of the pattern to a calling function, to allow upstream functions to cycle through if desired */
uint8_t klassyLights::candy_cane(uint8_t starting_index/*=0*/, uint8_t fade_amount/*=0*/) {
  /* set entire strip to RedRedWhiteWhite pattern */
  uint32_t light_pattern[] = {CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::White, CRGB::White};
  uint8_t qty_of_pattern = LIGHT_ARRAY_SIZE(light_pattern);

  /* draw the pattern - skipping over the "eye" positions */
  _lightTools->fill_light_pattern(_led_arr, _led_qty, light_pattern, qty_of_pattern, starting_index, fade_amount);

  return qty_of_pattern;
}

/* Draw a simple red/green/white pattern to resemble christmas spirit */
/* Return the size of the pattern to a calling function, to allow upstream functions to cycle through if desired */
uint8_t klassyLights::christmas_spirit(uint8_t starting_index/*=0*/, uint8_t fade_amount/*=0*/) {
  /* set entire strip to RedWhiteGreen pattern */
  uint32_t light_pattern[] = {CRGB::Red, CRGB::White, CRGB::Green};
  uint8_t qty_of_pattern = LIGHT_ARRAY_SIZE(light_pattern);

  /* draw the pattern - skipping over the "eye" positions */
  _lightTools->fill_light_pattern(_led_arr, _led_qty, light_pattern, qty_of_pattern, starting_index, fade_amount);

  return qty_of_pattern;
}

/* Rotates through the colors of the candy cane LED by LED */
void klassyLights::rotating_candy_cane() {
  EVERY_N_MILLISECONDS( 500 ) { _rotating_light_index = (_rotating_light_index + 1) % candy_cane(_rotating_light_index);}
}

/* Rotates through the colors of the christmas spirit LED by LED */
void klassyLights::rotating_christmas_spirit() {
  EVERY_N_MILLISECONDS( 500 ) { _rotating_light_index = (_rotating_light_index + 1) % christmas_spirit(_rotating_light_index);}
}

/* Bouncing lights from end-to-end, with the colors of a candy cane */
void klassyLights::juggle_candy_cane() {
  /* Fade all of the lights by 20 */
  fadeToBlackBy(_led_arr, _led_qty, 20);

  /* First light = Red */
  _led_arr[beatsin16(7, 0, _led_qty - 1)] |= CHSV(0, 255, 255);

  /* Second light = Pink */
  _led_arr[beatsin16(14, 0, _led_qty - 1)] |= CHSV(225, 201, 255);

  /* Third light = Offwhite */
  _led_arr[beatsin16(21, 0, _led_qty - 1)] |= 0xFFFCE6;  //offwhite

}

/* Bouncing lights from end-to-end, with the colors of a christmas spirit */
void klassyLights::juggle_christmas_spirit() {
  /* Fade all of the lights by 20 */
  fadeToBlackBy(_led_arr, _led_qty, 20);

  /* First light = Red */
  _led_arr[beatsin16(7, 0, _led_qty - 1)] |= CHSV(0, 255, 255);

  /* Second light = Offwhite */
  _led_arr[beatsin16(14, 0, _led_qty - 1)] |= 0xFFFCE6;  //offwhite

  /* Third light = Green */
  _led_arr[beatsin16(21, 0, _led_qty - 1)] |= CHSV(80, 255, 255);

}

/* Red/white pattern that fades between the two colors */
void klassyLights::fading_candy_cane() {
  EVERY_N_MILLISECONDS( 200 ) { _rotating_light_index = (_rotating_light_index + 1) % candy_cane(_rotating_light_index, 1);}
  EVERY_N_MILLISECONDS( 1 ) {candy_cane(_rotating_light_index, 3);}
}

/* Red/white pattern that fades between the christmas spirit colors */
void klassyLights::fading_christmas_spirit() {
  EVERY_N_MILLISECONDS( 200 ) { _rotating_light_index = (_rotating_light_index + 1) % christmas_spirit(_rotating_light_index, 1);}
  EVERY_N_MILLISECONDS( 1 ) {christmas_spirit(_rotating_light_index, 3);}
}

/* Light starts at both ends of the string, and travels towards each other before making a big flash */
void klassyLights::jacobs_ladder() {
  /* initialize persistent variables */
  static uint8_t explosion_time = false;

  if (!explosion_time) {explosion_time = travel_light_to_mid(CRGB::Aquamarine, 5);}
  else {
      EVERY_N_MILLISECONDS( 1 ) {fadeToBlackBy(_led_arr, _led_qty, 10);}
      EVERY_N_SECONDS( 2 ) { 
        explosion_time = !explosion_time;
        reset_travelling_lights();
      }
  }
}

/* Reset the indeces of the class-bound travelling lights */
void klassyLights::reset_travelling_lights() {
  _travelling_light_start_pos = 0;
  _travelling_light_end_pos = _led_qty - 1;
}

/* Draw light that travels from each end of the string, towards the center */
/*  returns false while the light is travelling */
/*  returns true once the lights have met in the middle */
/* Note: if 'reset_travelling_lights' isn't called, the lights will cross the middle and keep going until the opposite end */
uint8_t klassyLights::travel_light_to_mid(CRGB lead_light_color/*=CRGB::White*/, uint16_t trail_length/*=65535*/, CRGB trail_color/*=CRGB::Black*/) {
  #define TRAVEL_LIGHT_TO_MID_TIMER_MS 25       //Move the lights every 'this' ms

  /* If the trail length exists, but is set to black (default) --> match the lead light color */
  if (trail_length && !trail_color) {trail_color = lead_light_color;}

  /* Based on the travel speed, set the light colors/positions */
  EVERY_N_MILLISECONDS( TRAVEL_LIGHT_TO_MID_TIMER_MS ) {
    /* Set the lead lights' color */
    _led_arr[_travelling_light_start_pos] += lead_light_color;
    _led_arr[_travelling_light_end_pos] += lead_light_color;

    /* Set a floating fade_amount, in case the trail length is long enough that each bulb wouldn't be dimmed due to integer math */
    float fade_amount = 0.0;

    /* Skip the trail if we're on the first lighting position */
    if (_travelling_light_start_pos) {
      /* Set the trailing lights' colors and fade them as necessary */
      for (int32_t trail = _travelling_light_start_pos - 1; trail >= 0; trail--) {
        _led_arr[trail] = trail_color;
        _led_arr[(_led_qty - 1) - trail] = trail_color;

        /* if trail_length is set to max value (2^16 - 1), don't do any fading */
        if (trail_length != 65535) {
          /* fade each light an even amount, based on the trail length (each light will incrementally be dimmer) */
          /*   example: if trail length is 1, we want to fade the first trail by 50%, then next light by 100% and so on */
          fade_amount = constrain(fade_amount + 255.0 / (trail_length + 1), 0.0, 255.0);

          /* Blend the light towards 'off' (black), but first cast the floating fade amount to an unsigned int */
          _led_arr[trail] = _lightTools->fadeToColor(_led_arr[trail], CRGB::Black, (uint8_t) fade_amount);
          _led_arr[(_led_qty - 1) - trail] = _lightTools->fadeToColor(_led_arr[(_led_qty - 1) - trail], CRGB::Black, (uint8_t) fade_amount);
        }
      }
    }

    /* Travel the light towards the opposite ends */
    _travelling_light_start_pos = (_travelling_light_start_pos + 1) % _led_qty;
    _travelling_light_end_pos = (_led_qty - 1) - _travelling_light_start_pos;
  }

  /* 
    Fade all of the lights (based on desired trail_length) to make it a trail.
    Note: if trail_length = 0, no fading will be done
  */
  //uint8_t fade_amount = (trail_length ? (uint8_t) (6 * 255 / (TRAVEL_LIGHT_TO_MID_TIMER_MS * trail_length)) : 0);
  //EVERY_N_MILLISECONDS( 1 ) {fadeToBlackBy(_led_arr, _led_qty, fade_amount);}

  return (_travelling_light_start_pos > _travelling_light_end_pos);
}