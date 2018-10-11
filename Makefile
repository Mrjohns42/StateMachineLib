.PHONY: default
default: all

# Compiler binaries
CC=gcc
AR=ar
RANLIB=ranlib

# Output directories
OUT_DIR=out
OBJ_DIR=$(OUT_DIR)/obj
OBJ_LIB_DIR=$(OBJ_DIR)/lib
OUT_LIB_DIR=$(OUT_DIR)/lib
OUT_TEST_DIR=$(OUT_DIR)/tests

# Lib parameters
LIB_NAME=libsm.a

LIB_DIR=lib
LIB_IDIR=$(LIB_DIR)/inc
LIB_CFLAGS=-I$(LIB_IDIR)

_LIB_DEPS = StateMachine.h
LIB_DEPS = $(patsubst %,$(LIB_IDIR)/%,$(_LIB_DEPS))

_LIB_OBJS = StateMachine.o
LIB_OBJS = $(patsubst %,$(OBJ_LIB_DIR)/%,$(_LIB_OBJS))

# Lib recipes
$(OBJ_LIB_DIR)/%.o: $(LIB_DIR)/%.c $(LIB_DEPS)
	mkdir -p $(OBJ_LIB_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUT_LIB_DIR)/$(LIB_NAME): $(LIB_OBJS) $(LIB_DEPS)
	mkdir -p $(OUT_LIB_DIR)
	$(AR) rc $@ $^
	$(RANLIB) $@
	cp $(LIB_DEPS) $(OUT_LIB_DIR)

.PHONY: lib
lib: $(OUT_LIB_DIR)/$(LIB_NAME) 


# Bin parameters
IDIR=$(OUT_LIB_DIR)
LIBS=-lsm
CFLAGS=-L$(OUT_LIB_DIR) $(LIBS) -I$(IDIR)

_DEPS = StateMachine.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = linear.o printer.o interactive.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))
BINS = $(patsubst %.o,$(OUT_DIR)/%,$(_OBJ))

# Bin recipes
$(OBJ_DIR)/%.o: %.c $(OUT_LIB_DIR)/$(LIB_NAME)
	mkdir -p $(OBJ_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUT_DIR)/%: $(OBJ_DIR)/%.o
	$(CC) -o $@ $^ $(CFLAGS)



# Extra targets
.PHONY: clean all linear printer interactive

linear: $(OUT_DIR)/linear
printer: $(OUT_DIR)/printer
interactive: $(OUT_DIR)/interactive
all: $(BINS)

clean:
	rm -rf $(OUT_DIR)

#run_linear, run_printer, or run_interactive
run_%: $(OUT_DIR)/%
	./$^


