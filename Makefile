
CFLAGS 		:= -O2
export CFLAGS

CXXFLAGS	?= --std=c++2a -O2
export CXXFLAGS

all: day01 day02 day05 day09

day%: pre-build
	$(MAKE) -C $@

opcomp: pre-build
	$(MAKE) -C opcomp 

conan:
	cd $(BUILD_DIR) && conan install ..

test:
	$(MAKE) -C opcomp run-test
	$(MAKE) -C day09 run-test 

.PHONY: pre-build clean
clean:
	$(MAKE) -C day01 clean
	$(MAKE) -C day02 clean
	$(MAKE) -C day05 clean
	$(MAKE) -C day09 clean