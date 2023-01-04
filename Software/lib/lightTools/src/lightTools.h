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
    //f_movingLight_t structure (uses floating point math)
    typedef struct {
        float r;                    //position         (units are a a light)
        float v;                    //velocity        (units are lights/min)
        float a;                    //acceleration    (units are lights/min/s)
        uint32_t t;                //position time (resolution is in ms)

        //Function to initialize a moving light
            //param r0 = initial position of the light
            //param v0 = initial velocity of the light (lights/min)
            //param a0 = initial acceleration of the light (lights/min/s)
        void start(float r0, float v0, float a0) {r=r0; v=v0; a=a0; t=millis();};

        //Function to move the position of the light
        void setR() {r += (v  * (millis() - t) / 60000.0);};

        //Function to move the velocity of the light
        void setV() {v += (a * (millis() - t) / 1000.0);};

        //Function to move a light (updating position and velocity)
        void move() {setR(); setV(); t=millis();};

        //Function to return the fractional portion of the position, scaled as an 8-bit unsigned int
        uint8_t getRscale8() {return (uint8_t)(255 * (r - (uint32_t)r));};

        //Function to return the current direction of travel
        //  return 1 = travelling forward (positive velocity)
        //  return 0 = not moving
        //  return -1 = travelling backward (negative velocity)
        int8_t getVdir() {if(v > 0) {return 1;} else if(v < 0) {return -1;} else {return 0;}};
    } f_movingLight_t;


    //r is bit-shifted up by this amount
    #define MOVINGLIGHT_R_SCALAR 8

    //v is set to lights-per-minute (60000ms per unit of movement)
    #define MOVINGLIGHT_V_UNIT 60000UL

    //a is set to lights-per-minute-per-second (1000ms per unit of movement)
    #define MOVINGLIGHT_A_UNIT 1000UL

    //Calculate the next interval for checking r
    //  This should occur in 60000 (ms/min) / velocity (light/min) / MOVINGLIGHT_R_SCALAR
    //  param 'x' = velocity value
    //  NOTE --> if 'x' = 0, this will force it to be 1 to prevent divide-by-zero
    #define MOVINGLIGHT_R_TIMER(x) (MOVINGLIGHT_V_UNIT / (1 << MOVINGLIGHT_R_SCALAR) / (abs(x) ? abs(x) : 1))

    //Calculate the next interval for checking v
    //  This should occur in 1000 (ms/s) / acceleration (lights/min/s)
    //  param 'x' = acceleration value
    //  NOTE --> if 'x' = 0, this will force it to be 1 to prevent divide-by-zero
    #define MOVINGLIGHT_V_TIMER(x) (MOVINGLIGHT_A_UNIT / (abs(x) ? abs(x) : 1))

    //Calculate the next positional movement
    //  param 'x' = current velocity
    //  param 'y' = current positional real-time tracker
    #define MOVINGLIGHT_R_MOVE(x, y) (((x << MOVINGLIGHT_R_SCALAR) * (millis() - y)) / MOVINGLIGHT_V_UNIT)

    //Calculate the next positional movement
    //  param 'x' = current acceleration
    //  param 'y' = current velocity real-time tracker
    #define MOVINGLIGHT_V_MOVE(x, y) ((x * (millis() - y)) / MOVINGLIGHT_A_UNIT)

    //int_movingLight_t structure (uses only integer math)
    typedef struct {
        uint32_t r;                 //position      (units are a 1/256th of a light)
        int32_t v;                  //velocity      (units are lights/min)
        int32_t a;                  //acceleration  (units are lights/min/s)
        uint32_t rTimer;            //interval timer for when the position should actually move, based on velocity
        uint32_t rt;                //real-time caculator to properly determine positional movement based on time
        uint32_t vTimer;            //interval timer for when the velocity should actually move, based on acceleration
        uint32_t vt;                //real-time caculator to properly determine velocity movement based on time

        //Function to initialize a moving light
            //param r0 = initial position of the light
            //param v0 = initial velocity of the light (lights/min)
            //param a0 = initial acceleration of the light (lights/min/s)
        void start(uint16_t r0, int32_t v0, int32_t a0) {
            //set initial position, scaled up
            r=r0 << MOVINGLIGHT_R_SCALAR;

            //set initial velocity and acceleration
            v=v0;   a=a0;

            //setup initial position timer
            rTimer = MOVINGLIGHT_R_TIMER(v0);

            //setup initial velocity timer
            vTimer = MOVINGLIGHT_V_TIMER(a0);

            //setup initial real-time trackers
            vt=rt=millis();
        };

        //Function to move the position of the light
        void setR() {
            //rtimer is only needed for the first execution
            EVERY_N_MILLIS_I(rTimeObj, rTimer) {
                //update the position, based on real time
                r += MOVINGLIGHT_R_MOVE(v, rt);

                //update the positional real-time tracker
                rt = millis();

                //setup the next position timer
                //rtimer = MOVINGLIGHT_R_TIMER(v);
                rTimeObj.setPeriod(MOVINGLIGHT_R_TIMER(v));
                //Serial.println("[" + String(millis(), DEC) + "] updating r = " + String(r,DEC) + " | rTimer = " + String(MOVINGLIGHT_R_TIMER(v),DEC));
            }
        };

        //Function to move the velocity of the light
        void setV() {
            EVERY_N_MILLIS_I(vTimeObj, vTimer) {
                //update the position, based on real time
                v += MOVINGLIGHT_V_MOVE(a, vt);

                //update the velocity real-time tracker
                vt = millis();

                //setup the next position timer
                //vTimer = MOVINGLIGHT_V_TIMER(a);
                vTimeObj.setPeriod(MOVINGLIGHT_V_TIMER(a));
                //Serial.println("[" + String(millis(), DEC) + "] updating v = " + String(v,DEC) + " | vTimer = " + String(MOVINGLIGHT_V_TIMER(a),DEC));
            }
        };

        //Function to return light position
        uint32_t getR() {return r >> MOVINGLIGHT_R_SCALAR;};

        //Function to move a light (updating position and velocity)
        void move() {setR(); setV();};
    } int_movingLight_t;

   /* - work in progress -
        #define MOVINGLIGHT_MULTI_V_SIGN 0    //velocity sign bit (true = positive, false = negative)
        #define MOVINGLIGHT_MULTI_V_STATIC 1  //velocity static boolean (true = 0 velocity, false = velocity greater than 0)
        #define MOVINGLIGHT_MULTI_A_SIGN 2    //acceleration sign bit (true = positive, false = negative)
        #define MOVINGLIGHT_MULTI_A_STATIC 3  //acceleration static boolean (true = 0 velocity, false = velocity greater than 0)
        #define MOVINGLIGHT_SET_MULTI(x, loc, val) x |= (value << loc)
        #define MOVINGLIGHT_GET_MULTI(x, loc) ((x >> loc) & 0x1)
        typedef struct {
            uint16_t r;                 //position      (units are a light)
            uint16_t vt;                //velocity-time (units are ms/lights)
            uint16_t at;                //accel-time    (units are ms/lights/min)
            uint8_t multiByte;          //multi-purpose byte

            // Function to initiate a moving light
                //param r0 = initial light position
                //param v0 = initial velocity (units are lights/min)
                //param a0 = initial acceleration (units are lights/min/s)
            void start(uint16_t r0, int32_t v0, int32_t a0) {
                //set the initial position
                r = r0;

                //set the multiByte info
                MOVINGLIGHT_SET_MULTI(multiByte, MOVINGLIGHT_MULTI_V_SIGN, v0 > 0 ? 1 : 0);
                MOVINGLIGHT_SET_MULTI(multiByte, MOVINGLIGHT_MULTI_V_STATIC, v0 == 0 ? 1 : 0);
                MOVINGLIGHT_SET_MULTI(multiByte, MOVINGLIGHT_MULTI_A_SIGN, a0 > 0 ? 1 : 0);
                MOVINGLIGHT_SET_MULTI(multiByte, MOVINGLIGHT_MULTI_V_STATIC, a0 == 0 ? 1 : 0);

                //Prepare vt based on current velocity
                vt = 60000 / constrain(abs(v0), 1, 2^16 - 1);

                //Prepare at based on current velocity
                at = 1000 / constrain(abs(a0), 1, 2^16 - 1);

                //Notes: this method won't work if the calling code has long blocking calls
                //  to get around that, we could maybe track a running error correction
                //  (i.e. - set a timer, track how long it actually was, then offset the next vt/at accordingly)
            };

            // Function to move the light
            void move() {
                EVERY_N_MILLIS_I(timeobj, t) {
                    //Update the position
                    r = constrain(r + MOVINGLIGHT_GET_MULTI(multiByte, MOVINGLIGHT_MULTI_V_SIGN) ? 1 : -1, 0, 2^16 - 1);

                    //set the next time to update the position based on the current velocity
                    v = constrain(r + MOVINGLIGHT_GET_SIGN(signs, MOVINGLIGHT_V_SIGN) ? 1 : -1, 0, 2^16 - 1);
                }
            };

            // Function to return the struct's sign byte
            uint8_t getSigns() {return signs;};
        } movingLight;
    */

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
