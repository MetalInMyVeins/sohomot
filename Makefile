CC ?= clang
LD ?= ld.lld

CFLAGS_COMMON := \
	-std=c23 \
	-Wall -Wextra -Wpedantic -pedantic-errors -Werror \
	-Wshadow \
	-Wconversion -Wsign-conversion \
	-Wcast-align -Wcast-qual \
	-Wnull-dereference \
	-Wmissing-prototypes -Wmissing-declarations \
	-Wstrict-prototypes \
	-Wold-style-definition \
	-Wdouble-promotion \
	-Wformat=2 \
	-Wundef \
	-Wvla \
	-Wpointer-arith \
	-Wwrite-strings \
	-Wswitch-enum \
	-Winit-self

CFLAGS_OPT0 := \
	-O0

CFLAGS_OPT1 := \
	-O1

CFLAGS_OPT2 := \
	-O2

CFLAGS_OPT3 := \
	-O3

CFLAGS_DEBUG := \
	-g3 \
	-fno-omit-frame-pointer \
	-fno-optimize-sibling-calls

CFLAGS_SAN := \
	-fsanitize=address,undefined \
	-fno-sanitize-recover=undefined

CFLAGS_RELEASE := \
	-DNDEBUG \
	-D_FORTIFY_SOURCE=3 \
	-fstack-protector-strong \
	-fstack-clash-protection \
	-fcf-protection=full

LDFLAGS_COMMON := \
	-fuse-ld=lld

LDFLAGS_SAN := \
	-fsanitize=address,undefined

LDFLAGS_RELEASE := \
	-Wl,-z,relro,-z,now

INCLUDEDIR := \
	-Iinclude

BUILDDIR := build
TARET_DEB := $(BUILDDIR)/sohomot_main
TARET_SAN := $(BUILDDIR)/sohomot_main_san

$(TARET_DEB): src/main.c src/suite.c
	@mkdir -p build/
	$(CC) $(CFLAGS_COMMON) $(CFLAGS_OPT0) $(CFLAGS_DEBUG) \
		$(LDFLAGS_COMMON) $(INCLUDEDIR) $^ -o $@

.PHONY: deb
deb: $(TARET_DEB)

$(TARET_SAN): src/main.c src/suite.c
	@mkdir -p build/
	$(CC) $(CFLAGS_COMMON) $(CFLAGS_OPT0) $(CFLAGS_DEBUG) $(CFLAGS_SAN) \
		$(LDFLAGS_COMMON) $(LDFLAGS_SAN) $(INCLUDEDIR) \
		$^ -o $@

.PHONY: san
san: $(TARET_SAN)

.PHONY: clean
clean:
	rm -rf .cache/
	rm -rf build/
	find . -name "*.o" -delete
	rm -f compile_commands.json
