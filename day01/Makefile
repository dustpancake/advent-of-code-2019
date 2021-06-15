include ../build/conanbuildinfo.mak
$(call CONAN_BASIC_SETUP)

MAIN		:= day01

CC 			:= gcc
INCS		:= $(CONAN_INCLUDE_DIRS_GLIB) ../helper

CFLAGS 		?= -Wall -g
LDFLAGS 	:= \
			   $(foreach d, $(CONAN_LIB_DIRS_GLIB), -L$d) \
			   $(foreach d, $(CONAN_LIBS_GLIB), -l$d)

INCSFLAGS 	:= $(foreach d, $(INCS), -I$d)

all: main

main:
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCSFLAGS) $(MAIN).c -o $(MAIN)

.PHONY: main clean
clean:
	rm -f $(MAIN)