.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: ./
CC = gcc
EXAMPLE_NAME = peggy
BIN_DIR = ../../bin
GRAMMAR = $(EXAMPLE_NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_WARN
TEST_LOG_LEVEL = LOG_LEVEL_ERROR
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL) `if [ -n "$(SANITIZE)" ] ; then echo "-g -fsanitize=address,undefined"; else echo "-DNDEBUG"; fi` -fPIC
IFLAGS = -I../../include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeggy

SRC_DIR = ../../src
INC_DIR = ../../include

INCS = $(INC_DIR)/peggybuild.h $(INC_DIR)/peggyparser.h $(INC_DIR)/peggystring.h $(INC_DIR)/peggytransform.h
SRCS = $(SRC_DIR)/peggybuild.c $(SRC_DIR)/peggyparser.c $(SRC_DIR)/peggystring.c $(SRC_DIR)/peggytransform.c

EXE_SRCS = peggybuild.c peggyparser.c peggystring.c peggytransform.c peggy.c

all: $(BIN_DIR)/peggy.exe

# OpenBSD weirdness
.MAIN: all

$(BIN_DIR)/peggy.exe: $(GRAMMAR)
	@cp $(SRCS) $(INCS) .
	@$(BIN_DIR)/peggy.exe $(GRAMMAR) $(GRAMMAR).log $(BLD_LOG_LEVEL)
	@$(CC) $(CFLAGS) $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

test: $(BIN_DIR)/peggy.exe
	@mv peggy.c peggy_orig.c
	@mv peggy.h peggy_orig.h
	@$(BIN_DIR)/peggy.exe peggy.grmr
	@(if [ -z "`comm -3 peggy_orig.h peggy.h`" ] && [ -z "`comm -3 peggy_orig.c peggy.c`" ] ; then echo "test peggy bootstrap...passed" ; else echo "test peggy bootstrap...failed" ; fi)

# be carefule with this. will remove any version of peggy.exe that you have already built
clean:
	@rm -f *.c *.h $(BIN_DIR)/peggy.exe *.log *.o *.dll