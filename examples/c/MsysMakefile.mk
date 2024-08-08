.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
EXAMPLE_NAME = c
GRAMMAR = $(EXAMPLE_NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_INFO
TEST_LOG_LEVEL = LOG_LEVEL_ERROR
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL) `if [ -n "$(SANITIZE)" ] ; then echo "-g -fsanitize=address,undefined"; else echo "-DNDEBUG"; fi` -fPIC
IFLAGS = -I../../include -I../../lib/logger/include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/../../bin' -L../../bin -lpeggy

EXE_SRCS = cparser.c c.c

all: c.exe

.MAIN: all

c.exe: $(GRAMMAR)
	../../bin/peggy $(GRAMMAR) $(GRAMMAR).log $(BLD_LOG_LEVEL)
	$(CC) $(CFLAGS) $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)
	cp ../../bin/libpeggy.dll .

clean:
	@rm -f c.h c.c c.exe *.log *.dll
