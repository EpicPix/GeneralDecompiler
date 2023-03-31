SRC = $(shell find src -name "*.c")
TESTS := $(shell find tests -name '*.test')

all: clean out/decompiler test

clean:
	rm -rf out/*

out/decompiler: $(SRC)
	gcc -o $@ $^ -g -O1 -Wall -Wextra -Wno-unused-parameter

test_runner: tests/test.c
	gcc tests/test.c -o out/test_runner

test: test_runner out/decompiler $(TESTS)
	out/test_runner $(TESTS)
