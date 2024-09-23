.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
EXAMPLE_NAME = calculator
BIN_DIR = ../../bin
GRAMMAR = $(EXAMPLE_NAME).grmr
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -fPIC
IFLAGS = -I../../include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/../../bin' -L$(BIN_DIR) -lpeggy -lm

EXE_SRCS = mathrepl.c calculatorparser.c calculator.c

all: $(BIN_DIR)/calculator.exe

.MAIN: all

$(BIN_DIR)/calculator.exe: $(GRAMMAR)
	$(BIN_DIR)/peggy $(GRAMMAR)
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

clean:
	@rm -f calculator.h calculator.c $(BIN_DIR)/calculator.exe *.dll
