/** @file   paddle.h
    @author Kye Oldham (kno42) and Jack Ryan (jwr87) of ENCE260 Group 535
    @date   13 October 2020
    @brief  This is the interface for all the tasks associated with the
            paddle in a simple pong game
*/

#ifndef PADDLE_H
#define PADDLE_H

#include "system.h"
#include "navswitch.h"
#include "tinygl.h"

/*
 * Function: get_paddle_top
 * --------------------
 * Getter for the top of the paddle
 *
 * Returns: a tinygl_point struct for the top of the paddle, holding its
 * x and y co-ordinates
 *
*/
struct tinygl_point get_paddle_top(void);


/*
 * Function: get_paddle_bottom
 * --------------------
 * Getter for the bottom of the paddle
 *
 * Returns: a tinygl_point struct for the bottom of the paddle, holding its
 * x and y co-ordinates
 *
*/
struct tinygl_point get_paddle_bottom(void);


/*
 * Function: paddle_init
 * --------------------
 * Initializes the paddle
 *
*/
void paddle_init(void);


/*
 * Function: paddle_update
 * --------------------
 * Updates the paddle when the player moves the navswitch up or down
 *
*/
void paddle_update(void);

#endif

