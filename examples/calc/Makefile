.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
EXAMPLE_NAME = calculator
BIN_DIR = ../../bin
GRAMMAR = $(EXAMPLE_NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_WARN
TEST_LOG_LEVEL = LOG_LEVEL_ERROR
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL) -fPIC
IFLAGS = -I../../include -I../../lib/logger/include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeggy -lm

EXE_SRCS = mathrepl.c calculatorparser.c calculator.c

all: $(BIN_DIR)/calculator

.MAIN: all

$(BIN_DIR)/calculator: $(GRAMMAR)
	$(BIN_DIR)/peggy $(GRAMMAR) $(GRAMMAR).log $(BLD_LOG_LEVEL)
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

# testing -sinh([-1*-2.0, 2.0/2.0, 2-2, 2^0.5]^2 + 1) should equal [-74.203211,-3.626860,-1.175201,-10.017875]

clean:
	@rm -f calculator.h calculator.c calculator *.log $(BIN_DIR)/calculator
