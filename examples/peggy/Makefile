.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: ./
CC = gcc
EXAMPLE_NAME = peggy
BIN_DIR = ../../bin
GRAMMAR = $(EXAMPLE_NAME).grmr
BLD_LOG_LEVEL = LOG_LEVEL_INFO
TEST_LOG_LEVEL = LOG_LEVEL_ERROR
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL) -fPIC
IFLAGS = -I../../include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeggy

SRC_DIR = ../../src
INC_DIR = ../../include

INCS = $(INC_DIR)/peggybuild.h $(INC_DIR)/peggyparser.h $(INC_DIR)/peggystring.h $(INC_DIR)/peggytransform.h
SRCS = $(SRC_DIR)/peggybuild.c $(SRC_DIR)/peggyparser.c $(SRC_DIR)/peggystring.c $(SRC_DIR)/peggytransform.c

EXE_SRCS = peggybuild.c peggyparser.c peggystring.c peggytransform.c peggy.c

all: $(BIN_DIR)/peggy

# OpenBSD weirdness
.MAIN: all

$(BIN_DIR)/peggy: $(GRAMMAR)
	@(echo "\nbuilding peggy parser")
	@(if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS))

rebuild:
	@cp $(SRCS) $(INCS) .
	@(cd ../.. && bin/peggy examples/peggy/$(GRAMMAR) $(GRAMMAR).log $(BLD_LOG_LEVEL) && mv peggy.* examples/peggy && \
	rm -f bin/peggy) || $(BIN_DIR)/peggy peggy.grmr $(GRAMMAR).log $(BLD_LOG_LEVEL) && rm -f $(BIN_DIR)/peggy
	@(echo "\nbuilding peggy parser")
	@(if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $(BIN_DIR)/peggy $(LFLAGS))

test: rebuild
	@(echo "running bootstrap tests")
	@mv peggy.c peggy_orig.c
	@mv peggy.h peggy_orig.h
	@(cd ../../ && bin/peggy examples/peggy/peggy.grmr $(GRAMMAR).log LOG_LEVEL_ERROR && mv peggy.* examples/peggy) || $(BIN_DIR)/peggy peggy.grmr $(GRAMMAR).log $(BLD_LOG_LEVEL)
	@(if [ -z "`comm -3 peggy_orig.h peggy.h`" ] && [ -z "`comm -3 peggy_orig.c peggy.c`" ] ; then echo "test peggy bootstrap...passed" ; else echo "test peggy bootstrap...failed" ; fi)

# be careful with this. will remove any version of peggy that you have already built
clean:
	@rm -f *.c *.h peggy *.log *.o $(BIN_DIR)/peggy
