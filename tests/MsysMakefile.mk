.POSIX:
.OBJDIR: ./
CC = gcc
NAME = test
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -std=gnu99 -O0 -g3
LFLAGS = -Wl,-rpath . -L. -lpeggyd -lpcre2-8
IFLAGS = -I../include -I../lib/logger/include -I../lib/TypeMemPools/include

TEST_OBJS = test_parser.o test_driver.o test_utils.o test_rules.o test_peggy_utils.o

all: clean get_shared test.exe

clean:
	@rm -f *.o
	@rm -f test.exe
	@rm -f *.txt
	@rm -f *.log
	@rm -f *.dll

get_shared:
	@cp ../bin/libpeggyd.dll ./

test.exe: $(TEST_OBJS)
	@$(CC) $(CFLAGS) $(IFLAGS) $(TEST_OBJS) -o $@ $(LFLAGS) && ./test.exe --verbose

.c.o:
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@