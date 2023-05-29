SRC := $(shell find src -name "*.c")
TESTS := $(shell find tests -name '*.test')

all: clean out/decompiler test

clean:
	mkdir -p out
	rm -rf out/*
	rm -f $(subst .test,.test.run,$(TESTS)) $(subst .test,.o,$(TESTS))
	rm -rf $(find -name "generated")

decompiler_debug: $(SRC)
	node srcgen/source_generator.js srcgen_data/ src/
	gcc -o out/decompiler_debug $^ -g -O1 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-missing-braces -DDEBUG

out/decompiler: $(SRC)
	node srcgen/source_generator.js srcgen_data/ src/
	gcc -o $@ $^ -g -O1 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-missing-braces

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
