# Pong

Pong is a simple two player game designed to be run on the UC Fun kit 4.

## Compilation

In order to compile the given code to the UC Fun kit4, ensure you are in the .../assignment/group-535/ directory and that the UC Fun kit 4 is plugged in and on (Indicated by the green light). Once here, open terminal and run the command:

```bash
sudo make program
```

If the below error message is received, press the reset button (S2) on the UC Fun kit 4 and retry.

```bash
dfu-programmer: no device present.
dfu-programmer: no device present.
dfu-programmer: no device present.
make: *** [Makefile:94: program] Error 1

```

Note: This process will need to be done for both microcontrollers being used for this game.
## Playing the game

For the best experience, ensure the game is played somewhere with no/minimal IR interferance.
In order to begin, point the IR receivers on each microcontroller at each other and press down the navswitch. This should display some introductory text on the led mat. Ensure that the microcontrollers remain pointed at each other throughout the duration of the game.

To start the game, one player should press the navswitch down again. The game will then play out as a pong game is expected, the player can move their paddle from left to right by moving the navswitch in the corresponding direction. A point is scored if a player lets the bouncing ball pass by their paddle.

Once a player reaches 3 points, the game will end, with each players score being displayed on their screen.

## Contributors
Kye Oldham, Jack Ryan
