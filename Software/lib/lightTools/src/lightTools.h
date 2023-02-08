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

    /* Define a template to return sign of signed integers*/
    //  graciously taken from https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
    //  returns 1 (positive), -1 (negative), or 0 (directly equals 0)
    template <typename T> int8_t sign(T val) {
        return (T(0) < val) - (val < T(0));
    }

    /* f_movingLight_t structure (uses floating point math) */
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
        //  Note: artificially limit calculations to once per ms, as this will save
        //      CPU cycles with minimal impact on the animation fluidity
        void move() {setR(); setV(); t=millis();};

        //Function to return the fractional portion of the position, scaled as an 8-bit unsigned int
        uint8_t getRscale8() {return (uint8_t)(255 * (r - (uint32_t)r));};

        //Function to return the current direction of travel
        //  return 1 = travelling forward (positive velocity)
        //  return 0 = not moving
        //  return -1 = travelling backward (negative velocity)
        //  Note: user can also just call sign(f_movingLight_t.v) directly :-)
        int8_t getVdir() {return sign(v);};
    } f_movingLight_t;

    /* - SECTION IN PROGRESS :: NOT READY FOR USE - */
    /*
        This structure is intended to provide a faster (less CPU cycles)
        method of physics based movement, at the cost of more RAM.

        This is intended to be accomplished through pure integer math.

        Currently, there are some issues with the timer based approach resulting
        in 'jerky' movement.  The intent of the timers is to reduce the computation
        overhead for cycles where the position wouldn't move anyway.  Until this
        is fixed, I DONT RECOMMEND USING --> USE THE FLOAT STRUCTURE ABOVE.
    */

        //Function to scale 'x' by scalar 'y' returning a 32b unsigned value
        //  param 'x' = unsigned value
        //  param 'y' = scalar
        #define MOVINGLIGHT_UNSIGNED_SCALE(x, y) (uint32_t)(x << y)

        //Function to unscale 'x' by scalar 'y' returning a 32b unsigned value
        //  param 'x' = unsigned value
        //  param 'y' = scalar
        #define MOVINGLIGHT_UNSIGNED_UNSCALE(x, y) (uint32_t)(x >> y)

        //Function to scale 'x' by scalar 'y' returning a 32b signed value
        //  param 'x' = signed value
        //  param 'y' = scalar
        #define MOVINGLIGHT_SIGNED_SCALE(x, y) (int32_t)((sign(x)) * ((uint32_t)abs(x) << y))

        //Function to unscale 'x' by scalar 'y' returning a 32b signed value
        //  param 'x' = signed value
        //  param 'y' = scalar
        #define MOVINGLIGHT_SIGNED_UNSCALE(x, y) (int32_t)((sign(x)) * ((uint32_t)abs(x) >> y))

        //r is bit-shifted up by this amount
        //  this is done to provide more granular control, without using floating point
        #define MOVINGLIGHT_R_SCALAR 8

        //Function to scale r
        #define MOVINGLIGHT_SCALE_R(x) MOVINGLIGHT_UNSIGNED_SCALE(x, MOVINGLIGHT_R_SCALAR)

        //Return an unscaled version of r
        #define MOVINGLIGHT_UNSCALE_R(x) MOVINGLIGHT_UNSIGNED_UNSCALE(x, MOVINGLIGHT_R_SCALAR)

        //v is bit-shifted up by this amount
        //  this is done to provide more granular control, without using floating point
        #define MOVINGLIGHT_V_SCALAR 8

        //Function to scale v
        //  Note: v is signed, so need to maintain sign, then shift the magnitude
        #define MOVINGLIGHT_SCALE_V(x) MOVINGLIGHT_SIGNED_SCALE(x, MOVINGLIGHT_V_SCALAR)

        //Return an unscaled version of v
        //  Note: v is signed, so need to maintain sign, then shift the magnitude
        #define MOVINGLIGHT_UNSCALE_V(x) MOVINGLIGHT_SIGNED_UNSCALE(x, MOVINGLIGHT_V_SCALAR)

        //v is set to lights-per-minute (60000ms per unit of movement)
        #define MOVINGLIGHT_V_UNIT 60000UL

        //a is set to lights-per-minute-per-second (1000ms per unit of movement)
        #define MOVINGLIGHT_A_UNIT 1000UL

        //Calculate the next interval for checking r
        //  This should occur in 60000 (ms/min) / MOVINGLIGHT_R_SCALAR / velocity (light/min)
        //  param 'x' = velocity value
        //  NOTE --> if 'x' = 0, this will force it to be 1 to prevent divide-by-zero
        //  NOTE --> this equation will currently add a 5ms offset (to force the math to run less often, saving CPU cycles)
        //      This isn't a great solution, but currently cuts the average cycle time by 50% and doesn't seem to impact animation
        #define MOVINGLIGHT_R_TIMER(x) (MOVINGLIGHT_V_UNIT / (1 << MOVINGLIGHT_R_SCALAR) / (abs(x) ? abs(x) : 1)) + 5

        //Calculate the next interval for checking v
        //  This should occur in 1000 (ms/s) / MOVINGLIGHT_V_SCALAR / acceleration (lights/min/s)
        //  param 'x' = acceleration value
        //  NOTE --> if 'x' = 0, this will force it to be 1 to prevent divide-by-zero
        //  NOTE --> this equation will currently add a 5ms offset (to force the math to run less often, saving CPU cycles)
        //      This isn't a great solution, but currently cuts the average cycle time by 50% and doesn't seem to impact animation
        #define MOVINGLIGHT_V_TIMER(x) (MOVINGLIGHT_A_UNIT / (1 << MOVINGLIGHT_V_SCALAR) / (abs(x) ? abs(x) : 1)) + 5

        //Calculate the next positional movement
        //  param 'x' = current velocity
        //  param 'y' = current positional real-time tracker
        #define MOVINGLIGHT_R_MOVE(x, y) ((MOVINGLIGHT_SIGNED_SCALE(MOVINGLIGHT_UNSCALE_V(x), MOVINGLIGHT_R_SCALAR) * (millis() - y)) / MOVINGLIGHT_V_UNIT)

        //Calculate the next positional movement
        //  param 'x' = current acceleration
        //  param 'y' = current velocity real-time tracker
        #define MOVINGLIGHT_V_MOVE(x, y) ((MOVINGLIGHT_SIGNED_SCALE(x, MOVINGLIGHT_V_SCALAR) * (millis() - y)) / MOVINGLIGHT_A_UNIT)

        //Create several definitions for the multi-purpose byte
        #define MOVINGLIGHT_MULTI_V_SIGN 0    //velocity sign bit (true = positive, false = negative)
        #define MOVINGLIGHT_MULTI_V_STATIC 1  //velocity static boolean (true = 0 velocity, false = velocity greater than 0)
        #define MOVINGLIGHT_MULTI_A_SIGN 2    //acceleration sign bit (true = positive, false = negative)
        #define MOVINGLIGHT_MULTI_A_STATIC 3  //acceleration static boolean (true = 0 velocity, false = velocity greater than 0)

        //Function to set one bit in the multi-byte
        //  param 'x' = multiByte variable (to be updated by this function)
        //  param 'loc' = location in the byte for this bit
        //  param 'val' = bit value to be set
        //  Example:  MOVINGLIGHT_SET_MULTI(multiByte, MOVINGLIGHT_MULTI_V_SIGN, 1);
        //#define MOVINGLIGHT_SET_MULTI(x, loc, val) x |= (value << loc)

        //Function to get one bit from the multi-byte
        //  param 'x' = multiByte variable (to do the lookup inside)
        //  param 'loc' = location in the byte for this bit
        //  Example:  desiredValue = MOVINGLIGHT_GET_MULTI(multiByte, MOVINGLIGHT_MULTI_V_SIGN);
        //#define MOVINGLIGHT_GET_MULTI(x, loc) ((x >> loc) & 0x1)

        //int_movingLight_t structure (uses only integer math)
        typedef struct {
            uint32_t r;                 //position      (units are (1/MOVINGLIGHT_R_SCALAR) of a light)
            int32_t v;                  //velocity      (units are lights/min)
            int16_t a;                  //acceleration  (units are lights/min/s)
            //uint8_t multiByte;          //multi-purposed byte for tracking signs if we end up using uints for v and a
            uint32_t rTimer;            //interval timer for when the position should actually move, based on velocity
            uint32_t rt;                //real-time caculator to properly determine positional movement based on time
            uint32_t vTimer;            //interval timer for when the velocity should actually move, based on acceleration
            uint32_t vt;                //real-time caculator to properly determine velocity movement based on time

            //Function to initialize a moving light
                //param r0 = initial position of the light ==> (full 16b range)
                //param v0 = initial velocity of the light (lights/min) ==> (15b range, since MSb is for the sign)
                //param a0 = initial acceleration of the light (lights/min/s) ==> (15b range, since MSb is for the sign)
            void start(uint16_t r0, int16_t v0, int16_t a0) {
                //set the multiByte info
                //MOVINGLIGHT_SET_MULTI(multiByte, MOVINGLIGHT_MULTI_V_SIGN, v0 >= 0 ? 1 : 0);
                //MOVINGLIGHT_SET_MULTI(multiByte, MOVINGLIGHT_MULTI_V_STATIC, v0 == 0 ? 1 : 0);
                //MOVINGLIGHT_SET_MULTI(multiByte, MOVINGLIGHT_MULTI_A_SIGN, a0 >= 0 ? 1 : 0);
                //MOVINGLIGHT_SET_MULTI(multiByte, MOVINGLIGHT_MULTI_A_STATIC, a0 == 0 ? 1 : 0);

                //set initial position, scaled up
                r = MOVINGLIGHT_SCALE_R(r0);

                //set initial velocity, scaled up
                v = MOVINGLIGHT_SCALE_V(v0);
                
                //set initial acceleration
                a = a0;

                //setup initial position timer
                rTimer = MOVINGLIGHT_R_TIMER(v0);

                //setup initial velocity timer
                vTimer = MOVINGLIGHT_V_TIMER(a0);

                //setup initial real-time trackers
                vt = rt = millis();
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
            uint16_t getR() {return MOVINGLIGHT_UNSCALE_R(r);};

            //Function to return light velocity
            int16_t getV() {return MOVINGLIGHT_UNSCALE_V(v);};

            //Function to move a light (updating position and velocity)
            void move() {setR(); setV();};
        } int_movingLight_t;

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
