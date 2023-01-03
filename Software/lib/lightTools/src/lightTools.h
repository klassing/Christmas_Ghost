/*
    lightTools.h - built from 'lib_template.h'
    This library is intended to hold common tools for creating
    various light sequences for the AOEM lab
*/

#ifndef lightTools_h
    #define lightTools_h

    /* Include standard libraries needed */
    #include <Arduino.h>
    #include <FastLED.h>

    /* Create an ARRAY_SIZE calculator for the light users if desired */
    #define LIGHT_ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

    /* Define an non class-bound structure to track moving lights by physics! */
    typedef struct {
        float r;            //position      (units are lights)
        float v;            //velocity      (units are lights/min)
        float a;            //acceleration  (units are lights/min/s)
        uint32_t t;         //current time  (resolution is in ms)
        void start(float r0, float v0, float a0) {r=r0; v=v0; a=a0; t=millis();};  //initializes a moving light
        void setR() {r += v * (millis() - t) / 60000;};     //r = v * time_delta + previous r
        void setV() {v += a * (millis() - t) / 1000;};      //v = a * time_delta + previous v
        void move() {setR(); setV(); t = millis();};        //move position & velocity, update time
    } movingLight;

    /* Class container */
    class lightTools
    {   
        public: 
            /* Constructor of the class */
            /* Not currently needed to initialize anything - let default constructor be called
                lightTools();
            */

            /* public simple function to draw a pattern, defined by an incoming array to be repeated over the full LED string */
                /* Note - pattern_starting_index can be incremented to simmulate a "walking pattern" if desired, but needs to be incremented outside of this function */
                /* Note - if "fade_amount" is provided, each light will blend towards the next index of the pattern by 'fade_amount' (set to 0 for no blending to occur and transition to be instant) */
            void fill_light_pattern(CRGB *led_arr, uint16_t led_qty, uint32_t *light_pattern, uint16_t pattern_qty, uint8_t pattern_starting_index=0, uint8_t fade_amount=0);

            /* public function to fade from one color to a different color by a specified amount */
            CRGB fadeToColor(CRGB fromCRGB, CRGB toCRGB, uint8_t amount);
        private:

            /* class-bound function to blend between two unsignedINTs by a specified amount */
            uint8_t blendU8(uint8_t fromU8, uint8_t toU8, uint8_t amount);

    };
#endif
