CXX=g++
CXXFLAGS= \
		  --std=c++14 -pedantic \
		  -Wall -Werror -Wextra -Wsuggest-override \
		  -Wduplicated-branches -Wduplicated-cond \


CATCH_VERSION=v1.9.7

all: lib

### library
LIB_BINDIR=bin/obj
LIB_HEADERS=$(wildcard scribe/*.h)
LIB_SOURCES=$(wildcard scribe/*.cc)
LIB_OBJBASENAMES=$(addsuffix .o,$(notdir $(basename $(LIB_SOURCES))))
LIB_OBJS=$(addprefix $(LIB_BINDIR)/,$(LIB_OBJBASENAMES))
LIB_INCLUDE=-I.

debug:
	@echo $(LIB_OBJS)

create-lib-bindir:
	mkdir -p $(LIB_BINDIR)

.PHONY: clean-lib
clean-lib:
	rm -rf $(LIB_BINDIR)

.PHONY: lib
lib: create-lib-bindir build-lib

.PHONY: build-lib
build-lib: $(LIB_OBJS)
	echo $^

$(LIB_BINDIR)/%.o: scribe/%.cc
	$(CXX) $(CXXFLAGS) -c $(LIB_INCLUDE) $^ -o $@

### tests

BIN_TESTDIR=bin/tests
TEST_SRC=$(wildcard tests/*.cc)
TEST_NAMES=$(notdir $(basename $(TEST_SRC)))
TEST_EXEC=$(addprefix $(BIN_TESTDIR)/,$(TEST_NAMES))
TEST_INCLUDE=-I.

create-bin-testdir:
	mkdir -p $(BIN_TESTDIR)

.PHONY: build-tests
build-tests: $(TEST_EXEC)

$(BIN_TESTDIR)/%: tests/%.cc $(LIB_OBJS)
	@echo Building $@
	$(CXX) $(CXXFLAGS) $(TEST_INCLUDE) $^ -o $@

.PHONY: check
check: create-bin-testdir build-tests
	for test in $(TEST_EXEC); do ./$$test || exit 1; done

.PHONY: clean-tests
clean-tests:
	rm -rf $(BIN_TESTDIR)

# environment - init
init: download-catch

download-catch:
	wget https://raw.githubusercontent.com/philsquared/Catch/$(CATCH_VERSION)/single_include/catch.hpp --output-document=tests/catch.hpp

.PHONY: clean
clean: clean-lib clean-tests


