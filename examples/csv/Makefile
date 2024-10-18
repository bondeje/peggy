.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: .
CC = gcc
NAME = csv
BIN_DIR = ../../bin
GRAMMAR = $(NAME).grmr
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -fPIC
IFLAGS = -I../../include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L../../bin -lpeg4c

EXE_SRCS = csvparser.c csv.c

all: $(BIN_DIR)/csv

.MAIN: all

$(BIN_DIR)/csv: $(GRAMMAR)
	$(BIN_DIR)/peg4c $(GRAMMAR)
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

clean:
	@rm -f csv.c csv.h *.o csv $(BIN_DIR)/csv

# run timing tests
SAMPLE_DIR = sample_files
test_all: all
	$(BIN_DIR)//csv --timeit `find $(SAMPLE_DIR) -type f -name "*.csv"`

test_one: all
	$(BIN_DIR)//csv --timeit $(FILE) 