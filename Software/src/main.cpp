
/* 
    Author: Ryan Klassing 
    Date: 11/29/22
    Version: */ #define BLYNK_FIRMWARE_VERSION "0.0.40" /*
    Description:
        This is intended to run a small ESP32 based PCB that looks
        like a ghost, for simple/fun Christmas decotrations.  The
        ESP32 controls 2x "eye" LEDs (for the ghost's eyes) as well
        as an additional 8x "ambiance" LEDs (for glowing effects) around
        the ghost.  The board also contains USB Type C (for programming / debugging),
        and can be operated from a battery range of ~1.6V (startup, then can operate down to ~1.0V)
        up to ~5.2V.  This is heavily based on the "Halloween Ghost" project, but will expand the
        lighting control offboard as well (the lighting will be an array driven from the end of the
        on-board "ambiance" lighting, and will be manually soldered on to the external lighting cable).

    License: see "LICENSE" file

    12/21/2022 - Cochise Push
    Description:
        Added light patterns "red_and_green_curtain_lights_to_middle" and "stack_lights_in_the_middle"
        in "Cochise.h" file.  The main.cpp code was modified to call out these light patterns in the
        christmas_patterns function list and to add #include of the Cochise.h file just below "[END]
        HW Configuration Setup" section.  This placement ensures that #defines used by the functions
        have been completed before Cochise.h inclusion. Lastly, modified LED_PER_START_POS from 2 to 10
        and added LED_PER_MID_POS 60 which marks the mid-point of the LED string offset by the start
        position.

*/

/* ------------ [START] Early definitions for Blynk -------------- */
    #define BLYNK_TEMPLATE_ID "TMPLmXb3Shr8"
    #define BLYNK_DEVICE_NAME "ChristmasGhost"
    #define BLYNK_PRINT Serial
/* ------------   [End] Early definitions for Blynk -------------- */


/* ------------ [START] Include necessary libraries -------------- */
    #include <FastLED.h>        // Tested with v3.5.0 - https://github.com/FastLED/FastLED.git#3.5.0
    #include <WiFi.h>           // Included in ESP32 Arduino Core - Tested with v2.0.5 - https://github.com/platformio/platform-espressif32.git#v5.2.0
    #include <esp_bt.h>         // Included in ESP32 Arduino Core - Tested with v2.0.5 - https://github.com/platformio/platform-espressif32.git#v5.2.0
    #include <WiFiAP.h>         // Included in ESP32 Arduino Core - Tested with v2.0.5 - https://github.com/platformio/platform-espressif32.git#v5.2.0
    #include <WebServer.h>      // Included in ESP32 ARduino Core - Tested with v2.0.5 - https://github.com/platformio/platform-espressif32.git#v5.2.0
    #include <EEPROM.h>         // Included in ESP32 ARduino Core - Tested with v2.0.5 - https://github.com/platformio/platform-espressif32.git#v5.2.0
    #include <ESP2SOTA.h>       // Tested with v1.0.2 - https://github.com/pangodream/ESP2SOTA.git#1.0.2
    #include <Button2.h>        // Tested with v2.0.3 - https://github.com/LennartHennigs/Button2.git#2.0.3
    #include <BlynkEdgent.h>    // Tested with v1.1.0 - https://github.com/blynkkk/blynk-library.git#1.1.0
    #include <klassyLights.h>   // Light function library by Ryan K.
    #include <cochise.h>        // Light function library by Cochise F.
/* -------------- [END] Include necessary libraries -------------- */

/* ------------ [START] HW Configuration Setup -------------- */
    /* Pin Configurations */
    #define LED_PWR_EN_PIN 25
    #define LED_DATA_PIN 26
    #define LEFT_TOUCH_PIN 15
    #define RIGHT_TOUCH_PIN 14

    /* LED Configurations */
    #define LED_TYPE WS2811
    #define LED_COLOR_ORDER RGB
    #define LED_ARR_QTY 200         //TODO --> figure out why the lights are flickering when defining the array to be the actual size of the lights (this works fine on other projects....)
    #define LED_STRAND_QTY 100      //Actual QTY of lights in the strands --> USE THIS FOR LIGHT FUNCTIONS
    #define LED_PER_START_POS 10    //Starting array position for the peripheral LEDs
    #define LED_MAX_BRIGHTNESS 255  //Maximum allowed brightness for the LEDs
    CRGB LED_ARR[LED_ARR_QTY];      //global LED array

/* -------------- [END] HW Configuration Setup -------------- */

/* ------------ [START] Debug compile options -------------- */
    #define LOG_DEBUG true          //true = logging printed to terminal, false = no logging
/* -------------- [END] Debug compile options -------------- */

/* ------------ [START] Serial Terminal Configuration -------------- */
    #define SERIAL_BAUD 115200
/* -------------- [END] Serial Terminal Configuration -------------- */

/* ---------- [START] Button Configuration -------------- */
    Button2 left_hand_btn;          //Button for pressing the ghost's left hand
    Button2 right_hand_btn;         //Button for pressing the ghost's right hand
/* ------------ [End] Button Configuration -------------- */

/* ------------ [START] Define Function Prototypes -------------- */
    /* General Protoypes */
    void pin_config();                          //Function to initialize HW config
    void print_welcome_message();               //Function to print a welcome message with the SW version
    void next_pattern();                        //Cycle through the pattern list periodically, wrapping around once reaching the end of the array

    /* Power Management Prototypes */
    void disableWiFi();                                             //Function to disable WiFi for power savings
    void disableBT();                                               //Function to disable BT for power savings

    /* LED Management Prototypes */
    void led_handler();                         //Handler function to execute various LED management tasks

    /* Input Button Management Prototypes */
    void button_handler();                      //Handler function to execute various input button management tasks
    void init_buttons();                         //Function to initialize the button configurations
    void button_left_click(Button2& btn);     //Callback function to be executed when left is pressed/held
    void button_right_click(Button2& btn);      //Callback function to be executed when right is clicked

    /* Debug / Printing Prototypes */
    void print(String message);                 // Function to print a message */
    void println(String message);               // Function to print a message, with CRLF
    void time_print(String message);            //Function to pre-pend a message with the current CPU running timestamp
    void time_println(String message);          //Function to pre-pend a message with the current CPU running timestamp, with CRLF
    void log(String message);                   //Function to log debug messages during development
    void logln(String message);                 //Function to log debug messages during development, with CRLF
    void time_log(String message);              //Function to log debug messages during development, prepended with a timestamp
    void time_logln(String message);            //Function to log debug messages during development, prepended with a timestamp, with CRLF

/* -------------- [END] Define Function Prototypes -------------- */

 /* ----------- [START] Construct all User Light Libraries ------------- */
    lightTools lightTools;  //Common lightTools member, to be used by any user classes
    klassyLights klassyLights(&LED_ARR[LED_PER_START_POS], LED_STRAND_QTY, &lightTools);
    cochise cochise(&LED_ARR[LED_PER_START_POS], LED_STRAND_QTY, &lightTools);
 /* ------------- [END] Construct all User Light Libraries ------------- */

/* ------------ [START] Define Pattern List -------------- */

    /* Define an array of functions to cycle through - each function added will be called periodically to run the lights */
    typedef void (*FunctionList[])();

    /* Update this array whenever new functions need to be added, and the led_handler will automatically loop through them */
    FunctionList christmas_patterns = {cochise.stack_lights_in_the_middle, cochise.red_and_green_curtain_lights_to_middle, klassyLights.fading_candy_cane};

    /* function list index to loop through the patterns */
    uint8_t christmas_patterns_idx = 0;

    /* update this to set the duration (in seconds) of each pattern (how long it will run before moving to the next pattern) */
    #define PATTERN_DURATION 60

    /* Macro to calculate array sizes */
    #define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* -------------- [END] Define Pattern List -------------- */

void setup() {
    /* Initialize the Serial Terminal */
    Serial.begin(SERIAL_BAUD);

    /* Set WiFi to automatic sleep to reduce power */
    WiFi.setSleep(true);

    /* Disable BT to reduce power */
    disableBT();

    /* Initialize the HW pins / buttons */
    pin_config();
    init_buttons();

    /* Print Welcome Message */
    print_welcome_message();

    /* Create LED array / Set master brightness */
    FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_COLOR_ORDER>(LED_ARR, LED_ARR_QTY).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(LED_MAX_BRIGHTNESS);

    /* Initiate Blynk Edgent */
    BlynkEdgent.begin();
}

void loop() {
    /* Handle LED tasks */
    led_handler();

    /* Handle input tasks */
    button_handler();

    /* Handle BlynkEdgent */
    BlynkEdgent.run();

    /* Delay a small amount to pet the watchdog */
    delayMicroseconds(1);
}

/* Function to initialize HW config */
void pin_config() {
    /* Set the pin modes (output/input) */
    pinMode(LED_PWR_EN_PIN, OUTPUT);

    /* Set the default state for output pins */
    digitalWrite(LED_PWR_EN_PIN, HIGH);
}

/* Function to print a welcome message with the SW version */
void print_welcome_message() {
    time_println("***************************");
    time_println("***** Christmas Ghost *****");
    time_print("*****    SW: v"); print(BLYNK_FIRMWARE_VERSION); println("   *****");
    time_println("***************************");
}

/* Function to disable WiFi for power savings */
void disableWiFi() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
}

