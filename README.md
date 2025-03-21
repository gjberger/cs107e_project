## Project title

## Team members

## Project description

## Member contribution
A short description of the work performed by each member of the team.

Finley:
- Generally worked more on the hardware / hardware integration for the game
    - Set up, wiring, and reading from the mpu-6050 module
        - wrote mpu6050.c, mini library allowing configuration and use of the mpu-6050
        - interrupts used for sampling and the logic of determining the charcter position based on the board
    - Made the controller with the buttons
        - designed the main menu, top scores and charater select menu
        - implemented use of buttons to toggle / select options, choose charctaer, view top scores, restart
            the game
- random number generator based on Middle Square Weyl Sequence


Gabe:

## References
Cite any references/resources that inspired/influenced your project. 
If your submission incorporates external contributions such as adopting 
someone else's code or circuit into your project, be sure to clearly 
delineate what portion of the work was derivative and what portion is 
your original work.

For random number generator: https://en.wikipedia.org/wiki/Middle-square_method
    - used for general overview of middle square weyl sequence
Used Julie's I2C driver to communciate with mpu-6050 board. Built the mpu6050 module on top of the I2C driver

## Self-evaluation
How well was your team able to execute on the plan in your proposal?  
Any trying or heroic moments you would like to share? Of what are you particularly proud:
the effort you put into it? the end product? the process you followed?
what you learned along the way? Tell us about it!

## Photos
You are encouraged to submit photos/videos of your project in action. 
Add the files and commit to your project repository to include along with your submission.
