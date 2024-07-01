include ../../environment.mk

EXAMPLE_NAME = ipv4
GRAMMAR = $(EXAMPLE_NAME).grmr
PKG_PATH = ../..
PKG_RETURN = examples/$(NAME)
PEGGY_IFLAGS = -I../../include -I../../lib/logger/include
ifneq ($(UNAME), Linux)
    PEGGY_IFLAGS += -I../../lib/pcre2/include
endif
PARSER_SRC := $(EXAMPLE_NAME)parser$(SRC_SUFFIX)
EXPORT_SRC = $(EXAMPLE_NAME)$(SRC_SUFFIX)

all: generate_parser build_parser move_regex

ifneq ($(UNAME), Linux)
move_regex:
	cp ../../$(LIB_DIR)/pcre2/bin/libpcre2-* .
else
move_regex:

endif

generate_parser:
	rm -f *.log
	$(PKG_PATH)/$(BIN_DIR)/$(EXE) $(GRAMMAR) $(GRAMMAR).log LOG_LEVEL_INFO

build_parser:
	$(CC) $(CFLAGS) $(PEGGY_IFLAGS) $(PARSER_SRC) $(EXPORT_SRC) -o $(EXAMPLE_NAME)$(EXE_EXT) -L$(PKG_PATH)/$(BIN_DIR) -l:$(STATIC_LIB_FILE) $(LIBS)

clean:
	rm -f $(EXAMPLE_NAME)$(EXE_EXT)
	rm -f $(EXAMPLE_NAME)$(HDR_SUFFIX)
	rm -f $(EXAMPLE_NAME)$(SRC_SUFFIX)
	rm -f gmon.out
	rm -f *.log