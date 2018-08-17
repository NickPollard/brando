#CXX = g++
CFLAGS = -Wall -Wextra -Werror -std=c++14 -Iinclude
LFLAGS =
LIBS = -lpthread
SHARED = libbrando.so
STATIC = libbrando.a
include Makelist
OBJS = $(SRCS:src/%.cpp=bin/%.o)
TARGET = target
INSTALL = /usr/local

TESTLIBS = -lpthread -lbrando
TESTCFLAGS = -g -Wall -Wextra -Werror -std=c++14 -Iinclude -I$(TARGET)/static/include -Itest
TESTLFLAGS = -L$(TARGET)/static/lib

all : shared static

shared : $(OBJS)
	@echo "-- linking shared library --"
	@mkdir -p $(TARGET)/shared/lib
	@$(CXX) $(LFLAGS) -o $(TARGET)/shared/lib/$(SHARED) $(OBJS) $(LIBS)

bin/%.o : src/%.cpp
	@$(CXX) -shared $(CFLAGS) -c -o $@ $<

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
	@$(CXX) -shared $(TESTCFLAGS) -c -o test/bin/test.o test/src/test.cpp
	@$(CXX) $(TESTLFLAGS) -o test/bin/test test/bin/test.o $(TESTLIBS)

test : build-test
	@echo "-- running tests --"
	@test/bin/test

install : test static
	@echo "-- installing libs to /usr/local/lib --"
	@mkdir -p $(INSTALL)/lib/brando
	@cp $(TARGET)/static/lib/$(STATIC) $(INSTALL)/lib/brando/.
	@echo "-- installing headers to /usr/local/include --"
	@mkdir -p $(INSTALL)/include/brando
	@cp -r include/* $(INSTALL)/include/brando/.

