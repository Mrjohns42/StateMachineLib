/**
 * @brief Builds and injects events into a basic linear state machine
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

void enter0(void * data) {printf("Entering A\n");}
void enter1(void * data) {printf("Entering B\n");}
void enter2(void * data) {printf("Entering C\n");}
void enter3(void * data) {printf("Entering D\n");}

void exit0(void * data) {printf("Exiting A\n");}
void exit1(void * data) {printf("Exiting B\n");}
void exit2(void * data) {printf("Exiting C\n");}
void exit3(void * data) {printf("Exiting D\n");}

Status_e run_linear_machine() 
{
    StateMachine_t sm = {0};
    State_t states[4] = {0};
    Transition_t from[3][2] = {0};  

    VERIFY( populateTransition(&from[0][0], '1', &states[1], NULL), SUCCESS );
    VERIFY( populateState(&states[0], "A", enter0, NULL, exit0, &from[0][0]), SUCCESS );

    VERIFY( populateTransition(&from[1][0], '2', &states[2], NULL), SUCCESS );
    VERIFY( populateState(&states[1], "B", enter1, NULL, exit1, &from[1][0]), SUCCESS );

    VERIFY( populateTransition(&from[2][0], '3', &states[3], NULL), SUCCESS );
    VERIFY( populateState(&states[2], "C", enter2, NULL, exit2, &from[2][0]), SUCCESS );

    VERIFY( populateState(&states[3], "D", enter3, NULL, exit3, NULL), SUCCESS );

    VERIFY( populateStateMachine(&sm, &states[0], NULL, state_transition), SUCCESS );

    Event_t events[4] = {0};
    VERIFY( populateEvent(&events[0], '1', NULL), SUCCESS );
    VERIFY( populateEvent(&events[1], '2', NULL), SUCCESS );
    VERIFY( populateEvent(&events[2], '3', NULL), SUCCESS );

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
    printf("\nLINEAR STATE MACHINE\n");
    Status_e status = run_linear_machine();
    if (SUCCESS == status) printf("DONE\n");
    else printf("ERROR: %d\n", status);
    return status;
}