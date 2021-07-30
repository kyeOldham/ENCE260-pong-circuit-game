/**
 * @file game.c
 * @author Kye Oldham (kno42) and Jack Ryan (jwr87) of ENCE260 Group 535
 * @date 13 October 2020
 * @brief Main module of a simple pong game
 */


#include "system.h"
#include "navswitch.h"
#include "pio.h"
#include "task.h"
#include "led.h"
#include "tinygl.h"
#include "ir_uart.h"
#include "../fonts/font5x7_1.h"
#include "ball.h"
#include "paddle.h"


#define DISPLAY_TASK_RATE 300
#define GAME_TASK_RATE 2
#define NAVSWITCH_TASK_RATE 20

#define WINNING_SCORE 3
#define TEXT_SCROLL_SPEED 10
#define ASCII_DIFFERENCE 48

#define INITIAL_BALL_X_POS 0
#define INITIAL_BALL_Y_POS 3

#define READY 'R'
#define INITIALIZATION_STATE 'I'
#define PLAYING_STATE 'P'
#define END_STATE 'E'


char state; // State of the game
int this_score; // Score of this other player
int their_score; // Score of the other player
int player_num; // Number used to determine who starts
int ball_visible; // Number used to keep track of what screen the ball is on
int just_scored; // Number used to keep track of when someone just scored


/*
 * Function: display_msg
 * --------------------
 * Simple method to display a given message on the led matrix
 *
 * char* msg: Message to be displayed
 *
 */
void display_msg(char* msg)
{
    tinygl_font_set (&font5x7_1);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_speed_set (TEXT_SCROLL_SPEED);
    tinygl_clear();
    tinygl_text(msg);
}


/*
 * Function: display_task
 * --------------------
 * Task to handle the displaying of information throughout the game.
 * Used in a task scheduler, behaves differently based on the current
 * game state.
 *
 */
void display_task_(__unused__ void *data)
{
    static int displaying_end_text = 0; // Var used to determine if end game text displayed
    tinygl_update(); // Refreshing tinygl
    char* score_string;
    switch(state) {
    case INITIALIZATION_STATE:
        // Empty
        break;
    case PLAYING_STATE:
        // Clears previous board state, then draws the paddle and ball on the display
        tinygl_clear();
        tinygl_draw_line(get_paddle_top(), get_paddle_bottom(), 1);
        if (ball_visible) {
            tinygl_draw_point(get_ball(), 1);
        }
        break;
    case END_STATE:
        // Displays the final game outcome and score
        if (!displaying_end_text) {
            if (this_score == WINNING_SCORE) {
                score_string = "Win ";
                score_string[4] = this_score + ASCII_DIFFERENCE;
                score_string[5] = '\0';
            } else {
                score_string = "Lose ";
                score_string[5] = this_score + ASCII_DIFFERENCE;
                score_string[6] = '\0';
            }
            display_msg(score_string);
            displaying_end_text = 1;
        }
        break;
    }
}


/*
 * Function: navswitch_task
 * --------------------
 * Task to handle the inputs from the navswitch throughout the game.
 * Used in a task scheduler, behaves different based on the current
 * game state.
 *
 */
void navswitch_task_(__unused__ void *data)
{
    navswitch_update(); // Updating the navswitch
    switch(state) {
    case INITIALIZATION_STATE:
        // Handles the starting of the game for both players
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            // If this microcontroller initiates the game
            ir_uart_putc(PLAYING_STATE);
            state = PLAYING_STATE;
            player_num = 1;
            ball_visible = 1;
            set_ball_position(INITIAL_BALL_X_POS, INITIAL_BALL_Y_POS);
            led_set(LED1, 0);
        }
        if (ir_uart_read_ready_p()) {
            // If the other microcrontroller initiated the game
            char incoming = ir_uart_getc();
            if (incoming == PLAYING_STATE) {
                state = PLAYING_STATE;
                led_set(LED1, 0);
            }
        }
        break;
    case PLAYING_STATE:
        // Gets input from the user, updating the paddle position.
        paddle_update();
        break;
    case END_STATE:
        // Empty
        break;
    }
}


/*
 * Function: game_task
 * --------------------
 * Task used to handle the game logic throughout the game.
 * Used in a task scheduler, only does something during the game_state
 * of the game.
 *
 */
void game_task_(__unused__ void *data)
{
    int ball_state; // Used to represent state of ball
    switch(state) {
    case INITIALIZATION_STATE:
        // Empty
        break;
    case PLAYING_STATE:
        // Check used to flash LED when someone scores
        if (just_scored) {
            just_scored = 0;
            led_set(LED1, 0);
        }
        // Check used to determine if game is finished
        if(their_score >= WINNING_SCORE || this_score >= WINNING_SCORE) {
            send_ball_position(their_score);
            state = END_STATE;
        } else {
            // If the game isn't finished, either update the balls position (If the
            // ball is on this screen) other wise check if the ball is incoming from
            // the other screen
            if (ball_visible) {
                ball_state = update_position();
                if (ball_state == 1) {
                    // Case when someone has scored
                    their_score++;
                    reset_ball();
                    led_set(LED1, 1);
                    just_scored = 1;
                } else if (ball_state == -1) {
                    // Case when ball has moved screen
                    send_ball_position(their_score);
                    ball_visible = 0;
                }
            } else {
                // Case where ball is on the other screen
                if(ir_uart_read_ready_p()) {
                    // Case where ball is moving from the other screen to this screen
                    this_score = get_ball_position();
                    ball_visible = 1;
                }
            }
        }
        break;
    case END_STATE:
        // Empty
        break;
    }
}


/*
 * Function: setup_connection
 * --------------------
 * Method used to ensure both devices are present before the game is started.
 *
 */
void setup_connection(void)
{
    int sent = 0; // True if confirmation sent
    int rec = 0; // True if confirmation received
    char input;

    // Continuously checks for confirmation from both microcontrollers
    while (!(rec && sent)) {
        navswitch_update();
        if (ir_uart_read_ready_p()) {
            input = ir_uart_getc();
            if (input == READY) {
                rec = 1;
            }
        }
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            ir_uart_putc(READY);
            sent = 1;
        }
    }
    led_set(LED1, 1); // Indicates connections
}


/*
 * Main method of game.c.
 * Calls all required methods to run pong.
 */
int main (void)
{
    // Initializing required variables
    state = INITIALIZATION_STATE;
    this_score = 0;
    their_score = 0;
    player_num = 0;
    ball_visible = 0;
    just_scored = 0;

    // Initializing required systems
    system_init ();
    navswitch_init();
    tinygl_init(DISPLAY_TASK_RATE);
    ir_uart_init();
    led_init();
    paddle_init();
    led_set(LED1, 0);


    // Establishing connection with other microcontroller
    setup_connection();

    char* start_message = "PONG - Press down to start";
    display_msg(start_message);


    // Array of task to run throughout game
    task_t tasks[] = {
        {.func = navswitch_task_, .period = TASK_RATE / NAVSWITCH_TASK_RATE},
        {.func = game_task_, .period = TASK_RATE / GAME_TASK_RATE},
        {.func = display_task_, .period = TASK_RATE / DISPLAY_TASK_RATE}
    };


    // Calling task scheduler on tasks array
    task_schedule(tasks, ARRAY_SIZE(tasks));
}
