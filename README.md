# 107e Surfers
107e Surfers is a simple skateboarding game for the Mango Pi inspired by the classic hit game, Subway Surfers. The player stands on a skateboard and moves between three lanes by tilting back and forth, which is read by a MPU-6050 gyroscope connected to the bottom of the skateboard. The game uses interrupts to constantly read the players position, and every 15 seconds obstacles spawn which the player must avoid. The player's score is the amount of time they stay alive, and at the end of the game the player's score is added to the leaderboard, depending on how long they were alive.

The graphics are controlled 

Features:
	•	Motion controls: Tilt the skateboard to switch lanes.
	•	Obstacles: Spawn every 15 seconds in random lanes.
	•	Collision detection: Hitting an obstacle ends the game.
	•	Menu: Use buttons to choose a character, start the game, or view high scores.
	•	Graphics: Renders characters, obstacles, and background directly to the screen.
	•	Scoring: Tracks and displays top scores.

Everything runs on bare metal—no OS, no external libraries.
