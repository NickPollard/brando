C = g++
CFLAGS = -Wall -Wextra -Werror -std=c++14 -Iinclude
LFLAGS =
EXECUTABLE = brando
include Makelist
OBJS = $(SRCS:src/%.cpp=bin/%.o)

all : brando

brando : $(SRCS) $(OBJS)
	@echo "--linking--"
	@$(C) $(LFLAGS) -o $(EXECUTABLE) $(OBJS)

bin/%.o : src/%.cpp
	@$(C) -shared $(CFLAGS) -c -o $@ $<
