.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: ./
.SUFFIXES:
CC = gcc
NAME = peggy
BLD_LOG_LEVEL = LOG_LEVEL_TRACE
DBG_LOG_LEVEL = LOG_LEVEL_DEBUG

SUB_MAKE_ARGS = CC=$(CC) SANITIZE=$(SANITIZE) BLD_LOG_LEVEL=$(BLD_LOG_LEVEL) DBG_LOG_LEVEL=$(DBG_LOG_LEVEL)

# for use in specifying a PCRE2 path and (for linux) overriding GNU_regex
PCRE2 = 
COMMON_CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -Wno-unused-parameter -std=gnu99 -fPIC -g3
DBG_CFLAGS = $(COMMON_CFLAGS) -O0 -DMAX_LOGGING_LEVEL=$(DBG_LOG_LEVEL)
CFLAGS = $(COMMON_CFLAGS) -O2 -DNDEBUG -DMAX_LOGGING_LEVEL=$(BLD_LOG_LEVEL)
COMMON_IFLAGS = -Iinclude -Ilib/logger/include/ -Ilib/TypeMemPools/include/
DBG_IFLAGS = $(COMMON_IFLAGS)
IFLAGS = $(COMMON_IFLAGS)
COMMON_LFLAGS = -Lbin '-Wl,-rpath,$$ORIGIN/.'
LIB_LFLAGS = $(COMMON_LFLAGS) -L/usr/local/lib
DBG_LFLAGS = $(COMMON_LFLAGS) -lpeggyd
LFLAGS = $(COMMON_LFLAGS) -lpeggy

EXT_LIB_OBJS = lib/logger/src/logger.o lib/TypeMemPools/src/mempool.o
DBG_EXT_LIB_OBJS = lib/logger/src/logger.do lib/TypeMemPools/src/mempool.do
LIB_OBJS = src/astnode.o src/hash_utils.o src/packrat_cache.o src/parser.o src/rule.o src/token.o src/utils.o
DBG_LIB_OBJS = src/astnode.do src/hash_utils.do src/packrat_cache.do src/parser.do src/rule.do src/token.do src/utils.do
EXE_OBJS = src/peggy.o src/peggyparser.o src/peggystring.o src/peggybuild.o src/peggytransform.o

all: build_paths bin/lib$(NAME).so bin/lib$(NAME)d.so bin/$(NAME) bin/test

# OpenBSD weirdness
.MAIN: all

install:
	cp bin/lib*.so /usr/local/lib

bin/test: bin/lib$(NAME)d.so
	@touch examples/peggy/peggy.grmr
	@(cd tests && unset MAKELEVEL && make $(SUB_MAKE_ARGS))
	@(echo "running tests")
	$@ --verbose
	@(cd examples/peggy && touch peggy.grmr && unset MAKELEVEL && make $(SUB_MAKE_ARGS) test)

ext_libs: $(EXT_LIB_OBJS) $(DBG_EXT_LIB_OBJS)
	@(cd lib/logger && unset MAKELEVEL && make $(SUB_MAKE_ARGS))
	@(cd lib/TypeMemPools && unset MAKELEVEL && make $(SUB_MAKE_ARGS))
	@cp lib/logger/include/* include/peggy/
	@cp lib/TypeMemPools/include/* include/peggy/

clean:
	@rm -f src/*.o src/*.do include/peggy/logger.h include/peggy/mempool.h *.log
	@rm -rf bin
	@(cd tests && unset MAKELEVEL && make clean)
	@(cd lib/TypeMemPools && unset MAKELEVEL && make clean)
	@(cd lib/logger && unset MAKELEVEL && make clean)
	@(cd examples/peggy && unset MAKELEVEL && make clean)

build_paths:
	@mkdir -p bin

bin/lib$(NAME).so: build_paths ext_libs $(LIB_OBJS)
	@echo building library $@
	@if [ -n "$$USE_PCRE2"] || [ $$(uname -s) != "Linux" ] ; then export USE_PCRE2="-lpcre2-8" ; else export USE_PCRE2="" ; fi ; \
	$(CC) -shared -fPIC $(LIB_OBJS) $(EXT_LIB_OBJS) -o $@ $(LIB_LFLAGS) $$USE_PCRE2

bin/lib$(NAME)d.so: build_paths ext_libs $(DBG_LIB_OBJS)
	@echo building debug library $@
	@if [ $$(uname -s) != "Linux" ] ; then export USE_PCRE2="-lpcre2-8" ; else export USE_PCRE2="" ; fi ; \
	$(CC) -shared -fPIC $(DBG_LIB_OBJS) $(DBG_EXT_LIB_OBJS) -o $@ $(LIB_LFLAGS) $$USE_PCRE2

bin/$(NAME): build_paths bin/lib$(NAME).so $(EXE_OBJS)
	@echo building peggy executable $@
	@$(CC) $(CFLAGS) $(IFLAGS) $(EXE_OBJS) -o $@ $(LFLAGS)

.SUFFIXES: .c .o .do
# should build in a compile flag to use PCRE2 if USE_PCRE2 is set
.c.o:
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

.c.do:
	@if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="" ; fi ; \
	$(CC) $(DBG_CFLAGS) $$DBGOPT $(DBG_IFLAGS) -c $< -o $@
