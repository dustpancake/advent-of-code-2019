include ../build/conanbuildinfo.mak
$(call CONAN_BASIC_SETUP)

MAIN		:= day11
TEST_EXE	:= test_paintbot

TEST_SRC	:= test_paintbot.cpp

CC 			:= gcc
CCXX		:= g++
OBJS 		:= $(MAIN).o paintbot.o panelfloor.o
INCS		:= $(CONAN_INCLUDE_DIRS_GLIB) ../helper ../opcomp

CFLAGS 		?= -Wall -g
export CFLAGS

CXXFLAGS	?= --std=c++2a -Wall -g
export CXXFLAGS

LDFLAGS 	:= \
			   $(foreach d, $(CONAN_LIB_DIRS_GLIB), -L$d) \
			   $(foreach d, $(CONAN_LIBS_GLIB), -l$d)

GTEST 		:= 	$(foreach d, $(CONAN_LIB_DIRS_GTEST), -L$d) \
				$(foreach d, $(CONAN_LIBS_GTEST), -l$d) \
				$(foreach d, $(CONAN_INCLUDE_DIRS_GTEST), -I$d)

INCSFLAGS 	:= $(foreach d, $(INCS), -I$d)

all: opcomp main

opcomp:
	$(MAKE) -C ../opcomp

main: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCSFLAGS) $(OBJS) ../opcomp/opcomp.o -o $(MAIN)

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCSFLAGS) -c $< -o $@

test: opcomp
	$(CCXX) $(CXXFLAGS) $(LDFLAGS) $(INCSFLAGS) $(GTEST) \
		-pthread -lpthread ../opcomp/opcomp.o $(TEST_SRC) -o $(TEST_EXE)

run-test: test
	./$(TEST_EXE)

.PHONY: main clean test 
clean:
	rm -f $(OBJS) $(MAIN) $(TEST_EXE)
	$(MAKE) -C ../opcomp clean