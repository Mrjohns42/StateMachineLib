/**
 * @brief Builds and injects events into the example printer state machine
 */

#include "StateMachine.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Status_e _status;
#define VERIFY(statement, expected) {if((_status = statement) != expected) return _status;}

void state_transition(State_t * curr, State_t * next, Event_t * evt)
{
    printf("Event: \'%c\', Transition: \'%s\' -> \'%s\'\n",
        evt->input, curr->name, next->name);
}

void enter_off(void * data) {printf("Power OFF\n");}
void exit_off(void * data) {printf("Power ON\n");}
void enter_idle(void * data) {printf("Awaiting print\n");}
void enter_ready(void * data) {printf("Ready to print\n");}
void enter_print(void * data)
{
    printf("Printing...\n");
    if (NULL != data) {
        printf("\t%s\n", (char *)data);
    }
}
void enter_err_setup(void * data) {printf("Setup Error!\n");}
void enter_err_print(void * data) {printf("Print Error!\n");}
void exit_err(void * data) {printf("Error cleared\n");}

Status_e run_printer_machine()
{
    StateMachine_t sm = {0};
    State_t state_off, state_idle, state_ready, state_print,
            state_err_setup, state_err_print = {0};
    Transition_t from_off[2], from_idle[4], from_ready[3], from_print[3],
                 from_err_setup[2], from_err_print[2] = {0};

    VERIFY( populateTransition(&from_off[0], 'o', &state_idle, NULL), SUCCESS );
    VERIFY( populateState(&state_off, "Off", enter_off, NULL, exit_off, from_off), SUCCESS );

    VERIFY( populateTransition(&from_idle[0], 'p', &state_ready, NULL), SUCCESS );
    VERIFY( populateTransition(&from_idle[1], 'e', &state_err_setup, NULL), SUCCESS );
    VERIFY( populateTransition(&from_idle[2], 'o', &state_off, NULL), SUCCESS );
    VERIFY( populateState(&state_idle, "Idle", enter_idle, NULL, NULL, from_idle), SUCCESS );

    VERIFY( populateTransition(&from_ready[0], 's', &state_print, NULL), SUCCESS );
    VERIFY( populateTransition(&from_ready[1], 'e', &state_err_print, NULL), SUCCESS );
    VERIFY( populateState(&state_ready, "Ready", enter_ready, NULL, NULL, from_ready), SUCCESS );

    VERIFY( populateTransition(&from_print[0], 'c', &state_idle, NULL), SUCCESS );
    VERIFY( populateTransition(&from_print[1], 'e', &state_err_print, NULL), SUCCESS );
    VERIFY( populateState(&state_print, "Printing", enter_print, NULL, NULL, from_print), SUCCESS );

    VERIFY( populateTransition(&from_err_setup[0], 'r', &state_idle, NULL), SUCCESS );
    VERIFY( populateState(&state_err_setup, "Setup Error", enter_err_setup, NULL, exit_err, from_err_setup), SUCCESS );

    VERIFY( populateTransition(&from_err_print [0], 'r', &state_idle, NULL), SUCCESS );
    VERIFY( populateState(&state_err_print, "Print Error", enter_err_print, NULL, exit_err, from_err_print), SUCCESS );

    VERIFY( populateStateMachine(&sm, &state_off, NULL, state_transition), SUCCESS );

    Event_t events[8] = {0};
    VERIFY( populateEvent(&events[0], 'o', NULL), SUCCESS );
    VERIFY( populateEvent(&events[1], 'p', NULL), SUCCESS );
    VERIFY( populateEvent(&events[2], 'o', NULL), SUCCESS );
    VERIFY( populateEvent(&events[3], 's', "Hello World!"), SUCCESS );
    VERIFY( populateEvent(&events[4], 'e', NULL), SUCCESS );
    VERIFY( populateEvent(&events[5], 'r', NULL), SUCCESS );
    VERIFY( populateEvent(&events[6], 'o', NULL), SUCCESS );

    Event_t * event = &events[0];
    while (event->input != '\0') 
    {
        printf("Input: %c, Data: %s\n", event->input, (char *) event->data);
        processInput(&sm, event);
        event = event + 1;
        printf("\n");
    }

    return SUCCESS;
}


int main(void)
{
    printf("\nPRINTER STATE MACHINE\n");
    Status_e status = run_printer_machine();
    if (SUCCESS == status) printf("DONE\n");
    else printf("ERROR: %d\n", status);
    return status;
}