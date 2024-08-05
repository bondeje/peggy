.POSIX:
.OBJDIR: ./
CC = gcc
NAME = test
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O0 -g3 `if [ -n "$(SANITIZE)" ] ; then echo "-fsanitize=address,undefined"; fi`
LFLAGS = '-Wl,-rpath,$$ORIGIN/../bin' -L. -lpeggyd
IFLAGS = -I../include -I../lib/logger/include -I../lib/TypeMemPools/include

EXE_SRCS = test_parser.c test_driver.c test_utils.c test_rules.c test_peggy_utils.c

all: clean get_shared test.exe

clean:
	@rm -f *.o test.exe *.txt *.log *.dll

get_shared:
	@cp ../bin/libpeggyd.dll ./

test.exe: $(EXE_SRCS)
	@$(CC) $(CFLAGS) $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

