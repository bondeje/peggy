.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: ./
CC = gcc
EXAMPLE_NAME = peggy
GRAMMAR = $(EXAMPLE_NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_INFO
TEST_LOG_LEVEL = LOG_LEVEL_ERROR
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL) `if [ -n "$(SANITIZE)" ] ; then echo "-g -fsanitize=address,undefined"; else echo "-DNDEBUG"; fi` -fPIC
IFLAGS = -I../../include
LFLAGS = '-Wl,-rpath,$$ORIGIN/../../bin' -L../../bin -lpeggy

SRC_DIR = ../../src
INC_DIR = ../../include

INCS = $(INC_DIR)/peggybuild.h $(INC_DIR)/peggyparser.h $(INC_DIR)/peggystring.h $(INC_DIR)/peggytransform.h
SRCS = $(SRC_DIR)/peggybuild.c $(SRC_DIR)/peggyparser.c $(SRC_DIR)/peggystring.c $(SRC_DIR)/peggytransform.c

EXE_SRCS = peggybuild.c peggyparser.c peggystring.c peggytransform.c peggy.c

all: peggy.exe

# OpenBSD weirdness
.MAIN: all

peggy.exe: 
	@cp $(SRCS) $(INCS) .
	@../../bin/peggy $(GRAMMAR) $(GRAMMAR).log $(BLD_LOG_LEVEL)
	@$(CC) $(CFLAGS) $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)
	cp ../../bin/libpeggy.dll .

test: peggy.exe
	@mv peggy.c peggy_orig.c
	@mv peggy.h peggy_orig.h
	@./peggy.exe peggy.grmr
	@(if [ -z "`comm -3 peggy_orig.h peggy.h`" ] && [ -z "`comm -3 peggy_orig.c peggy.c`" ] ; then echo "test peggy bootstrap...passed" ; else echo "test peggy bootstrap...failed" ; fi)

clean:
	@rm -f *.c *.h peggy.exe *.log *.o