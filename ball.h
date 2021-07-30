/** @file   ball.h
    @author Kye Oldham (kno42) and Jack Ryan (jwr87) of ENCE260 Group 535
    @date   13 October 2020
    @brief  This is the interface for all the tasks associated with the
            ball in the pong game.
*/

#ifndef BALL_H
#define BALL_H

#include "system.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "paddle.h"

/*
 * Function: set_ball_position
 * --------------------
 * Initialization of the balls position and directon
 *
 * int x: x-coordinate to be set
 * int y: y-coordinate to be set
 *
*/
void set_ball_position(int x, int y);


/*
 * Function: get_ball
 * --------------------
 * Getter for the ball
 *
 * Returns: a tinygl_point struct for the ball, holding the balls x
 * and y co-ordinates
 *
*/
struct tinygl_point get_ball(void);


/*
 * Function: send_ball_position
 * --------------------
 * Sends the position of the ball to the other player when the border
 * has been crossed via ir_uart serial communications
 *
 * uint8_t their_score: The score for the other player to be sent
 */
void send_ball_position(int their_score);


/*
 * Function: get_ball_position
 * --------------------
 * Gets the position and directon for the ball along with this players
 * score from the opposing player via ir_uart serial communications.
 * The balls x co-ordinates, y co-ordinates, x direction are y direction
 * are then set.
 *
 * Returns: an int my_score representing this players score
*/
int get_ball_position(void);


/*
 * Function: reset_ball
 * --------------------
 * Resets the balls position and direction when the current game has
 * finished, so the next game can begin.
*/
void reset_ball(void);


/*
 * Function: update_position
 * --------------------
 * Updates the position of the ball by checking whether: The ball has
 * gone past the ball and the players has been scored against, the ball
 * is bouncing off the players paddle, the ball is bouncing off the,
 * wall or the ball is crossing the border. Moves the ball using the
 * move_ball() function if the border has not been crossed
 *
 * Returns: int to_return, 1 if the player has been scored against, -1 if
 * the player is crossing the border and 0 otherwise. This is so the
 * associated functionality in the game.c file can be updated if either
 * of these scenarios occur.
*/
int update_position(void);

#endif
