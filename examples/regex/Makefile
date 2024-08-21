.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
EXAMPLE_NAME = regex
BIN_DIR = ../../bin
STDC_DIALECT = -std=c99
GRAMMAR = $(EXAMPLE_NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_WARN
TEST_LOG_LEVEL = LOG_LEVEL_TRACE
DEFAULT_LOG_LEVEL = LOG_LEVEL_WARN
CFLAGS_COMMON = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter $(STDC_DIALECT) -g -DDEFAULT_LOG_LEVEL=$(DEFAULT_LOG_LEVEL) -DMAX_LOGGING_LEVEL=$(TEST_LOG_LEVEL) -fPIC
CFLAGS = $(CFLAGS_COMMON) -O2 
CFLAGS_TEST = $(CFLAGS_COMMON) -fsanitize=address,undefined
IFLAGS = -I../../include -I../../lib/logger/include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeggy
LFLAGS_TEST = '-Wl,-rpath,$$ORIGIN/../../bin' -L$(BIN_DIR) -lpeggy

EXE_SRCS = re.c reparser.c reNFA.c ../../lib/TypeMemPools/src/mempool.c
TEST_SRCS = test.c test_utils.c test_re_utils.c

all: $(BIN_DIR)/regex

.MAIN: all

re.h: $(GRAMMAR)
	$(BIN_DIR)/peggy $(GRAMMAR) $(GRAMMAR).log $(BLD_LOG_LEVEL)

$(BIN_DIR)/regex: re.h
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

test: re.h $(EXE_SRCS) $(TEST_SRCS)
	$(CC) $(CFLAGS_TEST) $(IFLAGS) $(EXE_SRCS) $(TEST_SRCS) -o $@ $(LFLAGS_TEST)
	./test --verbose
	
clean:
	@rm -f re.h re.c *.log $(BIN_DIR)/regex test
