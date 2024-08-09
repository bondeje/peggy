.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
EXAMPLE_NAME = ipv4
BIN_DIR = ../../bin
GRAMMAR = $(EXAMPLE_NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_WARN
TEST_LOG_LEVEL = LOG_LEVEL_ERROR
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL) -fPIC
IFLAGS = -I../../include -I../../lib/logger/include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeggy

EXE_SRCS = ipv4parser.c ipv4.c

all: $(BIN_DIR)/ipv4

.MAIN: all

$(BIN_DIR)/ipv4: $(GRAMMAR)
	$(BIN_DIR)/peggy $(GRAMMAR) $(GRAMMAR).log $(BLD_LOG_LEVEL)
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

# some tests
# $(BIN_DIR)/ipv4 192.168.0.0 OK
# $(BIN_DIR)/ipv4 192.168.0.-1 FAILS
# $(BIN_DIR)/ipv4 192.168.0.255 OK
# $(BIN_DIR)/ipv4 192.168.0.256 FAILS
# $(BIN_DIR)/ipv4 192.168..0 FAILS
# $(BIN_DIR)/ipv4 192.168.0 FAILS

clean:
	@rm -f ipv4.h ipv4.c ipv4 *.log $(BIN_DIR)/ipv4
