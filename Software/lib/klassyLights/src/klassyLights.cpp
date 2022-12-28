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
*/

/* Included header file */
#include <klassyLights.h>

/* Initialize static class variables defined in the header file */
lightTools *klassyLights::_lightTools = NULL;
CRGB *klassyLights::_led_arr = NULL;
uint16_t klassyLights::_led_qty = 0;
uint16_t klassyLights::_led_midpoint = 0;
uint16_t klassyLights::rotating_light_index = 0;

/* Constructor of the class - pass the LED array pointer + qty of used LEDs + lightTools class member */
klassyLights::klassyLights(CRGB *led_arr, uint16_t led_qty, lightTools *lightTools) {
    /* Point to the provided lightTools member */
    _lightTools = lightTools;
    
    /* update the class-bound pointers / variables */
    _led_arr = led_arr;
    _led_qty = led_qty;

    /* calculate the midpoint */
    _led_midpoint = _led_qty / 2;
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
  EVERY_N_MILLISECONDS( 500 ) { rotating_light_index = (rotating_light_index + 1) % candy_cane(rotating_light_index);}
}

/* Rotates through the colors of the christmas spirit LED by LED */
void klassyLights::rotating_christmas_spirit() {
  EVERY_N_MILLISECONDS( 500 ) { rotating_light_index = (rotating_light_index + 1) % christmas_spirit(rotating_light_index);}
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
  EVERY_N_MILLISECONDS( 200 ) { rotating_light_index = (rotating_light_index + 1) % candy_cane(rotating_light_index, 1);}
  EVERY_N_MILLISECONDS( 1 ) {candy_cane(rotating_light_index, 3);}
}

/* Red/white pattern that fades between the christmas spirit colors */
void klassyLights::fading_christmas_spirit() {
  EVERY_N_MILLISECONDS( 200 ) { rotating_light_index = (rotating_light_index + 1) % christmas_spirit(rotating_light_index, 1);}
  EVERY_N_MILLISECONDS( 1 ) {christmas_spirit(rotating_light_index, 3);}
}
