#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdbool.h>

typedef struct Transition_t Transition_t;
typedef struct State_t State_t;
typedef struct StateMachine_t StateMachine_t;
typedef struct Event_t Event_t;

typedef bool (*guard_fn_t)(void *);
typedef void (*transition_fn_t)(void *);
typedef void (*transition_cb_fn_t)(State_t *, State_t *, Event_t *);

typedef enum {
    SUCCESS,
    INVALID_INPUT,
    INVALID_TRANSITION,
    NO_TRANSITION,
    TRANSITION_BLOCKED
} Status_e;

typedef struct Transition_t{
    char input;
    State_t * next_state;
    guard_fn_t guard;
} Transition_t;

typedef struct State_t{
    char * name;
    transition_fn_t entry;
    transition_fn_t reentry;
    transition_fn_t exit;
    Transition_t * transition_list;
} State_t;

typedef struct StateMachine_t{
    State_t * prev_state;
    State_t * current_state;
    State_t * entry_state;
    State_t * fatal_state;
    transition_cb_fn_t transition_cb;
} StateMachine_t;

typedef struct Event_t{
    char input;
    void * data;
} Event_t;


/**
 * Fills in member values of a Transition_t instance
 *
 * @param transition Pointer to an instance of Transition_t, cannot be NULL
 * @param input The character to trigger the transition, cannot be NULL
 * @param next_state Pointer to a State_t to transition to, cannot be NULL
 * @param guard Optional guard function, use NULL to skip
 */
Status_e populateTransition(Transition_t * transition, char input, State_t * next_state, guard_fn_t guard);

/**
 * Fills in member values of a State_t instance
 *
 * @param state Pointer to an instance of State_t, cannot be NULL
 * @param name Character array containing name of the state, cannot be NULL
 * @param entry Optional transition function to call upon state entry, use NULL to skip
 * @param reentry Optional transition function to call upon state reentry, use NULL to skip
 * @param exit Optional transition function to call upon state exit, use NULL to skip
 * @param transition_list Array of Transition_t, last element must be a zeroed out Transition_t, use NULL for no transitions
 */
Status_e populateState(State_t * state, char * name, transition_fn_t entry, transition_fn_t reentry, transition_fn_t exit, Transition_t * transition_list);

/**
 * Fills in member values of a StateMachine_t instance
 *
 * @param sm Pointer to an instance of StateMachine_t, cannot be NULL
 * @param entry_state Pointer to starting state, cannot be NULL
 * @param fatal_state Optional state to transition to if fatal error occurs, use NULL to skip
 * @param transition_cb Optional transition_cb_fn_t function to call when a transition is about to occur, use NULL to skip
 */
Status_e populateStateMachine(StateMachine_t * sm, State_t * entry_state,  State_t * fatal_state, transition_cb_fn_t transition_cb);

/**
 * Fills in member values of an Event_t instance
 *
 * @param event Pointer to an instance of Event_t, cannot be NULL
 * @param input A character to trigger a transition, cannot be NULL
 * @param data Optional data associated with the event, passed to guard/entry/reentry/exit functions, use NULL to skip
 */
Status_e populateEvent(Event_t * event, char input, void * data);

/**
 * Apply an input event to a State Machine
 *
 * @param sm Pointer to an instance of StateMachine_t, cannot be NULL
 * @param event Pointer to an instance of Event_t, cannot be NULL
 */
Status_e processInput(StateMachine_t * sm, Event_t * event);


#endif /*STATEMACHINE_H*/