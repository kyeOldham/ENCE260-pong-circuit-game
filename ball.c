/** @file   ball.c
    @author Kye Oldham (kno42) and Jack Ryan (jwr87) of ENCE260 Group 535
    @date   13 October 2020
    @brief  This module handles all the tasks associated with the
            ball in the pong game.
*/


#include "system.h"
#include "ir_uart.h"
#include "tinygl.h"
#include "paddle.h"


#define INITIAL_BALL_X_POS 0
#define INITIAL_BALL_Y_POS 3

#define X_TOWARDS_PADDLE -1
#define X_AWAY_FROM_PADDLE 1
#define Y_TOWARDS_TOP 1
#define Y_STRAIGHT 0
#define Y_TOWARDS_BOTTOM -1


#define GOAL_X 4
#define ONE_FROM_GOAL 3

#define BORDER_X 0
#define TOP_WALL_Y 6
#define BOTTOM_WALL_Y 0


static int ball_dir_x; // Int for the balls x direction
static int ball_dir_y; // Int for the balls y direction
static tinygl_point_t ball; // Tinygl_point_t for the ball (its x and y co-ords)


/*
 * Function: set_ball_position
 * --------------------
 * Initialization of the balls position and directon
 *
 * int x: x-coordinate to be set
 * int y: y-coordinate to be set
 *
*/
void set_ball_position(int x, int y)
{
    ball.x = x;
    ball.y = y;
    //The balls x direction is initially towards the starting player
    ball_dir_x = X_TOWARDS_PADDLE;
    //The ball is initially travelling a straight line
    ball_dir_y = Y_STRAIGHT;
}


/*
 * Function: get_ball
 * --------------------
 * Getter for the ball
 *
 * Returns: a tinygl_point struct for the ball, holding the balls x
 * and y co-ordinates
 *
*/
struct tinygl_point get_ball(void)
{
    return ball;
}


/*
 * Function: send_ball_position
 * --------------------
 * Sends the position of the ball to the other player when the border
 * has been crossed via ir_uart serial communications
 *
 * uint8_t their_score: The score for the other player to be sent
 */
void send_ball_position(uint8_t their_score)
{
    int dir_y_to_send = -ball_dir_y;
    // Compresses y_pos and score into a single transmission, to reduce
    // the load on the ir transmitter using bitshifting
    uint8_t y_pos_and_score_to_send = (TOP_WALL_Y - ball.y ) << 4;
    y_pos_and_score_to_send = (0b00001111 & their_score) | y_pos_and_score_to_send;
    //Send 8bit number representing y_pos and score
    ir_uart_putc(y_pos_and_score_to_send);
    //Send y direction to other player
    ir_uart_putc(dir_y_to_send);
}


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
int get_ball_position(void)
{
    int my_score = 0;
    if (ir_uart_read_ready_p()) {
        uint8_t y_pos_and_score = (int) ir_uart_getc();
        ball_dir_y = (int) ir_uart_getc();
        // Performing reverse bitshifting operations to get score and y_pos
        ball.y = (y_pos_and_score >> 4);
        my_score = (y_pos_and_score) & 0b00001111;
        // Set initial x co-ord and direction
        ball.x = INITIAL_BALL_X_POS;
        ball_dir_x = X_TOWARDS_PADDLE;
    }
    return my_score;
}


/*
 * Function: reset_ball
 * --------------------
 * Resets the balls position and direction when the current game has
 * finished, so the next game can begin.
*/
void reset_ball(void)
{
    ball.x = INITIAL_BALL_X_POS;
    ball.y = INITIAL_BALL_Y_POS;
    ball_dir_x = X_TOWARDS_PADDLE;
    ball_dir_y = Y_STRAIGHT;
}


/*
 * Function: is_bouncing_off_paddle
 * --------------------
 * Determines whether the ball is bouncing of the players paddle, and
 * decides in which y direction the ball should rebound based on where
 * the ball came into contact with the paddle
 *
 * Returns: an int, 1 if the ball is bouncing of the paddle, 0 otherwise
*/
static int is_bouncing_off_paddle(void)
{
    //Get current paddle position
    int paddle_top_y = get_paddle_top().y;
    int paddle_bottom_y = get_paddle_bottom().y;
    if(ball.x == ONE_FROM_GOAL && ball_dir_x == X_TOWARDS_PADDLE && ball.y >= paddle_bottom_y && ball.y <= paddle_top_y) {
        if(ball.y == paddle_bottom_y) {
            ball_dir_y = Y_TOWARDS_BOTTOM;
        } else if (ball.y == paddle_top_y) {
            ball_dir_y = Y_TOWARDS_TOP;
        } else {
            ball_dir_y = Y_STRAIGHT;
        }
        return 1;
    } else {
        return 0;
    }
}


/*
 * Function: is_bouncing_off_wall
 * --------------------
 * Determines whether the ball is bouncing of the wall of the playing
 * grid
 *
 * Returns: an int to_return, 1 if the ball is bouncing of the wall,
 * 0 otherwise
*/
static int is_bouncing_off_wall(void)
{
    int to_return = 0;
    if(ball.y == TOP_WALL_Y && ball_dir_y == Y_TOWARDS_TOP) {
        to_return = 1;
    } else if(ball.y == BOTTOM_WALL_Y && ball_dir_y == Y_TOWARDS_BOTTOM) {
        to_return = 1;
    }
    return to_return;
}


/*
 * Function: is_crossing_border
 * --------------------
 * Determines whether the ball is crossing the grid border
 *
 * Returns: an int, 1 if the ball is crossing the border, 0 otherwise
*/
static int is_crossing_border(void)
{
    return (ball.x == BORDER_X && ball_dir_x == X_AWAY_FROM_PADDLE);
}


/*
 * Function: is_moving_away
 * --------------------
 * Determines whether the ball is moving away from the paddle
 *
 * Returns: an int, 1 if the ball is moving away from the paddle, 0
 * otherwise
*/
static int is_moving_away(void)
{
    return (ball_dir_x == X_AWAY_FROM_PADDLE);
}


/*
 * Function: scored_against
 * --------------------
 * Determines whether the player has been scored against
 *
 * Returns: an int, 1 if the player has been scored against, 0 otherwise
*/
static int scored_against(void)
{
    return (ball.x == GOAL_X);
}


/*
 * Function: move_ball
 * --------------------
 * Moves the ball position with its current x and y directions
 *
*/
static void move_ball(void)
{
    // Changing the x position of the ball
    if (is_moving_away()) {
        ball.x--;
    } else {
        ball.x++;
    }
    // Changing the y position of the ball
    switch (ball_dir_y) {
    case Y_TOWARDS_TOP:
        ball.y++;
        break;
    case Y_TOWARDS_BOTTOM:
        ball.y--;
        break;
    }
}


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
int update_position(void)
{
    int to_return = 0;
    if (scored_against()) {
        to_return = 1;
    } else {
        if (is_bouncing_off_paddle()) {
            ball_dir_x = X_AWAY_FROM_PADDLE;
        }
        if (is_bouncing_off_wall()) {
            //Reverse y direction of ball
            ball_dir_y = - ball_dir_y;
        }
        if (is_crossing_border()) {
            if (is_bouncing_off_wall()) {
                ball_dir_y = - ball_dir_y;
            }
            to_return = -1;
        } else {
            move_ball();
        }
    }
    return to_return;
}
