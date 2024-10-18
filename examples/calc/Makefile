.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
EXAMPLE_NAME = calculator
BIN_DIR = ../../bin
GRAMMAR = $(EXAMPLE_NAME).grmr
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -fPIC
IFLAGS = -I../../include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeg4c -lm

EXE_SRCS = mathrepl.c calculatorparser.c calctr.c

all: $(BIN_DIR)/calculator

.MAIN: all

$(BIN_DIR)/calculator: $(GRAMMAR)
	$(BIN_DIR)/peg4c $(GRAMMAR)
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

# testing -sinh([-1*-2.0, 2.0/2.0, 2-2, 2^0.5]^2 + 1) should equal [-74.203211,-3.626860,-1.175201,-10.017875]

clean:
	@rm -f calctr.h calctr.c calculator $(BIN_DIR)/calculator