/* Function to disable BT for power savings */
void disableBT() {
    btStop();
}

/* Handler function to execute various LED management tasks */
void led_handler() {

    /* Cycle through the pattern list periodically, wrapping around once reaching the end of the array */
    EVERY_N_SECONDS(PATTERN_DURATION) {next_pattern();}

    /* Run the currently selected pattern */
    christmas_patterns[christmas_patterns_idx]();

    /* push LED data */
    FastLED.show();
}

/* Cycle through the pattern list periodically, wrapping around once reaching the end of the array */
void next_pattern() {
    christmas_patterns_idx = (christmas_patterns_idx + 1) % ARRAY_SIZE(christmas_patterns);
    time_logln("Moving to next pattern index: " + String(christmas_patterns_idx, DEC));
}

/* Handler function to execute various input button management tasks */
void button_handler() {
    left_hand_btn.loop();
    right_hand_btn.loop();

    /* Temporary work around since the button callbacks aren't working properly */
    if (left_hand_btn.isPressed() || right_hand_btn.isPressed()) {EVERY_N_SECONDS(2) {next_pattern();}}
}

/* Function to initialize the button configurations */
void init_buttons() {
    /* Configure Left Hand */
    left_hand_btn.begin(LEFT_TOUCH_PIN, INPUT, false, false);       //(pin, mode, isCapacitive, activeLow)
    //left_hand_btn.setDebounceTime(1000);
    //left_hand_btn.setClickHandler(button_left_click);
    //left_hand_btn.setLongClickTime(5000);
    //left_hand_btn.setLongClickHandler(button_left_long_click);

    /* Configure Right Hand */
    right_hand_btn.begin(RIGHT_TOUCH_PIN, INPUT, false, false);       //(pin, mode, isCapacitive, activeLow)
    //right_hand_btn.setDebounceTime(1000);
    //right_hand_btn.setClickHandler(button_right_click);
}

