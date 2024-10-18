.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
EXAMPLE_NAME = c
BIN_DIR = ../../bin
GRAMMAR = $(EXAMPLE_NAME).grmr
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -fPIC
IFLAGS = -I../../include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeg4c

EXE_SRCS = cparser.c c.c

all: $(BIN_DIR)/c.exe test

.MAIN: all

$(BIN_DIR)/c.exe: $(GRAMMAR)
	$(BIN_DIR)/peg4c.exe $(GRAMMAR)
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

test: $(BIN_DIR)/c.exe
	@for file in sample_files/*.c; do echo $$file && $(CC) $(STDC_DIALECT) -E $$file | $(BIN_DIR)/c.exe > $$file.ast; done

check_peg4c: $(BIN_DIR)/c
	@for file in ../../src/*.c; do echo $$file && $(CC) -E -I../../include $$file | $(BIN_DIR)/c.exe > $$file.ast; done

check_libc: $(BIN_DIR)/c.exe
	@echo using $(CC) with dialect $(STDC_DIALECT)
	@for file in sample_files/libc/*.c; do echo $$file && $(CC) $(STDC_DIALECT) -E $$file | $(BIN_DIR)/c > $$file.ast; done

clean:
	@rm -f c.h c.c $(BIN_DIR)/c.exe *.dll sample_files/*.ast sample_files/libc/*.ast *.ast
