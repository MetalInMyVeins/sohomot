CC ?= clang
LD ?= ld.lld

CFLAGS := \
	-g \
	-std=c23 \
	-Wall -Wextra -Werror -pedantic \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-unused-but-set-variable
	-Iinclude

BUILD := build

build/sohomot_main: src/main.c
	@mkdir -p build/
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -rf .cache/
	rm -rf build/
	find . -name "*.o" -delete
	rm -f compile_commands.json
