.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
EXAMPLE_NAME = ipv4
BIN_DIR = ../../bin
GRAMMAR = $(EXAMPLE_NAME).grmr
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -fPIC
IFLAGS = -I../../include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeg4c

EXE_SRCS = ipv4parser.c ipv4.c

all: $(BIN_DIR)/ipv4

.MAIN: all

$(BIN_DIR)/ipv4: $(GRAMMAR)
	$(BIN_DIR)/peg4c $(GRAMMAR)
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
	@rm -f ipv4.h ipv4.c ipv4 $(BIN_DIR)/ipv4
