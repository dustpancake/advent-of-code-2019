include ../build/conanbuildinfo.mak
$(call CONAN_BASIC_SETUP)

MAIN		:= day05

CC 			:= gcc
OBJS 		:= $(MAIN).o
INCS		:= $(CONAN_INCLUDE_DIRS_GLIB) ../helper ../opcomp

CFLAGS 		?= -Wall -g
export CFLAGS

LDFLAGS 	:= \
			   $(foreach d, $(CONAN_LIB_DIRS_GLIB), -L$d) \
			   $(foreach d, $(CONAN_LIBS_GLIB), -l$d)

INCSFLAGS 	:= $(foreach d, $(INCS), -I$d)

all: opcomp main

opcomp:
	$(MAKE) -C ../opcomp

main: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCSFLAGS) $(OBJS) ../opcomp/opcomp.o -o $(MAIN)

%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCSFLAGS) -c $< -o $@

.PHONY: main clean
clean:
	rm -f $(OBJS) $(MAIN)
	$(MAKE) -C ../opcomp clean