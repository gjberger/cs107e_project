## Project title
CS107E Surfers

## Team members
Finley Desai, Gabe Berger

## Project description
Interactive subway surfer style game with dynamic graphics and skateboard-controller.

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
- Generally focused on software, game design logic, graphics, etc.
    - Designed all drawings, backgrounds, etc. + all movement, animation, etc.
    - Designed model-view-controller logic to drive the game (update screen)
        - I'm particularly proud of the module level representations for the character, barriers,
        and how we randomly generate the permutation of barriers that show up AND randomly generate
        which type of barrier comes at you. I also worked on the logic that allows the interplay between
        choosing your character, setting that value within the struct, etc.
    - Created interrupt to spawn barriers
    - Designed broader game logic + concepts, ie, 3 lanes, how the character dies, navigating around barriers,
    what it means to move (ie, must see zero first), how fast barriers come at you, permutation idea for barriers,
    random generation of permutations, etc.

## References
Cite any references/resources that inspired/influenced your project. 
If your submission incorporates external contributions such as adopting 
someone else's code or circuit into your project, be sure to clearly 
delineate what portion of the work was derivative and what portion is 
your original work.

Obviously, inspired by the hit game subway surfers.

For random number generator: https://en.wikipedia.org/wiki/Middle-square_method
    - used for general overview of middle square weyl sequence
Used Julie's I2C driver to communciate with mpu-6050 board. Built the mpu6050 module on top of the I2C driver

## Self-evaluation
How well was your team able to execute on the plan in your proposal?  
Any trying or heroic moments you would like to share? Of what are you particularly proud:
the effort you put into it? the end product? the process you followed?
what you learned along the way? Tell us about it!

I think it would have been hard for the game design process to go smoother than it did. To start off, Finley and I set very precise goals
with a very specific timeline. We also did the vast majority of the planning upfront, essentially implementing the entire game on the whiteboard
in Gates B02 to get the rough idea of how we'd code it, then doing just that.

Our development process was strongly helped by the fact that Finley and I have very complementary skills: namely, I am more into software,
and Finley is a bit more interested in hardware. Of course, I gave Finley ideas on how to implement a hardware paradigm, such as "seen_zero",
and he jumped in to help with the graphics because there was a lot to do on that front, but together the duo was totally dynamic.

I think one of the best moments was when we actually hooked up the gyroscope to the skateboard with all of our flushed out graphics. I asked
Finley to give me a working gyroscope pretty early on so I could work on graphics and movement, but we only hooked it up to the skateboard
later on. I was constantly amazed by Finley's hardware magic, and he was similarly appreciative of my graphics.

I am very proud of the drawings -- I think they're fun, and I like the idea of "dodging bugs" as if a programmer. I also like Mario, Steve, Luigi, and even stick, even if he's basic.
I like the loading screen, countdown, and RISC-V acknowledgement -- I think those are cornerstones of an arcade style videogame that I've always really appreciated.
As mentioned above, I also really like the broader game logic and how I managed the data and model using MVC.

Also, I learned a lot about graphics, how to create movement, how to manage framebuffers, etc. Initially, because of the number of permutations we had, Ben encouraged us to use like
20 framebuffers. I thought long and hard about this and realized that would needlessly complicate the process, and decided on MVC and 2 framebuffers and it worked really nicely. It was initially challenging
to figure out how to redraw the character, but constantly redrawing everything worked a treat.

I don't think we really had many complications as a result of our very solid planning. If anything, sometimes figuring out how to draw, drawing relatively on the screen, managing the barrier
movement, and using interrupts to spawn barriers was a bit odd, but I think all was smoothly implemented in the end.

Huge shoutout to Finley -- great partner!
## Photos
I've added a pic of the planning we did in advance, and a picture of Francis on the board,
and me on the board.
