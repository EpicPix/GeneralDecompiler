SRC := $(shell find src -name "*.c")
TESTS := $(shell find tests -name '*.*.test')

all: clean out/decompiler test

clean:
	mkdir -p out
	rm -rf out/*
	rm -f $(subst .test,,$(TESTS))

out/decompiler: $(SRC)
	gcc -o $@ $^ -g -O1 -Wall -Wextra -Wno-unused-parameter

out/test_runner: tests/test.c
	gcc tests/test.c -o out/test_runner -g -O1 -Wall -Wextra -Wno-unused-parameter

tests/%.test: tests/%.asm
	nasm -felf64 $? -o $(subst .test,,$@)

test: out/test_runner out/decompiler $(TESTS)
	out/test_runner $(TESTS)
