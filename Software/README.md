# Christmas Ghost SW

## Description
This project is meant to spread the holiday cheer with a bit of lighting.

## How to Contribute
1. Clone this repository locally
2. If you already have a 'user class' created, proceed with #4 below.  Otherwise, proceed with #3 below.
3. To create a 'user class' (container for your light functions, to prevent naming conflicts with other users):
    - Execute the 'Add_New_User-lib.vbs' tool by double clicking, and providing a class-name to the script
    - This tool can be found here:  
        [..\Software\tools\Add_New_User-lib.vbs](tools)
4. Within your 'user class', add as many light functions as you'd like (please see the 'Light Function Guidelines' section below)
5. After finishing your work in your 'user class', don't forget to add an instance of your class (if not already done) to the main.cpp file  
    ~~~
    /* Example: */  
    klassyLights klassLights(&LED_ARR[LED_PER_START_POS], LED_STRAND_QTY, &lightTools);
    ~~~
6. After adding an instance of your class, you can add your function calls to the `christmas_patterns` function array 
    ~~~
    /* Example: */  
    FunctionList christmas_patterns = {klassyLights.fading_candy_cane, ...};
    ~~~
7. When ready to test your code, perform the following checks:
    - Verify the code compiles without errors or custom dependencies on your local machine (that aren't part of the tracked repo)
    - Verify the lighting function performs as expected by using the [Online Simulator](https://wokwi.com/projects/352480708315963393)  
        - Before uploading code to the Online Simulator, please run the script at the following location:  
            [..\Software\tools\Create_wokwi_sim.vbs](tools)
        - The script will pre-compile (stitch all necessary libraries into a single file) and place onto your clipboard
        - Paste the contents into the simulator linked above, and click "run"
8. Push the software wirelessly to the board by using Blynk:
    - Navigate to [Blynk.cloud](https://blynk.cloud/)
    - Login with the group credentials (if you don't know them, reach out to Ryan Klassing)
    - Navigate to the "Devices" tab and select the "ChristmasGhost" device
    - Select the "..." to list additional actions, and then select "Update Firmware" (with a symbol that looks like a paper airplane)
    - Take note of the current firmware version on the device.  In order to push firmware to the board, your code must have a numerically higher version number
        - Example: current version = 0.0.43 , then your firmware update should be defined as 0.0.44 (in main.cpp: #define BLYNK_FIRMWARE_VERSION "0.0.44")
    - Compile your code and find the resulting .bin file (this should be located at /REPO/Software/.pio/build/esp32doit-devkit-v1/firmware.bin)
    - Upload the .bin file to the Blynk updater page and click "Start Shipping"
9. Once you're satisfied with your functions running in the simulator and on real hardware, please consider to submit a pull request to the repository so they can be added to the repo!
    - See [Making a Pull Request](https://docs.github.com/en/get-started/quickstart/contributing-to-projects#making-a-pull-request) for assistance

## Light Function Guidelines
To allow several users to wrap their functions inside classes, and then to allow a common main.cpp file to execute a generic array of functions, there are some
guidelines that need to be followed when creating the lighting functions within a 'user class'.

- The function must defined within the class below (to prevent naming conflicts with other users)
- The public class-function must be public
- The public class-function must be of the type 'void' (i.e. - it must not return any value)
- The public class-function must not take any parameters as an input
- The public class-function must be static

For detailed examples, please see [klassyLights](lib/klassyLights/src/)

## Blynk Troubleshooting
Occasionally, some issues might arise while using the Blynk services.  Below are a few examples of issues that have been seen, and how to resolve them:
1. OTA update is not working
    - Since the current account is free, there have occasionally been issues with number of OTA uploads per hour (maybe even per day).
    - If several updates have been pushed recently, please simply try waiting several hours and see if this resolves the issue.
        - As a note, the online simulator (see step 7 in "How to contribute" above) is an extremely useful tool to troubleshoot light algorithms without needing to push the software to the real hardware
2. Hardware is unable to connect to Blynk services
    - The Blynk software is setup to automatically authenticate, which prevents the need to store WiFi credentials or authenticator tokens in the source code
    - To reconnect the hardware and re-authenticate:
        1. Download the Blynk Cloud app on an iPhone or Android device
        2. Open the app and select "Add New Device".  This should trigger the app to scan locally (using BT) for a compatible hardware.  Please make sure you are located within 15ft of the hardware.
            - If no device is found, please try to manually reflash the device natively with PlatformIO and a direct USB connection to completely reset the device, then repeat step 2.
        3. Once connected, it should prompt you to enter the correct WiFi SSID and password, which will then be stored to EEPROM on the hardware itself
        4. The device should now be connected to the Blynk server and trigger a new authentication automatically

## Author(s)
- Author(s): Ryan Klassing
- Copyright (C) 2017-2022 Ryan Klassing.
- Released under the MIT license.

## License

MIT License

Copyright (c) 2017-2022 Ryan Klassing

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
