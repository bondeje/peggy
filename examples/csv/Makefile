include ../../environment.mk

EXAMPLE_NAME = csv
GRAMMAR = $(EXAMPLE_NAME).grmr
PKG_PATH = ../..
PKG_RETURN = examples/$(NAME)
PEGGY_IFLAGS = -I../../include
PARSER_SRC := $(EXAMPLE_NAME)parser$(SRC_SUFFIX)
EXPORT_SRC = $(EXAMPLE_NAME)$(SRC_SUFFIX)

all: from_grammar build_parser

from_grammar:
	$(PKG_PATH)/$(BIN_DIR)/$(EXE) $(GRAMMAR)

build_parser:
	$(CC) $(CFLAGS) $(PEGGY_IFLAGS) $(PARSER_SRC) $(EXPORT_SRC) -o $(EXAMPLE_NAME)$(EXE_EXT) -L$(PKG_PATH)/$(BIN_DIR) -l:$(STATIC_LIB_FILE) $(LIBS)

clean:
	rm -f $(EXAMPLE_NAME)$(EXE_EXT)
	rm -f $(EXAMPLE_NAME)$(HDR_SUFFIX)
	rm -f $(EXAMPLE_NAME)$(SRC_SUFFIX)
	rm -f gmon.out

# run timing tests
CSV_DIR = sample_files
ALL_CSVS := $(shell find $(CSV_DIR) -name '*.csv')
test_times: all
	./$(EXAMPLE_NAME)$(EXE_EXT) --timeit $(ALL_CSVS)

test_one: all
	./$(EXAMPLE_NAME)$(EXE_EXT) sample_files/000065536.csv
