/*
    nmayelights.h - built from 'lib_template.h' on 12/28/2022
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
  #include <nmayelights.h>
#endif

/* Initialize static class variables defined in the header file */
lightTools *nmayelights::_lightTools = NULL;
CRGB *nmayelights::_led_arr = NULL;
uint16_t nmayelights::_led_qty = 0;
uint16_t nmayelights::_led_midpoint = 0;

/* Constructor of the class - pass the LED array pointer + qty of used LEDs */
nmayelights::nmayelights(CRGB *led_arr, uint16_t led_qty, lightTools *lightTools) {
    /* Point to the provided lightTools member */
    _lightTools = lightTools;

    /* update the class-bound pointers / variables */
    _led_arr = led_arr;
    _led_qty = led_qty;

    /* calculate the midpoint */
    _led_midpoint = _led_qty / 2;
}

/* "off" state for police lights */
void nmayelights::pl_state0() {
  for (int i=0; i<_led_qty; i++) {
    _led_arr[i] = CRGB(0, 0, 0);
  }
}

/* primary color state for police lights */
void nmayelights::pl_state1(CRGB pl_color1) {
  for (int i=0; i<_led_qty; i++) {
    if (i<25) {
      _led_arr[i] = pl_color1;
    } else if (i<50){
      _led_arr[i] = CRGB(0, 0, 0);
    } else if (i<75){
      _led_arr[i] = pl_color1;
    } else if (i<100){
      _led_arr[i] = CRGB(0, 0, 0);
    } else if (i<125){
      _led_arr[i] = pl_color1;
    } else if (i<150){
      _led_arr[i] = CRGB(0, 0, 0);
    } else if (i<175){
      _led_arr[i] = pl_color1;
    } else {
      _led_arr[i] = CRGB(0, 0, 0);
    }
  }
}

/* secondary color state for police lights */
void nmayelights::pl_state2(CRGB pl_color2) {
  for (int i=0; i<_led_qty; i++) {
    if (i<25) {
      _led_arr[i] = CRGB(0, 0, 0);
    } else if (i<50){
      _led_arr[i] = pl_color2;
    } else if (i<75){
      _led_arr[i] = CRGB(0, 0, 0);
    } else if (i<100){
      _led_arr[i] = pl_color2;
    } else if (i<125){
      _led_arr[i] = CRGB(0, 0, 0);
    } else if (i<150){
      _led_arr[i] = pl_color2;
    } else if (i<175){
      _led_arr[i] = CRGB(0, 0, 0);
    } else {
      _led_arr[i] = pl_color2;
    }
  }
}

/* Pulses half the lights red and half the lights blue.*/
void nmayelights::police_lights(){
  static uint16_t global_pl_index = 0;
  EVERY_N_MILLISECONDS( 100 ) {global_pl_index = (global_pl_index + 1) % 8;}
  switch(global_pl_index) {
    case 0:
      pl_state1(CRGB(255, 0, 0));
      break;
    case 1:
      pl_state0();
      break;
    case 2:
      pl_state1(CRGB(255, 0, 0));
      break;
    case 3:
      pl_state0();
      break;
    case 4:
      pl_state2(CRGB(0, 255, 0));
      break;
    case 5:
      pl_state0();
      break;
    case 6:
      pl_state2(CRGB(0, 255, 0));
      break;
    case 7:
      pl_state0();
      break;
  }
}