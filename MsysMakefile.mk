.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: ./
.SUFFIXES:
CC = gcc
NAME = peggy
MAX_LOGGING_LEVEL = LOG_LEVEL_WARN
BLD_LOG_LEVEL = LOG_LEVEL_ERROR
DBG_LOG_LEVEL = LOG_LEVEL_DEBUG

SUB_MAKE_ARGS = CC=$(CC) SANITIZE=$(SANITIZE) BLD_LOG_LEVEL=$(BLD_LOG_LEVEL) DBG_LOG_LEVEL=$(DBG_LOG_LEVEL)
# for use in specifying a PCRE2 path and (for linux) overriding GNU_regex
PCRE2 = 
COMMON_CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -fPIC
DBG_CFLAGS = $(COMMON_CFLAGS) -O0 -g3 -DMAX_LOGGING_LEVEL=$(DBG_LOG_LEVEL)
CFLAGS = $(COMMON_CFLAGS) -O2 -DNDEBUG -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL)
COMMON_IFLAGS = -Iinclude -Ilib/logger/include/ -Ilib/TypeMemPools/include/
DBG_IFLAGS = $(COMMON_IFLAGS)
IFLAGS = $(COMMON_IFLAGS)
LIB_LFLAGS = -lpcre2-8
COMMON_LFLAGS = -Lbin '-Wl,-rpath,$$ORIGIN/.'
DBG_LFLAGS = $(COMMON_LFLAGS) -lpeggyd
LFLAGS = $(COMMON_LFLAGS) -lpeggy

EXT_LIB_OBJS = lib/logger/src/logger.o lib/TypeMemPools/src/mempool.o
DBG_EXT_LIB_OBJS = lib/logger/src/logger.do lib/TypeMemPools/src/mempool.do
LIB_OBJS = src/astnode.o src/hash_utils.o src/packrat_cache.o src/parser.o src/rule.o src/token.o src/utils.o 
DBG_LIB_OBJS = src/astnode.do src/hash_utils.do src/packrat_cache.do src/parser.do src/rule.do src/token.do src/utils.do
EXE_OBJS = src/peggy.o src/peggyparser.o src/peggystring.o src/peggybuild.o src/peggytransform.o

all: build_paths bin/lib$(NAME).dll bin/lib$(NAME)d.dll bin/$(NAME).exe bin/test.exe

bin/test.exe: bin/lib$(NAME)d.dll
	@(cd tests && unset MAKELEVEL && make $(SUB_MAKE_ARGS))
	@(echo "running tests")
	$@ --verbose
	@(cd examples/peggy && unset MAKELEVEL && touch peggy.grmr && make $(SUB_MAKE_ARGS) test)

ext_libs: $(EXT_LIB_OBJS) $(DBG_EXT_LIB_OBJS)
	@(cd lib/logger && unset MAKELEVEL && make $(SUB_MAKE_ARGS) | true)
	@(cd lib/TypeMemPools && unset MAKELEVEL && make $(SUB_MAKE_ARGS) | true)
	@cp lib/logger/include/* include/peggy/
	@cp lib/TypeMemPools/include/* include/peggy/

clean:
	@rm -f src/*.o src/*.do src/*.ast include/peggy/logger.h include/peggy/mempool.h
	@rm -rf bin
	@(cd tests && unset MAKELEVEL && make -f MsysMakefile.mk clean)
	@(cd lib/TypeMemPools && unset MAKELEVEL && make clean)
	@(cd lib/logger && unset MAKELEVEL && make clean)
	@(cd examples/peggy && unset MAKELEVEL && make -f MsysMakefile.mk clean)

build_paths:
	@mkdir -p bin

bin/lib$(NAME).dll: build_paths ext_libs $(LIB_OBJS)
	@echo building library $@
	@$(CC) -shared -fPIC $(LIB_OBJS) $(EXT_LIB_OBJS) -o $@ $(LIB_LFLAGS)

bin/lib$(NAME)d.dll: build_paths ext_libs $(DBG_LIB_OBJS)
	@echo building debug library $@
	@$(CC) -shared -fPIC $(DBG_LIB_OBJS) $(DBG_EXT_LIB_OBJS) -o $@ $(LIB_LFLAGS)

bin/$(NAME).exe: build_paths bin/lib$(NAME).dll $(EXE_OBJS)
	@echo building peggy executable $@
	@$(CC) $(CFLAGS) $(IFLAGS) $(EXE_OBJS) -o $@ $(LFLAGS)

.SUFFIXES: .c .o .do
.c.o:
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

.c.do:
	@if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="-DNDEBUG"; fi ; \
	$(CC) $(DBG_CFLAGS) $$DBGOPT $(DBG_IFLAGS) -c $< -o $@ | true