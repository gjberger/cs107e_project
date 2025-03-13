Game Structure:
1. Game start animation
- Welcome to CS107E surfers
    - powered by mango pi/risc-v (pink/yellow/black/mango logo)
2. Button to start game (wait for button press, 5 second delay until starting, screen countdown)
3. Character looks back and smiles
4. initialize game data
    - player position = middle
    - seenZero = true 
5. game structure
    - read gyroscope position using interrupts w/HSTIMER
        - +-1 is a move, bounded by -1 <= pos <= 1, have to reset to 0 to make another move
        (will handle in interrupt handler, and with boolean in character)
    - points dependent on time
    - 7 different barrier permutations
         - some kind of random number generator will select which one (might need to write one!)
        - an array of framebuffers will be intitialized with the sequence of the barriers
                - We will iterate through them to make it look like moving towards character
        - timing of the barrier sequence and the barriers will be determined by timers
    - the game will end if the character hits the barrier
        - there will be a field in the character struct which will we check against 
    - MAYBE: there will be a winning sequence if the player achieves a score of x


Week 2 Tasks
- when to have barriers come up
- button
- overall gameplay (start, counter, finisher)

Gabe
- Make character drawing dependent on coordinates
- character switching lane animations
- build barrier designs
- double buffer permutation idea, where character is constantly redrawn in buffers,
and so are moving barriers
- counter in top corner dependent on time
Finley
- interrupt handler
    - go back to 0 before being able to switch again
