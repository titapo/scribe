CXX=g++
CXXFLAGS= \
		  --std=c++14 -pedantic \
			-Wshadow \
		  -Wall -Werror -Wextra -Wsuggest-override \
		  -Wduplicated-branches -Wduplicated-cond \


# CATCH_VERSION=v1.10.0
CATCH_VERSION=v2.0.1
SCRIBE_NAME=scribe

all: lib

### library
LIB_OBJDIR=bin/obj
LIB_LIBDIR=bin/lib
LIB_HEADERS=$(wildcard scribe/*.h)
LIB_SOURCES=$(wildcard scribe/*.cc)
LIB_OBJBASENAMES=$(addsuffix .o,$(notdir $(basename $(LIB_SOURCES))))
LIB_OBJS=$(addprefix $(LIB_OBJDIR)/,$(LIB_OBJBASENAMES))
LIB_INCLUDE=-I.
SONAME=lib$(SCRIBE_NAME).so


### objs
create-lib-objdir:
	mkdir -p $(LIB_OBJDIR)

.PHONY: clean-objs
clean-objs:
	rm -rf $(LIB_OBJDIR)

.PHONY: build-objs
build-objs: create-lib-objdir $(LIB_OBJS)
	echo $^

$(LIB_OBJDIR)/%.o: scribe/%.cc
	$(CXX) $(CXXFLAGS) -c -fpic $(LIB_INCLUDE) $^ -o $@

### lib
create-lib-libdir:
	mkdir -p $(LIB_LIBDIR)

.PHONY: clean-lib
clean-lib:
	rm -rf $(LIB_LIBDIR)

.PHONY: build-lib
build-lib: build-objs create-lib-libdir
	$(CXX) -shared $(LIB_OBJS) -o $(LIB_LIBDIR)/$(SONAME)

.PHONY: lib
lib: build-lib


### tests

BIN_TESTDIR=bin/tests
TEST_SRC=$(wildcard tests/*.cc)
TEST_NAMES=$(notdir $(basename $(TEST_SRC)))
#TEST_EXEC=$(addprefix $(BIN_TESTDIR)/,$(TEST_NAMES))
TEST_EXEC=$(addprefix $(BIN_TESTDIR)/,test)
TEST_OBJECTS=$(addsuffix .o,$(addprefix $(BIN_TESTDIR)/,$(TEST_NAMES)))
TEST_INCLUDE=-I.

create-bin-testdir:
	mkdir -p $(BIN_TESTDIR)

d:
	@echo $(TEST_OBJECTS)
	@echo $(TEST_EXEC)

$(TEST_EXEC): $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $(TEST_INCLUDE) -L$(LIB_LIBDIR) -l$(SCRIBE_NAME) $(TEST_OBJECTS) -o $@

.PHONY: build-tests
build-tests: create-bin-testdir $(TEST_EXEC)

$(BIN_TESTDIR)/%.o: tests/%.cc
	@echo Building $@
	$(CXX) $(CXXFLAGS) $(TEST_INCLUDE) -c $^ -o $@

.PHONY: check
check: create-bin-testdir build-tests
	for test in $(TEST_EXEC); do LD_LIBRARY_PATH=$(LIB_LIBDIR) ./$$test || exit 1; done

.PHONY: clean-tests
clean-tests:
	rm -rf $(BIN_TESTDIR)

# environment - init
init: download-catch

download-catch:
	wget https://github.com/catchorg/Catch2/releases/download/v2.0.1/catch.hpp --output-document=tests/catch.hpp
	# wget https://raw.githubusercontent.com/philsquared/Catch/$(CATCH_VERSION)/single_include/catch.hpp --output-document=tests/catch.hpp

.PHONY: clean
clean: clean-lib clean-objs clean-tests


