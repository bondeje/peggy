.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: ./
.SUFFIXES:
CC = gcc
NAME = peg4c

SUB_MAKE_ARGS = CC=$(CC) SANITIZE=$(SANITIZE)
# for use in specifying a PCRE2 path and (for linux) overriding GNU_regex
PCRE2 = 
COMMON_CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -fPIC
DBG_CFLAGS = $(COMMON_CFLAGS) -O0 -g3
CFLAGS = $(COMMON_CFLAGS) -O2 -DNDEBUG
COMMON_IFLAGS = -Iinclude -Ilib/TypeMemPools/include/
DBG_IFLAGS = $(COMMON_IFLAGS)
IFLAGS = $(COMMON_IFLAGS)
LIB_LFLAGS = -lpcre2-8
COMMON_LFLAGS = -Lbin '-Wl,-rpath,$$ORIGIN/.'
DBG_LFLAGS = $(COMMON_LFLAGS) -lpeg4cd
LFLAGS = $(COMMON_LFLAGS) -lpeg4c

EXT_LIB_OBJS = lib/TypeMemPools/src/mempool.o
DBG_EXT_LIB_OBJS = lib/TypeMemPools/src/mempool.do
LIB_OBJS = src/astnode.o src/hash_utils.o src/packrat_cache.o src/parser.o src/rule.o src/token.o src/utils.o 
DBG_LIB_OBJS = src/astnode.do src/hash_utils.do src/packrat_cache.do src/parser.do src/rule.do src/token.do src/utils.do
EXE_OBJS = src/peg4c.o src/peg4cparser.o src/peg4cstring.o src/peg4cbuild.o src/peg4ctransform.o

all: build_paths bin/lib$(NAME).dll bin/lib$(NAME)d.dll bin/$(NAME).exe bin/test.exe

bin/test.exe: bin/lib$(NAME)d.dll
	@(cd tests && unset MAKELEVEL && make $(SUB_MAKE_ARGS))
	@(echo "running tests")
	$@ --verbose
	@(cd examples/peg4c && unset MAKELEVEL && touch peg4c.grmr && make $(SUB_MAKE_ARGS) test)

ext_libs: $(EXT_LIB_OBJS) $(DBG_EXT_LIB_OBJS)
	@(cd lib/TypeMemPools && unset MAKELEVEL && make $(SUB_MAKE_ARGS) | true)
	@cp lib/TypeMemPools/include/* include/peg4c/

clean:
	@rm -f src/*.o src/*.do src/*.ast include/peg4c/mempool.h
	@rm -rf bin
	@(cd tests && unset MAKELEVEL && make -f MsysMakefile.mk clean)
	@(cd lib/TypeMemPools && unset MAKELEVEL && make clean)
	@(cd examples/peg4c && unset MAKELEVEL && make -f MsysMakefile.mk clean)

build_paths:
	@mkdir -p bin

bin/lib$(NAME).dll: build_paths ext_libs $(LIB_OBJS)
	@echo building library $@
	@$(CC) -shared -fPIC $(LIB_OBJS) $(EXT_LIB_OBJS) -o $@ $(LIB_LFLAGS)

bin/lib$(NAME)d.dll: build_paths ext_libs $(DBG_LIB_OBJS)
	@echo building debug library $@
	@$(CC) -shared -fPIC $(DBG_LIB_OBJS) $(DBG_EXT_LIB_OBJS) -o $@ $(LIB_LFLAGS)

bin/$(NAME).exe: build_paths bin/lib$(NAME).dll $(EXE_OBJS)
	@echo building peg4c executable $@
	@$(CC) $(CFLAGS) $(IFLAGS) $(EXE_OBJS) -o $@ $(LFLAGS)

.SUFFIXES: .c .o .do
.c.o:
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

.c.do:
	@if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(DBG_CFLAGS) $$DBGOPT $(DBG_IFLAGS) -c $< -o $@ | true