include ../build/conanbuildinfo.mak
$(call CONAN_BASIC_SETUP)

CC 			:= gcc
CCXX		:= g++
OBJS 		:= opcomp.o
INCS		:= $(CONAN_INCLUDE_DIRS_GLIB) ../helper

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


all: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCSFLAGS) -c $< -o $@

test:
	$(CCXX) $(CXXFLAGS) $(LDFLAGS) $(INCSFLAGS) $(GTEST) -pthread -lpthread test_opcomp.cpp -o test_opcomp

run-test: test
	./test_opcomp

format:
	clang-format-11 -i test_opcomp.cpp 

.PHONY: clean
clean:
	rm -f test_opcomp $(OBJS)