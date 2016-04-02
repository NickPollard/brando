C = g++
CFLAGS = -Wall -Wextra -Werror -std=c++14 -Iinclude
LFLAGS =
LIBS = -lpthread
SHARED = libbrando.so
STATIC = libbrando.a
include Makelist
OBJS = $(SRCS:src/%.cpp=bin/%.o)
TARGET = target

all : shared static

shared : $(SRCS) $(OBJS)
	@echo "-- linking shared library --"
	@mkdir -p $(TARGET)/shared/bin
	@$(C) $(LFLAGS) -o $(TARGET)/shared/bin/$(SHARED) $(OBJS) $(LIBS)

bin/%.o : src/%.cpp
	@$(C) -shared $(CFLAGS) -c -o $@ $<

static : $(SRCS) $(OBJS)
	@echo "-- building static library --"
	@mkdir -p $(TARGET)/static/bin
	@ar rcs $(TARGET)/static/bin/$(STATIC) $(OBJS)

package : static
	@echo "-- building static package --"
	@mkdir -p $(TARGET)/static/include
	@cp -r include/* $(TARGET)/static/include/.
