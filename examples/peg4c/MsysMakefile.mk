.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: ./
CC = gcc
EXAMPLE_NAME = peg4c
BIN_DIR = ../../bin
GRAMMAR = $(EXAMPLE_NAME).grmr
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -O2 -g -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL) `if [ -n "$(SANITIZE)" ] ; then echo "-g -fsanitize=address,undefined"; else echo "-DNDEBUG"; fi` -fPIC
IFLAGS = -I../../include
LFLAGS = '-Wl,-rpath,$$ORIGIN/.' -L$(BIN_DIR) -lpeg4c

SRC_DIR = ../../src
INC_DIR = ../../include

INCS = $(INC_DIR)/peg4cbuild.h $(INC_DIR)/peg4cparser.h $(INC_DIR)/peg4cstring.h $(INC_DIR)/peg4ctransform.h
SRCS = $(SRC_DIR)/peg4cbuild.c $(SRC_DIR)/peg4cparser.c $(SRC_DIR)/peg4cstring.c $(SRC_DIR)/peg4ctransform.c

EXE_SRCS = peg4cbuild.c peg4cparser.c peg4cstring.c peg4ctransform.c peg4c.c

all: $(BIN_DIR)/peg4c.exe

# OpenBSD weirdness
.MAIN: all

$(BIN_DIR)/peg4c.exe: $(GRAMMAR)
	@cp $(SRCS) $(INCS) .
	@$(BIN_DIR)/peg4c.exe $(GRAMMAR)
	@$(CC) $(CFLAGS) $(IFLAGS) $(EXE_SRCS) -o $@ $(LFLAGS)

rebuild:
	@cp $(SRCS) $(INCS) .
	@(cd ../.. && bin/peg4c.exe examples/peg4c/$(GRAMMAR) && mv peg4c.* examples/peg4c && \
	rm -f bin/peg4c.exe) || $(BIN_DIR)/peg4c peg4c.grmr && rm -f $(BIN_DIR)/peg4c
	@(echo "\nbuilding peg4c parser")
	@(if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(CFLAGS) $$DBGOPT $(IFLAGS) $(EXE_SRCS) -o $(BIN_DIR)/peg4c $(LFLAGS))


test: rebuild
	@mv peg4c.c peg4c_orig.c
	@mv peg4c.h peg4c_orig.h
	@$(BIN_DIR)/peg4c.exe peg4c.grmr
	@(if [ -z "`comm -3 peg4c_orig.h peg4c.h`" ] && [ -z "`comm -3 peg4c_orig.c peg4c.c`" ] ; then echo "test peg4c bootstrap...passed" ; else echo "test peg4c bootstrap...failed" ; fi)

# be carefule with this. will remove any version of peg4c.exe that you have already built
clean:
	@rm -f *.c *.h $(BIN_DIR)/peg4c.exe *.o *.dll