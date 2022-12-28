/*
    [CLASS_NAME].h - built from 'lib_template.h' on [TODAY]
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
#include <[CLASS_NAME].h>

/* Initialize static class variables defined in the header file */
lightTools *[CLASS_NAME]::_lightTools = NULL;
CRGB *[CLASS_NAME]::_led_arr = NULL;
uint16_t [CLASS_NAME]::_led_qty = 0;
uint16_t [CLASS_NAME]::_led_midpoint = 0;

/* Constructor of the class - pass the LED array pointer + qty of used LEDs */
[CLASS_NAME]::[CLASS_NAME](CRGB *led_arr, uint16_t led_qty, lightTools *lightTools) {
    /* Point to the provided lightTools member */
    _lightTools = lightTools;

    /* update the class-bound pointers / variables */
    _led_arr = led_arr;
    _led_qty = led_qty;

    /* calculate the midpoint */
    _led_midpoint = _led_qty / 2;
}