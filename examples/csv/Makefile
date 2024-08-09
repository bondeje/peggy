.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
NAME = csv
BIN_DIR = ../../bin
GRAMMAR = $(NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_WARN
TEST_LOG_LEVEL = LOG_LEVEL_ERROR
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL) -fPIC
IFLAGS = -I../../include -I../../lib/logger/include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L../../bin -lpeggy

EXE_SRCS = csvparser.c csv.c

all: $(BIN_DIR)/csv

.MAIN: all

$(BIN_DIR)/csv: $(GRAMMAR)
	$(BIN_DIR)/peggy $(GRAMMAR) $(GRAMMAR).log $(BLD_LOG_LEVEL)
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

clean:
	@rm -f csv.c csv.h *.o csv *.log $(BIN_DIR)/csv

# run timing tests
SAMPLE_DIR = sample_files
test_all: all
	$(BIN_DIR)//csv --timeit `find $(SAMPLE_DIR) -type f -name "*.csv"`

test_one: all
	$(BIN_DIR)//csv --timeit --log=csvparser.log --log_level=$(TEST_LOG_LEVEL) $(FILE) 