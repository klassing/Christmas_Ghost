
/* Some global variables for facilitating the red_and_green_curtain_lights_to_middle() function */
uint8_t curtain_UpCounter = LED_PER_START_POS - 1;
uint8_t curtain_DownCounter = LED_QTY_USED;
bool curtain_LightToggle = false;

/* Some global variables for facilitating the stack_lights_in_the_middle() function */
uint8_t stack_UpCounter = LED_PER_START_POS - 1;
uint8_t stack_DownCounter = LED_QTY_USED;
uint8_t stack_LightStop = LED_PER_MID_POS - 1;
uint32_t stack_color_pattern[] = {CRGB::Red, CRGB::White, CRGB::Green, CRGB::Gold};
uint8_t stack_pattern_count = 0;
bool stack_LightToggle = false;

void red_and_green_curtain_lights_to_middle();                                  /* RED and GREEN "curtain" lights close from left and right and meet in the middle of the LED string */
void stack_lights_in_the_middle();

/* RED and GREEN "curtains" lights close from left and right and meet in the middle of the LED string */
void red_and_green_curtain_lights_to_middle() {
    // Every 25ms we'll update the LED pattern
    EVERY_N_MILLISECONDS( 25 ) { 
        if( curtain_UpCounter < LED_PER_MID_POS && curtain_LightToggle == false ) {
            LED_ARR[ curtain_UpCounter++ ] = CRGB::Red;       // As long as the LED upCounter has not reached the middle of the LED string increment and turn the next light in the array RED 
            LED_ARR[ curtain_DownCounter--] = CRGB::Red;      // Same condition as above but this walks in our LED pattern from the "farside" of the LED string in toward the middle
        }
        else if( curtain_UpCounter >= LED_PER_MID_POS && curtain_LightToggle == false ){
            curtain_UpCounter = LED_PER_START_POS - 1;      // After reaching the middle we now want to start the pattern over so we send the counters back to the "far ends" of the LED string
            curtain_DownCounter = LED_QTY_USED;             
            curtain_LightToggle = true;                     // As we don't want to repeat the pattern with RED lights anymore (wouldn't be visible) we need to toggle our 'LightToggle' variable to 'change states'
        }
        else if( curtain_UpCounter < LED_PER_MID_POS && curtain_LightToggle == true){
            LED_ARR[ curtain_UpCounter++ ] = CRGB::Green;   //As long as 
            LED_ARR[ curtain_DownCounter--] = CRGB::Green;
        }
        else{
            curtain_UpCounter = LED_PER_START_POS - 1;
            curtain_DownCounter = LED_QTY_USED;
            curtain_LightToggle = false;
        }
    }
}

void stack_lights_in_the_middle(){
    
    EVERY_N_MILLISECONDS(15){
        if( stack_LightToggle == false ){
            if( stack_UpCounter < stack_LightStop ){
                LED_ARR[ stack_UpCounter ] = CRGB::Black;
                LED_ARR[ stack_DownCounter ] = CRGB::Black;
                stack_UpCounter++;
                stack_DownCounter--;
                LED_ARR[ stack_UpCounter ] = stack_color_pattern[stack_pattern_count];
                LED_ARR[ stack_DownCounter ] = stack_color_pattern[stack_pattern_count];
            }
            else if( stack_UpCounter >= stack_LightStop ){
                stack_UpCounter = LED_PER_START_POS - 1;
                stack_DownCounter = LED_QTY_USED;
                stack_pattern_count++;
                stack_pattern_count = stack_pattern_count % 4;
                stack_LightStop--;
            }
            if( stack_LightStop == 0 ){
                stack_LightToggle = true;
            }
        }
        if( stack_LightToggle == true ){
            fadeToBlackBy(LED_ARR + LED_PER_START_POS, LED_QTY_USED - LED_PER_START_POS, 255);
            stack_LightStop = LED_PER_MID_POS - 1;
            stack_UpCounter = LED_PER_START_POS - 1;
            stack_DownCounter = LED_QTY_USED;
            stack_LightToggle = false;
        }
    }
}
