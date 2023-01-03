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
        2) The class-function must be public
        3) The public class-function must be of the type 'void' (i.e. - it must not return any value)
        4) The public class-function must not take any parameters as an input
        5) The public class-function must be static

    Other supporting functions (not needed to be called by 'main.cpp') may not have the requirements above.

    Note: there might be some uses of a #ifndef ONLINE_SIMULATION  --> these are to support a custom
    script that will concatenate all libraries directly into the main.cpp, which allows the use of
    online simulators to test code executions without the need of physical hardware
*/

#ifndef klassyLights_h
    #define klassyLights_h

    /* Include standard libraries needed */
    #include <Arduino.h>
    #include <FastLED.h>

    /* Include the standard light tools, unless this is the online simulation */
    #ifndef ONLINE_SIMULATION
        #include <lightTools.h>
    #endif

    /* Class container */
    class klassyLights
    {   
        public: 
            /* Constructor of the class - pass the LED array pointer + qty of used LEDs + lightTools class member */
            klassyLights(CRGB *led_arr, uint16_t led_qty, lightTools *lightTools);
            
            /* Function to fill the entire LED array with a rainbow pattern */
            static void rainbow_pattern();

            /* Rotates through the colors of the candy cane LED by LED */
            static void rotating_candy_cane();

            /* Rotates through the colors of the christmas spirit LED by LED */
            static void rotating_christmas_spirit();

            /* Bouncing lights from end-to-end, with the colors of a candy cane */
            static void juggle_candy_cane();

            /* Bouncing lights from end-to-end, with the colors of a christmas spirit */
            static void juggle_christmas_spirit();

            /* Red/white pattern that fades between the two colors */
            static void fading_candy_cane();

            /* Red/white pattern that fades between the christmas spirit colors */
            static void fading_christmas_spirit();

            /* Light starts at both ends of the string, and travels towards each other before making a big flash */
            static void jacobs_ladder();

            /* demo for light physics*/
            static void physics_demo();
        private:
            /* Draw a simple red/white pattern to resemble a candy cane */
            /* Return the size of the pattern to a calling function, to allow upstream functions to cycle through if desired */
            static uint8_t candy_cane(uint8_t starting_index=0, uint8_t fade_amount=0);

            /* Draw a simple red/green/white pattern to resemble christmas spirit */
            /* Return the size of the pattern to a calling function, to allow upstream functions to cycle through if desired */
            static uint8_t christmas_spirit(uint8_t starting_index=0, uint8_t fade_amount=0);

            /* Reset the indeces of the class-bound travelling lights */
            static void reset_travelling_lights();

            /* Draw light that travels from each end of the string, towards the center */
            /*  returns false while the light is travelling */
            /*  returns true once the lights have met in the middle */
            static uint8_t travel_light_to_mid(CRGB lead_light_color=CRGB::White, uint16_t trail_length=65535, CRGB trail_color=CRGB::Black);

            /* Class bound lightTools pointer */
            static lightTools *_lightTools;

            /* Class bound led array pointer */
            static CRGB *_led_arr;

            /* Class bound led quantity */
            static uint16_t _led_qty;

            /* Class bound (calculated) led midpoint */
            static uint16_t _led_midpoint;

            /* Class bound rotating light index */
            static uint16_t _rotating_light_index;

            /* Class bound traveling light indeces */
            static uint16_t _travelling_light_start_pos;
            static uint16_t _travelling_light_end_pos;
    };
#endif
