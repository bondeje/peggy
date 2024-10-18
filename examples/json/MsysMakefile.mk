.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
EXAMPLE_NAME = json
BIN_DIR = ../../bin
GRAMMAR = $(EXAMPLE_NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_WARN
TEST_LOG_LEVEL = LOG_LEVEL_ERROR
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL) -fPIC
IFLAGS = -I../../include -I../../lib/logger/include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeg4c

EXE_SRCS = jsonparser.c json.c

all: $(BIN_DIR)/json.exe

.MAIN: all

$(BIN_DIR)/json.exe: $(GRAMMAR)
	$(BIN_DIR)/peg4c.exe $(GRAMMAR) $(GRAMMAR).log $(BLD_LOG_LEVEL)
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

clean:
	@rm -f json.h json.c $(BIN_DIR)/json.exe *.log *.dll

