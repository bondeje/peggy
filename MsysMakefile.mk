.POSIX:
# just in case FreeBSD's make implementation screws up the current directory
.OBJDIR: ./
.SUFFIXES:
CC = gcc
NAME = peggy
MAX_LOGGING_LEVEL = LOG_LEVEL_WARN
# for use in specifying a PCRE2 path and (for linux) overriding GNU_regex
PCRE2 = 
COMMON_CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -std=gnu99 -DMAX_LOGGING_LEVEL=$(MAX_LOGGING_LEVEL) -fPIC
DBG_CFLAGS = $(COMMON_CFLAGS) -O0 -g3
CFLAGS = $(COMMON_CFLAGS) -O2 -DNDEBUG
COMMON_IFLAGS = -Iinclude -Ilib/logger/include/ -Ilib/TypeMemPools/include/
DBG_IFLAGS = $(COMMON_IFLAGS)
IFLAGS = $(COMMON_IFLAGS)
COMMON_LFLAGS = -Lbin -Wl,-rpath . -lpcre2-8
DBG_LFLAGS = $(COMMON_LFLAGS)
LFLAGS = $(COMMON_LFLAGS)

EXT_LIB_OBJS = lib/logger/src/logger.o lib/TypeMemPools/src/mempool.o
LIB_OBJS = src/astnode.o src/hash_utils.o src/packrat_cache.o src/parser.o src/rule.o src/token.o src/utils.o 
DBG_LIB_OBJS = src/astnode.do src/hash_utils.do src/packrat_cache.do src/parser.do src/rule.do src/token.do src/utils.do
EXE_OBJS = src/peggy.o src/peggyparser.o

all: build_paths bin/lib$(NAME).dll bin/lib$(NAME)d.dll bin/$(NAME).exe tests/test.exe

tests/test.exe: bin/lib$(NAME)d.dll
	(cd tests && unset MAKELEVEL && make -f MsysMakefile.mk)

ext_libs: $(EXT_LIB_OBJS)
	(cd lib/logger && unset MAKELEVEL && make | true)
	(cd lib/TypeMemPools && unset MAKELEVEL && make | true)

clean:
	@rm -f src/*.o src/*.do
	@rm -rf bin
	(cd tests && unset MAKELEVEL && make -f MsysMakefile.mk clean)
	(cd lib/TypeMemPools && unset MAKELEVEL && make clean)
	(cd lib/logger && unset MAKELEVEL && make clean)

build_paths:
	mkdir -p bin

bin/lib$(NAME).dll: build_paths ext_libs $(LIB_OBJS)
	$(CC) -shared -fPIC $(LIB_OBJS) $(EXT_LIB_OBJS) -o $@ $(LFLAGS)

bin/lib$(NAME)d.dll: build_paths ext_libs $(DBG_LIB_OBJS)
	$(CC) -shared -fPIC $(DBG_LIB_OBJS) $(EXT_LIB_OBJS) -o $@ $(LFLAGS)

bin/$(NAME).exe: build_paths bin/lib$(NAME).dll $(EXE_OBJS)
	$(CC) $(CFLAGS) $(IFLAGS) $(EXE_OBJS) -o $@ $(LFLAGS) -l$(NAME)

.SUFFIXES: .c .o .do
.c.o:
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

.c.do:
	$(CC) $(DBG_CFLAGS) $(DBG_IFLAGS) -c $< -o $@ | true