/* Callback function to be executed when left is clicked */
void button_left_click(Button2& btn) {
    /* Increment the pattern index */
    next_pattern();
}

/* Callback function to be executed when right is clicked */
void button_right_click(Button2& btn) {
    /* Increment the pattern index */
    next_pattern();
}

/* Function to print a message */
void print(String message) {
    Serial.print(message);
}

/* Function to print a message, with CRLF */
void println(String message) {
    print(message + "\r\n");
}

/* Function to pre-pend a message with the current CPU running timestamp */
void time_print(String message) {
    String timed_message = "[" + String((millis()), DEC) + "] " + message;
    Serial.print(timed_message);
}

/* Function to pre-pend a message with the current CPU running timestamp, with CRLF */
void time_println(String message) {
    time_print(message + "\r\n");
}

/* Function to log debug messages during development */
void log(String message) {
    if(LOG_DEBUG) {print(message);}
}

/* Function to log debug messages during development, with CRLF */
void logln(String message) {
    log(message + "\r\n");
}

/* Function to log debug messages during development, prepended with a timestamp */
void time_log(String message) {
    if(LOG_DEBUG) {time_print(message);}
}

/* Function to log debug messages during development, prepended with a timestamp, with CRLF */
void time_logln(String message) {
    time_log(message + "\r\n");
}