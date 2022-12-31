/*
    lightTools.h - built from 'lib_template.h'
    This library is intended to hold common tools for creating
    various light sequences for the AOEM lab
*/


/* Included header file, unless this is the online simulation */
#ifndef ONLINE_SIMULATION
    #include <lightTools.h>
#endif

/* Constructor of the class */
    /* Not currently needed to initialize anything - let default constructor be called
        lightTools::lightTools() {

        }
    */

/* public simple function to draw a pattern, defined by an incoming array to be repeated over the full LED string */
    /* Note - pattern_starting_index can be incremented to simmulate a "walking pattern" if desired, but needs to be incremented outside of this function */
    /* Note - if "fade_amount" is provided, each light will blend towards the next index of the pattern by 'fade_amount' (set to 0 for no blending to occur and transition to be instant) */
void lightTools::fill_light_pattern(CRGB *led_arr, uint16_t led_qty, uint32_t *light_pattern, uint16_t pattern_qty, uint8_t pattern_starting_index/*=0*/, uint8_t fade_amount/*=0*/) {
    /* Do a quick check to make sure we weren't passed a starting index > pattern array.  If so - just start at the end of the pattern array */
    uint16_t pattern_index = (pattern_starting_index < pattern_qty) ? pattern_starting_index : pattern_qty - 1;

    /* Fill the passed led array with the pattern provided, with blending if needed */
    for (uint16_t led_index = 0; led_index < led_qty; led_index++) {

        /* If blending is desired, smoothly fade towards the light pattern color.  Otherwise, immediately load the light pattern color directly */
        if (fade_amount > 0) {led_arr[led_index] = fadeToColor(led_arr[led_index], light_pattern[pattern_index], fade_amount);}
        else {led_arr[led_index] = light_pattern[pattern_index];}

        /* Move to the next pattern color */
        pattern_index = (pattern_index + 1) % pattern_qty;
    }
}

/* public function to fade from one color to a different color by a specified amount */
CRGB lightTools::fadeToColor(CRGB fromCRGB, CRGB toCRGB, uint8_t amount) {
    return CRGB(
        blendU8(fromCRGB.r, toCRGB.r, amount),
        blendU8(fromCRGB.g, toCRGB.g, amount),
        blendU8(fromCRGB.b, toCRGB.b, amount)
    );
}

/* class-bound function to blend between two unsignedINTs by a specified amount */
uint8_t lightTools::blendU8(uint8_t fromU8, uint8_t toU8, uint8_t amount) {
    /* don't do anything if they're already equivalent */
    if (fromU8 == toU8) {return fromU8;}

    /* Calculate how far apart the uint8_t values are, and scale it by the 'amount' passed to blend by */
    uint8_t dU8 = scale8_video(abs(toU8 - fromU8), amount);

    /* Increment or Decrement the fromU8 by the scaled delta */
    return (fromU8 > toU8) ? fromU8 - dU8 : fromU8 + dU8;
}
