CC = gcc

LIBS = 
ifeq ($(NDEBUG), 1)
else
	CFLAGS += -g3
	ifneq ($(OS),Windows_NT)
# sanitizer options are not available on Windows
#		CFLAGS += -fsanitize=address,undefined
#		LIBS = -lubsan -lasan
	endif
endif
INC_DIR = include
OBJ_DIR = obj
SRC_DIR = src
LIB_DIR = lib
BIN_DIR = bin
DEP_DIR := .deps
PKG_NAME = peggy
STATIC_LIB_FILE := lib$(PKG_NAME).a


OBJ_SUFFIX = .o
SRC_SUFFIX = .c
HDR_SUFFIX = .h
DEP_SUFFIX = .d

CFLAGS += -Wall -pedantic -Werror -Wextra -std=c99 -fPIC 
CLIBFLAGS = $(CFLAGS) -c
CFLAGS += -Wno-unused -Wno-unused-parameter

IFLAGS := -I$(INC_DIR)

DEPFLAGS = -MMD -MP -MF 

COMPILE_SRC := 

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(addprefix $(OBJ_DIR)/,$(SRCS:$(SRC_DIR)/%$(SRC_SUFFIX)=%$(OBJ_SUFFIX)))

all: build_hierarchy dynamic_lib $(BIN_DIR)/$(STATIC_LIB_FILE) main.exe

build_hierarchy: $(DEP_DIR) $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEP_DIR)/%.d | $(DEP_DIR)
	$(CC) -MT $@ $(DEPFLAGS) $(DEP_DIR)/$*.Td $(CLIBFLAGS) $(IFLAGS) -c $< -o $@
	mv -f $(DEP_DIR)/$*.Td $(DEP_DIR)/$*.d && touch $@

$(BIN_DIR)/$(STATIC_LIB_FILE): $(OBJS)
	$(AR) r $@ $(OBJS)

dynamic_lib:

$(DEP_DIR): ; @mkdir -p $@

$(OBJ_DIR): ; @mkdir -p $@

$(BIN_DIR): ; @mkdir -p $@

DEPFILES := $(addprefix $(DEP_DIR)/, $(SRCS:$(SRC_DIR)/%$(SRC_SUFFIX)=%$(DEP_SUFFIX)))
$(DEPFILES):

# DELETE THIS
main.exe: workbench/main.c $(BIN_DIR)/$(STATIC_LIB_FILE)
	$(CC) $(CFLAGS) $(IFLAGS) workbench/main.c -L$(BIN_DIR) -o $(BIN_DIR)/$@ -l:$(STATIC_LIB_FILE) $(LIBS)

# must be at least after the initial, default target; otherwise last
include $(wildcard $(DEPFILES))