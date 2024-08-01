.OBJDIR: ./
CC = gcc
NAME = test
PKG_PATH = ..
PKG_RETURN = /$(name)
SLIB = libpeggy.a
SLIB_PATH = ../bin
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -std=gnu99
IFLAGS = -I../include -I../lib/logger/include -I../lib/TypeMemPools/include
NDEBUG = 
LIBS = 
SHARED_EXT = .so
EXE_EXT = 
DYNAMIC_LFLAGS = -L../bin -lpeggy
#DYNAMIC_LFLAGS = 
ifeq ($(OS), Windows_NT)
	LIBS = -lregex
	SHARED_EXT = .dll
	EXE_EXT = .exe
	DYNAMIC_LFLAGS += -lpcre2-8
else
	CFLAGS += -Wl,-rpath ../bin
#	DYNAMIC_LFLAGS += -lgcc_s
endif
ifeq ($(NDEBUG), 1)
else
	CFLAGS += -g3 
	ifneq ($(OS),Windows_NT)
# sanitizer options are not available on Windows
		CFLAGS += -fsanitize=address,undefined
	endif
endif

STATIC_LFLAGS := -L$(SLIB_PATH) -l:$(SLIB) $(LIBS)


all: clean testlang #$(SLIB) packrat_cache testlang

$(SLIB):
	(cd $(PKG_PATH) && $(MAKE) all )

#PackratCache: $(SLIB)
#	$(CC) $(CFLAGS) $(IFLAGS) test_$@.c -o test_$@.exe $(STATIC_LFLAGS)

clean:
	rm -rf testlangparser.log testlang.c testlang.h testdeque.exe test_PackratCache.exe
	rm -f *.o
	rm -f test
	rm -f *.dll
	rm -f *.exe
	rm -f test_parser_doc_ast.txt
	rm -f *.log

TEST_OBJS = test_parser.o test_driver.o test_utils.o

test: $(TEST_OBJS)
	$(CC) $(CFLAGS) -O0 $(IFLAGS) $(TEST_OBJS) -o $@$(EXE_EXT) $(DYNAMIC_LFLAGS)
	cp ../bin/libpeggy.dll . | true
	cp ../bin/libpcre2-8.dll . | true

.SUFFIXES:
.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -O0 $(IFLAGS) -c $< -o $@