.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
EXAMPLE_NAME = c
GRAMMAR = $(EXAMPLE_NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_TRACE
TEST_LOG_LEVEL = LOG_LEVEL_ERROR
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL) -fPIC
IFLAGS = -I../../include -I../../lib/logger/include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/../../bin' -L../../bin -lpeggy

EXE_SRCS = cparser.c c.c

all: c

.MAIN: all

c: $(GRAMMAR)
	../../bin/peggy $(GRAMMAR) $(GRAMMAR).log LOG_LEVEL_INFO
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

clean:
	@rm -f c.h c.c c *.log test_files/*.ast
