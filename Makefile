C = g++
CFLAGS = -Wall -Wextra -Werror -std=c++14 -Iinclude
LFLAGS =
LIBS = -lpthread
SHARED = libbrando.so
STATIC = libbrando.a
include Makelist
OBJS = $(SRCS:src/%.cpp=bin/%.o)
TARGET = target

TESTLIBS = -lpthread -lbrando
TESTCFLAGS = -Wall -Wextra -Werror -std=c++14 -Iinclude -I$(TARGET)/static/include -Itest
TESTLFLAGS = -L$(TARGET)/static/lib

all : shared static

shared : $(OBJS)
	@echo "-- linking shared library --"
	@mkdir -p $(TARGET)/shared/lib
	@$(C) $(LFLAGS) -o $(TARGET)/shared/lib/$(SHARED) $(OBJS) $(LIBS)

bin/%.o : src/%.cpp
	@$(C) -shared $(CFLAGS) -c -o $@ $<

static : $(OBJS)
	@echo "-- building static library --"
	@mkdir -p $(TARGET)/static/lib
	@ar rcs $(TARGET)/static/lib/$(STATIC) $(OBJS)

package : static
	@echo "-- building static package --"
	@mkdir -p $(TARGET)/static/include
	@cp -r include/* $(TARGET)/static/include/.

build-test : package
	@echo "-- building test package --"
	@mkdir -p test/bin
	@$(C) -shared $(TESTCFLAGS) -c -o test/bin/test.o test/src/test.cpp
	@$(C) $(TESTLFLAGS) -o test/bin/test test/bin/test.o $(TESTLIBS)

test : build-test
	@echo "-- running tests --"
	@test/bin/test
