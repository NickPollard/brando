C = g++
CFLAGS = -Wall -Wextra -Werror -std=c++14 -Iinclude
LFLAGS =
EXECUTABLE = brando
include Makelist
OBJS = $(SRCS:src/%.cpp=bin/%.o)

all : brando

brando : $(SRCS) $(OBJS)
	$(C) $(LFLAGS) -o $(EXECUTABLE) $(OBJS)

bin/%.o : src/%.cpp
	$(C) $(CFLAGS) -c -o $@ $<
