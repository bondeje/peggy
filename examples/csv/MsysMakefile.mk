.POSIX:
.OBJDIR: .
CC = gcc
NAME = csv
GRAMMAR = $(NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_INFO
TEST_LOG_LEVEL = LOG_LEVEL_ERROR
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL) `if [ -n "$(SANITIZE)" ] ; then echo "-g -fsanitize=address,undefined"; else echo "-DNDEBUG"; fi` -fPIC
IFLAGS = -I../../include -I../../lib/logger/include -I../../lib/TypeMemPools/include
LFLAGS = '-Wl,-rpath,$$ORIGIN/../../bin' -L../../bin -lpeggy

EXE_SRCS = csvparser.c csv.c

all: csv.exe

.MAIN: all

csv.exe: $(GRAMMAR)
	../../bin/peggy.exe $(GRAMMAR) $(GRAMMAR).log $(BLD_LOG_LEVEL)
	$(CC) $(CFLAGS) $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)
	cp ../../bin/libpeggy.dll .

clean:
	@rm -f csv.c csv.h *.o csv *.log *.dll *.exe

# run timing tests
SAMPLE_DIR = sample_files
test_all: all
	./csv.exe --timeit `find $(SAMPLE_DIR) -type f -name "*.csv"`

test_one: all
	./csv.exe --timeit --log=csvparser.log --log_level=$(TEST_LOG_LEVEL) $(FILE) 

.c.o:
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@