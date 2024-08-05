.POSIX:
.OBJDIR: ./
CC = gcc
NAME = test
DBG_LOG_LEVEL=LOG_LEVEL_DEBUG
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O0 -g2 -DMAX_LOGGING_LEVEL=$(DBG_LOG_LEVEL) `if [ -n "$(SANITIZE)" ] ; then echo "-fsanitize=address,undefined"; fi`
LFLAGS = '-Wl,-rpath,$$ORIGIN/../bin' -L. -L../bin -lpeggyd
IFLAGS = -I../include -I../lib/logger/include -I../lib/TypeMemPools/include

EXE_SRCS = test_parser.c test_driver.c test_utils.c test_rules.c test_peggy_utils.c

all: clean test

# OpenBSD weirdness
.MAIN: all

clean:
	@rm -f *.o test *.txt *.log *.so


test: $(EXE_SRCS)
	@$(CC) $(CFLAGS) $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)
