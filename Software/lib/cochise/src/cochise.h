/*
    cochise.h - built from 'lib_template.h' on 12/27/2022
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

#ifndef cochise_h
    #define cochise_h

    /* Include standard libraries needed */
    #include <Arduino.h>
    #include <FastLED.h>
    #include <lightTools.h>

    /* Class container */
    class cochise
    {   
        public: 
            /* Constructor of the class - pass the LED array pointer + qty of used LEDs */
            cochise(CRGB *led_arr, uint16_t led_qty, lightTools *lightTools);
            
            /* ADD PUBLIC USER LIGHT FUNCTIONS HERE (to be called by 'main.cpp')*/
                static void red_and_green_curtain_lights_to_middle();
                static void stack_lights_in_the_middle();
        private:
            /* ADD class-bound VARIABLES / FUNCTIONS HERE (to be used by this class only) */

            /* Class bound lightTools pointer */
            static lightTools *_lightTools;

            /* Class bound led array pointer */
            static CRGB *_led_arr;

            /* Class bound led quantity */
            static uint16_t _led_qty;

            /* Class bound (calculated) led midpoint */
            static uint16_t _led_midpoint;
                        
    };

#endif
