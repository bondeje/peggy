CC = gcc
NAME = test
PKG_PATH = ..
PKG_RETURN = /$(name)
SLIB = libpeggy.a
SLIB_PATH = ../bin
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused
IFLAGS = -I../include
NDEBUG = 
LIBS = 
ifeq ($(OS), Windows_NT)
	LIBS = -lregex
endif
ifeq ($(NDEBUG), 1)
else
	CFLAGS += -g 
	ifneq ($(OS),Windows_NT)
# sanitizer options are not available on Windows
		CFLAGS += -fsanitize=address,undefined
		LIBS = -lubsan -lasan
	endif
endif

STATIC_LFLAGS := -L$(SLIB_PATH) -l:$(SLIB) $(LIBS)
DYNAMIC_LFLAGS = 

$(SLIB): $($SLIB)
	cd $(PKG_PATH) && $(MAKE) all && cd $(PKG_RETURN)

PackratCache: $(SLIB)
	$(CC) $(CFLAGS) $(IFLAGS) test_$@.c -o test_$@.exe $(STATIC_LFLAGS)

all: $(SLIB) packrat_cache