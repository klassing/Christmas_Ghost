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
        2) The class-function must be public
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
    #include "cochise.h"
#endif

/* Initialize static class variables defined in the header file */
lightTools *cochise::_lightTools = NULL;
CRGB *cochise::_led_arr = NULL;
uint16_t cochise::_led_qty = 0;
uint16_t cochise::_led_midpoint = 0;

/* Constructor of the class - pass the LED array pointer + qty of used LEDs */
cochise::cochise(CRGB *led_arr, uint16_t led_qty, lightTools *lightTools) {
    /* Point to the provided lightTools member */
    _lightTools = lightTools;

    /* update the class-bound pointers / variables */
    _led_arr = led_arr;
    _led_qty = led_qty;

    /* calculate the midpoint */
    _led_midpoint = _led_qty / 2;
}

/* RED and GREEN "curtains" lights close from left and right and meet in the middle of the LED string */
void cochise::red_and_green_curtain_lights_to_middle() {
    /* Define persistent variables */
    static uint16_t curtain_UpCounter = 0;                   //Begining of the led strand
    static uint16_t curtain_DownCounter = _led_qty - 1;      //Ending of the led strand
    static uint16_t curtain_LightToggle = false;

    // Every 25ms we'll update the LED pattern
    EVERY_N_MILLISECONDS( 25 ) { 
        if(curtain_UpCounter < _led_midpoint) {
            _led_arr[curtain_UpCounter++] = curtain_LightToggle ? CRGB::Green : CRGB::Red;        // As long as the LED upCounter has not reached the middle of the LED string increment and turn the next light
            _led_arr[curtain_DownCounter--] = curtain_LightToggle ? CRGB::Green : CRGB::Red;      // Same condition as above but this walks in our LED pattern from the "farside" of the LED string in toward the middle
        } else {
            curtain_UpCounter = 0;                          // After reaching the middle we now want to start the pattern over so we send the counters back to the "far ends" of the LED string
            curtain_DownCounter = _led_qty - 1;             
            curtain_LightToggle = !curtain_LightToggle;     // As we don't want to repeat the pattern with the same light color anymore (wouldn't be visible) we need to toggle our 'LightToggle' variable to 'change states'
        }
    }
}

void cochise::stack_lights_in_the_middle(){
    /* Define persistent variables */
    static uint16_t stack_UpCounter = 0;
    static uint16_t stack_DownCounter = _led_qty - 1;
    static uint16_t stack_LightStop = _led_midpoint;
    static uint16_t stack_pattern_count = 0;
    static uint32_t stack_color_pattern[] = {CRGB::Red, CRGB::White, CRGB::Green, CRGB::Gold};
    
    EVERY_N_MILLISECONDS(15){
        /* if LightStop is non-zero, stack towards the LightStop */
        if(stack_LightStop){
            if( stack_UpCounter < stack_LightStop ){
                /* If we're not at the begning of the strand, set the previous light to be off */
                if (stack_UpCounter > 0) {_led_arr[stack_UpCounter - 1] = CRGB::Black;}
                if (stack_DownCounter < (_led_qty - 1)) { _led_arr[stack_DownCounter + 1] = CRGB::Black;}

                /* Set the current light, and move the counters towards the LightStop */
                _led_arr[ stack_UpCounter++ ] = stack_color_pattern[stack_pattern_count];
                _led_arr[ stack_DownCounter-- ] = stack_color_pattern[stack_pattern_count];

            } else {
                /* Counters have reached the LightStop - move the LightStop and reset counters */
                stack_UpCounter = 0;
                stack_DownCounter = _led_qty - 1;
                stack_LightStop--;

                /* Move the color pattern, wrapping around as needed */
                stack_pattern_count = (stack_pattern_count + 1) % LIGHT_ARRAY_SIZE(stack_color_pattern);
            }
        } else {
            /* LightStop has reached the begining of the strand, reset everything*/
            fadeToBlackBy(_led_arr, _led_qty, 255);
            stack_LightStop = _led_midpoint;
            stack_UpCounter = 0;
            stack_DownCounter = _led_qty - 1;
        }
    }
}
