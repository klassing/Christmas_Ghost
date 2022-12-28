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
7. Once you're satisfied with your functions, please consider to submit a pull request to the repository so they can be added to the repo!
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
