#include "inc/StateMachine.h"

#include <stdio.h>
#include <stdlib.h>

//Logging helpers
#define LOG(...) printf(__VA_ARGS__)
#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)

/* @see StateMachine.h */
Status_e populateTransition(Transition_t * transition,
                            char input,
                            State_t * next_state,
                            guard_fn_t guard)
{
    if (NULL == transition)
    {
        LOG_ERROR("Transition is NULL\n");
        return INVALID_INPUT;
    }
    if ('\0' == input)
    {
        LOG_ERROR("Input is NULL\n");
        return INVALID_INPUT;
    }
    if (NULL == next_state)
    {
        LOG_ERROR("Next State is NULL\n");
        return INVALID_INPUT;
    }
    transition->input = input;
    transition->next_state = next_state;
    transition->guard = guard;
    return SUCCESS;
}

/* @see StateMachine.h */
Status_e populateState(State_t * state,
                     char * name,
                     transition_fn_t entry,
                     transition_fn_t reentry,
                     transition_fn_t exit,
                     Transition_t * transition_list)
{
    if (NULL == state)
    {
        LOG_ERROR("State is NULL\n");
        return INVALID_INPUT;
    }
    if (NULL == name)
    {
        LOG_ERROR("Name is NULL\n");
        return INVALID_INPUT;
    }
    state->name = name;
    state->entry = entry;
    state->reentry = reentry;
    state->exit = exit;
    state->transition_list = transition_list;
    return SUCCESS;
}

/* @see StateMachine.h */
Status_e populateStateMachine(StateMachine_t * sm,
                            State_t * entry_state,  
                            State_t * fatal_state,
                            transition_cb_fn_t transition_cb)
{
    if (NULL == sm)
    {
        LOG_ERROR("State Machine is NULL\n");
        return INVALID_INPUT;
    }
    if (NULL == entry_state)
    {
        LOG_ERROR("Entry State is NULL\n");
        return INVALID_INPUT;
    }
    sm->prev_state = NULL;
    sm->current_state = entry_state;
    sm->entry_state = entry_state;
    sm->fatal_state = fatal_state;
    sm->transition_cb = transition_cb;
    return SUCCESS;
}

/* @see StateMachine.h */
Status_e populateEvent(Event_t * event, char input, void * data)
{
    if (NULL == event)
    {
        LOG_ERROR("Event is NULL\n");
        return INVALID_INPUT;
    }
    event->input = input;
    event->data = data;
    return SUCCESS;
}

/* @see StateMachine.h */
Status_e processInput(StateMachine_t * sm, Event_t * event)
{
    if (NULL == sm)
    {
        LOG_ERROR("State Machine is NULL\n");
        return INVALID_INPUT;
    }
    if (NULL == event)
    {
        LOG_ERROR("Event is NULL\n");
        sm->prev_state = sm->current_state;
        sm->current_state = sm->fatal_state;
        return INVALID_INPUT;
    }
    if (NULL == sm->current_state)
    {
        LOG_ERROR("Current State is NULL\n");
        return INVALID_INPUT;
    }

    Transition_t * transition = sm->current_state->transition_list;
    while (transition != NULL && transition->input != '\0' 
           && transition->input != event->input)
    {
        transition = transition + 1;
    }
    if (transition == NULL || transition->input == '\0')
    {
        LOG_ERROR("No valid transition\n");
        return NO_TRANSITION;
    }

    bool can_transit = true;
    if (transition->guard != NULL)
    {
        can_transit = (transition->guard)(event->data);
    }
    if (!can_transit)
    {
        LOG_ERROR("Transition guard returned false\n");
        return TRANSITION_BLOCKED;
    }

    if (sm->transition_cb != NULL)
    {
        (sm->transition_cb)(sm->current_state, transition->next_state, event);
    }

    if (sm->current_state->exit != NULL)
    {
        (sm->current_state->exit)(event->data);
    }

    sm->prev_state = sm->current_state;
    sm->current_state = transition->next_state;

    if (sm->current_state == sm->prev_state) {
        if (sm->current_state->reentry != NULL)
        {
            (sm->current_state->reentry)(event->data);
        }
    }
    else {
        if (sm->current_state->entry != NULL)
        {
            (sm->current_state->entry)(event->data);
        }
    }

    return SUCCESS;
}

