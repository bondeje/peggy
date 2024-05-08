include ../../environment.mk

EXAMPLE_NAME = csv
GRAMMAR = $(EXAMPLE_NAME).grmr
PKG_PATH = ../..
PKG_RETURN = examples/$(NAME)
PEGGY_IFLAGS = -I../../include -I../../lib/logger/include
ifneq ($(UNAME), Linux)
    PEGGY_IFLAGS += -I../../lib/pcre2/include
endif
PARSER_SRC := $(EXAMPLE_NAME)parser$(SRC_SUFFIX)
EXPORT_SRC = $(EXAMPLE_NAME)$(SRC_SUFFIX)

all: generate_parser build_parser

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

# run timing tests
CSV_DIR = sample_files
ALL_CSVS := $(shell find $(CSV_DIR) -name '*.csv')
test_times: all
	./$(EXAMPLE_NAME)$(EXE_EXT) --timeit $(ALL_CSVS)

test_one: all
	./$(EXAMPLE_NAME)$(EXE_EXT) --timeit $(FILE) csvparser.log LOG_LEVEL_INFO
