/**
 * @brief Builds the example printer state machine and allows
 *        the user to input events
 */

#include "StateMachine.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Status_e _status;
#define VERIFY(statement, expected) {if((_status = statement) != expected) return _status;}

char * INSTRUCTIONS = "\nType a single event char,\noptionally followed by a space plus any event data.\nType 'quit' to stop.\n\n";

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

Status_e interactive_printer_machine()
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

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    printf("%s", INSTRUCTIONS);
    printf("Enter event: ");
    nread = getline(&line, &len, stdin);
    while (nread != -1)
    {
        if (0 == strncmp(line, "quit\n", len)) break;
        if (nread < 2) // one char plus \n
        {
            printf("\n");
            printf("Enter event: ");
            nread = getline(&line, &len, stdin);
            continue;
        }
        if (nread > 2 && line[1] != ' ') // > one char plus \n
        {
            printf("%s", INSTRUCTIONS);
            printf("\n");
            printf("Enter event: ");
            nread = getline(&line, &len, stdin);
            continue;
        }
        Event_t event = {0};
        void * data = NULL;
        if (nread > 3 && line[1] == ' ') // > one char plus ' ' plus \n
        {
            line[nread-1] = '\0';
            data = (void *)(line+2);
        }
        VERIFY( populateEvent(&event, line[0], data), SUCCESS );
        processInput(&sm, &event);
        printf("\n");
        printf("Enter event: ");
        nread = getline(&line, &len, stdin);
    }
    printf("\nMACHINE STOPPED\n");
    free(line);
    return SUCCESS;
}


int main(void)
{
    printf("\nPRINTER STATE MACHINE\n");
    Status_e status = interactive_printer_machine();
    if (SUCCESS == status) printf("DONE\n");
    else printf("ERROR: %d\n", status);
    return status;
}