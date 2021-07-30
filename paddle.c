/** @file   paddle.c
    @author Kye Oldham (kno42) and Jack Ryan (jwr87) of ENCE260 Group 535
    @date   13 October 2020
    @brief  This module handles all the tasks associated with the
            paddle in a simple pong game
*/


#include "system.h"
#include "navswitch.h"
#include "tinygl.h"


#define PADDLE_X 4
#define PADDLE_TOP_INIT_Y 4
#define PADDLE_BOTTOM_INIT_Y 2

#define TOP_WALL_Y 6
#define BOTTOM_WALL_Y 0


static tinygl_point_t paddle_top; //Tinygl_point_t for the top of the paddle
static tinygl_point_t paddle_bottom; //Tinygl_point_t for the bottom of the paddle


/*
 * Function: get_paddle_top
 * --------------------
 * Getter for the top of the paddle
 *
 * Returns: a tinygl_point struct for the top of the paddle, holding its
 * x and y co-ordinates
 *
*/
struct tinygl_point get_paddle_top(void)
{
    return paddle_top;
}


/*
 * Function: get_paddle_bottom
 * --------------------
 * Getter for the bottom of the paddle
 *
 * Returns: a tinygl_point struct for the bottom of the paddle, holding its
 * x and y co-ordinates
 *
*/
struct tinygl_point get_paddle_bottom(void)
{
    return paddle_bottom;
}


/*
 * Function: paddle_init
 * --------------------
 * Initializes the paddle
 *
*/
void paddle_init(void)
{
    paddle_bottom.x = PADDLE_X;
    paddle_bottom.y = PADDLE_BOTTOM_INIT_Y;
    paddle_top.x = PADDLE_X;
    paddle_top.y = PADDLE_TOP_INIT_Y;
}


/*
 * Function: paddle_update
 * --------------------
 * Updates the paddle when the player moves the navswitch up or down
 *
*/
void paddle_update(void)
{
    if (navswitch_push_event_p (NAVSWITCH_NORTH)) {
        if(paddle_bottom.y > BOTTOM_WALL_Y) {
            paddle_bottom.y--;
            paddle_top.y--;
        }
    } else if (navswitch_push_event_p (NAVSWITCH_SOUTH)) {
        if(paddle_top.y < TOP_WALL_Y) {
            paddle_bottom.y++;
            paddle_top.y++;
        }
    }
}
