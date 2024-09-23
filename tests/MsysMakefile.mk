.POSIX:
.OBJDIR: ./
CC = gcc
NAME = test
BIN_DIR = ../bin
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O0 -g3 `if [ -n "$(SANITIZE)" ] ; then echo "-fsanitize=address,undefined"; fi`
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeggyd
IFLAGS = -I../include -I../lib/TypeMemPools/include

EXE_SRCS = test_parser.c test_driver.c test_utils.c test_rules.c test_peggy_utils.c

all: clean $(BIN_DIR)/test.exe

clean:
	@rm -f *.o $(BIN_DIR)/test.exe *.txt *.dll

$(BIN_DIR)/test.exe: $(EXE_SRCS)
	@(echo "\nbuilding tests")
	@if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="" ; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

