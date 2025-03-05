## Title of your project

## Team members
Gabriel Berger and Finley Desai

## Project goals
Short description of project idea and end goal you are aiming for.
Include stretch goals and/or fallback position to indicate how you 
might fine-tune the project scope if needed.

We want to make an interactive subway surfers type game.

The user will stand on a skateboard modified to sense backward and forward movements.
We will do this with either a gyroscope, accelerometer, or pressure sensors.

We will sync the player's movements with graphical output mimicing the game Subway Surfers.

The graphical output will use multiple framebuffers to represent the various possible scenes.

We will draw on these and constantly redraw where the character is relative to the other objects.
Recall that the character moves in a forward motion in an alley with 3 lanes, moving back and forth
to avoid obstacles. If a player hits the obstacles, the game is over.

The base idea is simply to have the player move back and forth to switch lanes, but a stretch goal
would be being able to jump over obstacles.

A fallback would be to nix the jumping idea.

Recap:
Graphics:
- character at bottom of screen, constantly being redrawn as running. Potentially will animate as moving between lanes.
- relatively static background.
- barriers moving towards character, can constantly redraw those on the background.
Hardware:
- skateboard with gyroscope
- button to start game
Stretch goals:
- better graphics
   - character looks at you then runs
   - backpack
   - more detailed character
- jumping
- arches that you can run through

## Resources, budget
- Two skate boards (already ordered)
Gabe - $36.00
Finley - $27.27
- Gyroscope/Accelerometer
Finley has and will confirm functionality
- (optional) pressure senses (if gyroscope/accelerometer doesn't work)
- button to start game (and potentially jump)
- $14.18
## Tasks, member responsibilities
Outlining Tasks:
There are two major components to the project:
- Graphical component (Subway Surfers' visual output)
- Hardware (skateboard with sensors)
- Integration

Gabe will take more charge over the graphical component while Finley will take more charge
over the hardware. We will both work on both parts of the project to get exposure to each
side, and this will make integration easier. Further, we are going to try get the skateboard
done immediately to make integration further down the line easier.

## Schedule, midpoint milestones
Sketch a timeline for steady progress across full project period
Order tasks so as to resolve unknowns/risks sooner than later
What are the one-week goals you will complete by next lab meeting?

Timeline:
- Week 1
- Get as far as possible on skateboard using gyroscope
- Get rudimentary graphics system going
   - Establish background scene art
      - (potentially) shading
   - Character (various functions to redraw character in different running positions)
      - Circle for head (xiaolin wu)
      - antialiased lines (for now)
   - basic barrier designs (will have a function to draw each type of barrier)
      - to get barriers to constantly look like they are moving, we will constantly redraw them
        the smaller the barriers, the better because less redrawing (note, game ending condition
        will be if character is in same lane as barrier and y-coordinate intersects with characters
        y-coordinate)

- (ROUGH) Week 2
- build dynamic graphics
- broader control flow of game architecture
   - graphics library
   - game player
   - hardware driver
   - integration between hardware and software
## Additional resources, issues
Are there books/code/tools that you need access to?
- I2C or SPI driver

What are the possible risks to overcome? What are you concerned about? 
- graphics could potentially be very hard, but I think it is within our reach
How can we help you succeed?
- cheer us on!
