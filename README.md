README - STATE MACHINE
Matthew R. Johnson

This project implements a state machine creation API in C.
The declarations of the API functions can be found in 'inc/StateMachine.h'.

To compile the API as a library for integration with other projects:
    make lib
This will build libsm.a and put it with StateMachine.h in 'out/lib'.

This project also includes some examples that link against this library.
1) Linear - Implements and injects inputs into a linear state machine
2) Printer - Implements and injects inputs into an example printer state machine
3) Interactive - Uses the same printer state machine but allows user input events

These can built with:
   make {linear|printer|interactive|all}
The default Make target is all.
These binaries will be built and put in '/out'.

There's a special target that will rebuild and run a binary:
	make run_{linear|printer|interactive}

And of course to clean up the outputs:
	make clean


TODO (if I were to continue developing on this):
* API Tweaks
  * Requiring transition lists to be terminated by a null transition isn't the most user-friendly
  * Consider having the State Machine own the transition list, rather than the States
  * Consider supporting inputs outside the range of single chars
  * Consider supporting referencing states by name/id rather than by pointer
* Testing
  * More examples to cover full feature-set
  * Incorporate a unit test framework
  * Write function level testcases
  * Valgrind testing for memory leaks
  * Linter checking
  * Develop a CI/CD process


