CC = gcc

LIBS = 
CFLAGS = 
LFLAGS = 
IFLAGS = 
OS = 
UNAME = 

ifeq '$(findstring ;,$(PATH))' ';'
    UNAME := Windows_NT
    COPY = copy
else
    COPY = cp
    UNAME := $(shell uname 2>/dev/null || echo Unknown)
    UNAME := $(patsubst CYGWIN%,Cygwin,$(UNAME))
    UNAME := $(patsubst MSYS%,MSYS,$(UNAME))
    UNAME := $(patsubst MINGW%,MSYS,$(UNAME))
endif

CFLAGS += -D $(UNAME)

# set architecture and compile target flags
ifeq ($(UNAME), $(filter $(UNAME), Windows_NT MSYS MINGW CYGWIN))
    CFLAGS += -D WIN32
	UNAME_M := $(PROCESSOR_ARCHITECTURE)
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        CFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CFLAGS += -D IA32
        endif
    endif
else
    CFLAGS += -D $(UNAME)
    UNAME_M := $(shell uname -m)
    ifeq ($(UNAME_M),$(filter x86_64 amd64, $(UNAME_M)))
        CFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_M)),)
        CFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_M)),)
        CFLAGS += -D ARM
    endif
endif

ifneq ($(origin NDEBUG), undefined)
	CFLAGS += -DNDEBUG -O2 -DMAX_LOGGING_LEVEL=LOG_LEVEL_WARN
else
	CFLAGS += -g3
	ifeq (, $(filter $(UNAME), Windows_NT MSYS MINGW CYGWIN))
	ifneq ($(origin SANITIZE), undefined)
# sanitizer options are not available on Windows and a separate flag is necessary so that I can 
# ensure build is explicitly compatible with valgrind 
# (https://github.com/google/sanitizers/issues/810, which still seems relevant)
		CFLAGS += -fsanitize=address,undefined
		LIBS = -lubsan -lasan
	endif
	endif
endif

INC_DIR = include
SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = obj
BIN_DIR = bin
DEP_DIR := .deps
PKG_NAME = peggy
STATIC_LIB_FILE := lib$(PKG_NAME).a

OBJ_SUFFIX = .o
SRC_SUFFIX = .c
HDR_SUFFIX = .h
DEP_SUFFIX = .d
TEMP_DEP_SUFFIX = .Td
EXE_EXT = 

CFLAGS += -Wall -pedantic -Werror -Wextra -std=gnu99
CLIBFLAGS = $(CFLAGS) -fPIC -c
CFLAGS += -Wno-unused -Wno-unused-parameter

IFLAGS += -I$(INC_DIR) -Ilib/logger/include

DEPFLAGS = -MMD -MP -MF 

MAIN_SRCS = src/peggy.c src/peggyparser.c
LIB_SRCS = src/astnode.c src/hash_map.c src/parser.c src/rule.c src/token.c src/type.c src/utils.c src/packrat_cache.c
ALL_SRCS := $(shell find $(SRC_DIR) -name '*.c')
LIB_OBJS := $(addprefix $(OBJ_DIR)/,$(LIB_SRCS:$(SRC_DIR)/%$(SRC_SUFFIX)=%$(OBJ_SUFFIX)))
LIB_OBJS += lib/logger/obj/logger.o
LIB_SRCS += lib/logger/src/logger.c

#OS specific changes
ifeq ($(UNAME), $(filter $(UNAME), Windows_NT MSYS MINGW CYGWIN))
	LIBS += -lregex -ltre
	EXE_EXT = .exe
endif

EXE := $(PKG_NAME)$(EXE_EXT)
