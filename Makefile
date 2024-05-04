include environment.mk

all: build_hierarchy dynamic_lib static_lib main
	@echo environment: $(UNAME)
	@echo architecture: $(UNAME_M)
	@echo sanitize: $(SANITIZE)
	@echo no debug: $(NDEBUG)

build_hierarchy: $(DEP_DIR) $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR)/%$(OBJ_SUFFIX): $(SRC_DIR)/%$(SRC_SUFFIX)
$(OBJ_DIR)/%$(OBJ_SUFFIX): $(SRC_DIR)/%$(SRC_SUFFIX) $(DEP_DIR)/%$(DEP_SUFFIX) | $(DEP_DIR)
	$(CC) -MT $@ $(DEPFLAGS) $(DEP_DIR)/$*$(TEMP_DEP_SUFFIX) $(CLIBFLAGS) $(IFLAGS) -c $< -o $@
	mv -f $(DEP_DIR)/$*$(TEMP_DEP_SUFFIX) $(DEP_DIR)/$*$(DEP_SUFFIX) && touch $@

lib/logger/obj/logger.o: lib/logger/src/logger.c
	cd lib/logger && make all && cd ../..

static_lib: $(LIB_OBJS)
	$(AR) r $(STATIC_LIB_FILE) $(LIB_OBJS)
	mv -f $(STATIC_LIB_FILE) $(BIN_DIR)/$(STATIC_LIB_FILE)

dynamic_lib:

$(DEP_DIR): ; @mkdir -p $@

$(OBJ_DIR): ; @mkdir -p $@

$(BIN_DIR): ; @mkdir -p $@

DEPFILES := $(addprefix $(DEP_DIR)/, $(ALL_SRCS:$(SRC_DIR)/%$(SRC_SUFFIX)=%$(DEP_SUFFIX)))
$(DEPFILES):

main: $(MAIN_SRCS) static_lib
	$(CC) $(CFLAGS) $(IFLAGS) $(MAIN_SRCS) -L$(BIN_DIR) -o $(BIN_DIR)/$(EXE) -l:$(STATIC_LIB_FILE) $(LIBS)

clean: reset
	rm -rf $(BIN_DIR)

# reset just deletes dependency folders that might themselves depend on the environment in which they were last built so as to trigger rebuild everything, but keeps binaries
reset:
	rm -rf $(DEP_DIR)
	rm -rf $(OBJ_DIR)
	rm -rf lib/logger/obj

# must be at least after the initial, default target; otherwise last
include $(wildcard $(DEPFILES))