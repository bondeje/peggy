.POSIX:
.OBJDIR: .
CC = gcc
NAME = csv
BIN_DIR = ../../bin
GRAMMAR = $(NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_WARN
TEST_LOG_LEVEL = LOG_LEVEL_ERROR
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL) `if [ -n "$(SANITIZE)" ] ; then echo "-g -fsanitize=address,undefined"; else echo "-DNDEBUG"; fi` -fPIC
IFLAGS = -I../../include -I../../lib/logger/include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeg4c

EXE_SRCS = csvparser.c csv.c

all: $(BIN_DIR)/csv.exe

.MAIN: all

$(BIN_DIR)/csv.exe: $(GRAMMAR)
	$(BIN_DIR)/peg4c.exe $(GRAMMAR) $(GRAMMAR).log $(BLD_LOG_LEVEL)
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

clean:
	@rm -f csv.c csv.h *.o csv *.log *.dll $(BIN_DIR)/csv.exe

# run timing tests
SAMPLE_DIR = sample_files
test_all: all
	$(BIN_DIR)/csv.exe --timeit `find $(SAMPLE_DIR) -type f -name "*.csv"`

test_one: all
	$(BIN_DIR)/csv.exe --timeit --log=csvparser.log --log_level=$(TEST_LOG_LEVEL) $(FILE) 