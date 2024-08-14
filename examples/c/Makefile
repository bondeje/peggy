.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
EXAMPLE_NAME = c
BIN_DIR = ../../bin
STDC_DIALECT = -std=c99
GRAMMAR = $(EXAMPLE_NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_WARN
TEST_LOG_LEVEL = LOG_LEVEL_TRACE
DEFAULT_LOG_LEVEL = LOG_LEVEL_WARN
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter $(STDC_DIALECT) -O2 -g -DDEFAULT_LOG_LEVEL=$(DEFAULT_LOG_LEVEL) -DMAX_LOGGING_LEVEL=$(TEST_LOG_LEVEL) -fPIC
IFLAGS = -I../../include -I../../lib/logger/include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeggy

EXE_SRCS = cparser.c c.c

all: $(BIN_DIR)/c

.MAIN: all

$(BIN_DIR)/c: $(GRAMMAR)
	$(BIN_DIR)/peggy $(GRAMMAR) $(GRAMMAR).log $(BLD_LOG_LEVEL)
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

test: $(BIN_DIR)/c
	@for file in sample_files/*.c; do echo $$file && $ $(BIN_DIR)/c $$file > $$file.ast; done

check_peggy: $(BIN_DIR)/c
	@for file in ../../src/*.c; do echo $$file && $(CC) -E -I../../include -I../../lib/logger/include $$file | $(BIN_DIR)/c > $$file.ast; done

check_libc: $(BIN_DIR)/c
	@echo using $(CC) with dialect $(STDC_DIALECT)
	@for file in sample_files/libc/*.c; do echo $$file && $(CC) $(STDC_DIALECT) -E $$file | $(BIN_DIR)/c > $$file.ast; done

clean:
	@rm -f c.h c.c *.log sample_files/*.ast sample_files/libc/*.ast *.ast $(BIN_DIR)/c
