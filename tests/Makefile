.POSIX:
.OBJDIR: ./
CC = gcc
NAME = test
DBG_LOG_LEVEL=LOG_LEVEL_DEBUG
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O0 -g2 -DMAX_LOGGING_LEVEL=$(DBG_LOG_LEVEL) `if [ -n "$(SANITIZE)" ] ; then echo "-fsanitize=address,undefined"; fi`
LFLAGS = '-Wl,-rpath,$$ORIGIN/../bin' -L. -L../bin -lpeggyd
IFLAGS = -I../include -I../lib/logger/include -I../lib/TypeMemPools/include

TEST_OBJS = test_parser.o test_driver.o test_utils.o test_rules.o test_peggy_utils.o

all: clean test

# OpenBSD weirdness
.MAIN: all

clean:
	@rm -f *.o test *.txt *.log *.so


test: $(TEST_OBJS)
	@$(CC) $(CFLAGS) $(IFLAGS) $(TEST_OBJS) -o $@ $(LFLAGS)

.c.o:
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@