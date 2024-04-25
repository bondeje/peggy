CC = gcc
NAME = peggy
PKG_PATH = ../../
PKG_RETURN = examples/$(NAME)
SLIB = libpeggy.a
SLIB_PATH = ../../bin
CFLAGS = -Wall -Werror -Wextra -pedantic -Wno-unused -std=c99
IFLAGS = -I../../include
NDEBUG = 
LIBS = 
ifeq ($(OS), Windows_NT)
	LIBS = -lregex
endif
ifeq ($(NDEBUG), 1)
else
	CFLAGS += -g3
	ifneq ($(OS),Windows_NT)
# sanitizer options are not available on Windows
#		CFLAGS += -fsanitize=address,undefined
#		LIBS = -lubsan -lasan
	endif
endif

$(SLIB):
	cd $(PKG_PATH) && $(MAKE) all && cd $(PKG_RETURN)

all: $(SLIB)
	$(CC) $(CFLAGS) $(IFLAGS) $(NAME)parser.c $(NAME).c -o $(NAME)parser.exe -L$(SLIB_PATH) -l:$(SLIB) $(LIBS)