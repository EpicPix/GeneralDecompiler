SRC := $(shell find src -name "*.c")
TESTS := $(shell find tests -name '*.test')

all: clean out/decompiler test

clean:
	mkdir -p out
	rm -rf out/*
	rm -f $(subst .test,.test.run,$(TESTS)) $(subst .test,.o,$(TESTS))

out/decompiler: $(SRC)
	gcc -o $@ $^ -g -O1 -Wall -Wextra -Wno-unused-parameter

out/test_runner: tests/test.c
	gcc tests/test.c -o out/test_runner -g -O1 -Wall -Wextra -Wno-unused-parameter

tests/%_x86_elf.test: .FORCE
	nasm -felf64 $(subst .test,.asm,$@) -o $(subst .test,.o,$@)
	ld -o $(subst .test,.test.run,$@) $(subst .test,.o,$@)

tests/%_jvm.test: .FORCE
	javac $(subst .test,.java,$@)
	mv $(subst .test,.class,$@) $(subst .test,.test.run,$@)

test: out/test_runner out/decompiler $(TESTS)
	out/test_runner $(TESTS)

.FORCE:
