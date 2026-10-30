ifeq ($(origin CC),default)
	CC := $(if $(shell command -v clang 2>/dev/null),clang,gcc)
endif
ifeq ($(origin LINKER),undefined)
	LINKER := $(if $(shell command -v mold 2>/dev/null),mold,$(if $(shell command -v lld 2>/dev/null),lld,bfd))
endif
ifeq ($(origin CCACHE),undefined)
	CCACHE := $(if $(shell command -v ccache 2>/dev/null),ccache)
endif
STRIP := strip

.PHONY: info
info:
	@echo -e "Selected compiler: $(CC)"
	@echo -e "Selected linker: $(LINKER)"
	@echo -e "Compiler cache: $(if $(CCACHE),$(CCACHE),none)"
	@echo -e "Available targets:"
	@echo -e "    deb: do a debug build"
	@echo -e "    san: do a sanitizer build"
	@echo -e "    rel: do a release build"
	@echo -e "    clean: clean source tree"
	@echo -e "    info: print this info message"

CFLAGS_COMMON := \
	-std=c23 \
	-Wall -Wextra -Wpedantic -pedantic-errors -Werror \
	-Wshadow -Wconversion -Wsign-conversion \
	-Wcast-align -Wcast-qual \
	-Wnull-dereference \
	-Wmissing-prototypes -Wmissing-declarations \
	-Wstrict-prototypes \
	-Wold-style-definition \
	-Wdouble-promotion \
	-Wformat=2 \
	-Wundef -Wvla \
	-Wpointer-arith -Wwrite-strings -Wswitch-enum \
	-Winit-self \
	-D_POSIX_C_SOURCE=200809L

CFLAGS_OPT0 := -O0
CFLAGS_OPT1 := -O1
CFLAGS_OPT2 := -O2
CFLAGS_OPT3 := -O3

CFLAGS_DEBUG := \
	-g3 \
	-fno-omit-frame-pointer \
	-fno-optimize-sibling-calls

CFLAGS_SAN := \
	-fsanitize=address,undefined \
	-fno-sanitize-recover=undefined

CFLAGS_RELEASE := \
	-DNDEBUG \
	-U_FORTIFY_SOURCE \
	-D_FORTIFY_SOURCE=3 \
	-fstack-protector-strong \
	-fstack-clash-protection \
	-fcf-protection=full

CFLAGS_LTO := -flto=auto

LDFLAGS_COMMON := -fuse-ld=$(LINKER)
LDFLAGS_SAN := -fsanitize=address,undefined
LDFLAGS_RELEASE := -Wl,-z,relro,-z,now
INCLUDEDIR := -Iinclude
DEPS_FLAGS := -MMD -MP

BUILDDIR := build
LIBDIR := lib
SRCDIR := src
DEBDIR := deb
SANDIR := san
RELDIR := rel
TARGET_DEB := sohomot_main_deb
TARGET_SAN := sohomot_main_san
TARGET_REL := sohomot_main
VERSION_MAJOR := 0
VERSION_MINOR := 1
VERSION_PATCH := 0

SRC_ALL := $(wildcard $(SRCDIR)/*.c)
SRCS_EXCLUDE :=
SRCS := $(filter-out $(SRCS_EXCLUDE), $(SRC_ALL))
SRCS_DEB_OBJ := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/$(DEBDIR)/%.c.o, $(SRCS))
SRCS_SAN_OBJ := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/$(SANDIR)/%.c.o, $(SRCS))
SRCS_REL_OBJ := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/$(RELDIR)/%.c.o, $(SRCS))

$(BUILDDIR)/$(DEBDIR)/%.c.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CCACHE) $(CC) $(CFLAGS_COMMON) $(CFLAGS_OPT0) $(CFLAGS_DEBUG) \
		$(INCLUDEDIR) -c -fPIC $(DEPS_FLAGS) \
		$< -o $@

TARGET_DEB_SO := lib$(TARGET_DEB).so
TARGET_DEB_FULL := $(TARGET_DEB_SO).$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)
$(BUILDDIR)/$(DEBDIR)/$(LIBDIR)/$(TARGET_DEB_FULL): $(SRCS_DEB_OBJ)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS_COMMON) -shared \
	-Wl,-soname,$(TARGET_DEB_SO).$(VERSION_MAJOR) \
	-Wl,-z,defs \
	$^ -o $@
	@ln -sf $(TARGET_DEB_FULL) \
		$(BUILDDIR)/$(DEBDIR)/$(LIBDIR)/$(TARGET_DEB_SO).$(VERSION_MAJOR)
	@ln -sf $(TARGET_DEB_FULL) \
		$(BUILDDIR)/$(DEBDIR)/$(LIBDIR)/$(TARGET_DEB_SO)

.PHONY: deb
deb: $(BUILDDIR)/$(DEBDIR)/$(LIBDIR)/$(TARGET_DEB_FULL)

-include $(SRCS_DEB_OBJ:.o=.d)

$(BUILDDIR)/$(SANDIR)/%.c.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CCACHE) $(CC) $(CFLAGS_COMMON) $(CFLAGS_OPT0) $(CFLAGS_DEBUG) $(CFLAGS_SAN) \
		$(INCLUDEDIR) -c -fPIC $(DEPS_FLAGS) \
		$< -o $@

TARGET_SAN_SO := lib$(TARGET_SAN).so
TARGET_SAN_FULL := $(TARGET_SAN_SO).$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)
$(BUILDDIR)/$(SANDIR)/$(LIBDIR)/$(TARGET_SAN_FULL): $(SRCS_SAN_OBJ)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS_COMMON) $(LDFLAGS_SAN) -shared \
	-Wl,-soname,$(TARGET_SAN_SO).$(VERSION_MAJOR) \
	$^ -o $@
	@ln -sf $(TARGET_SAN_FULL) \
		$(BUILDDIR)/$(SANDIR)/$(LIBDIR)/$(TARGET_SAN_SO).$(VERSION_MAJOR)
	@ln -sf $(TARGET_SAN_FULL) \
		$(BUILDDIR)/$(SANDIR)/$(LIBDIR)/$(TARGET_SAN_SO)

.PHONY: san
san: $(BUILDDIR)/$(SANDIR)/$(LIBDIR)/$(TARGET_SAN_FULL)

-include $(SRCS_SAN_OBJ:.o=.d)

$(BUILDDIR)/$(RELDIR)/%.c.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CCACHE) $(CC) $(CFLAGS_COMMON) $(CFLAGS_OPT3) $(CFLAGS_RELEASE) $(CFLAGS_LTO) \
		$(INCLUDEDIR) -c -fPIC $(DEPS_FLAGS) \
		$< -o $@

TARGET_REL_SO := lib$(TARGET_REL).so
TARGET_REL_FULL := $(TARGET_REL_SO).$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)
$(BUILDDIR)/$(RELDIR)/$(LIBDIR)/$(TARGET_REL_FULL): $(SRCS_REL_OBJ)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_OPT3) $(LDFLAGS_COMMON) $(LDFLAGS_RELEASE) $(CFLAGS_LTO) \
	-shared \
	-Wl,-soname,$(TARGET_REL_SO).$(VERSION_MAJOR) \
	-Wl,-z,defs \
	$^ -o $@
	$(STRIP) -s $@
	@ln -sf $(TARGET_REL_FULL) \
		$(BUILDDIR)/$(RELDIR)/$(LIBDIR)/$(TARGET_REL_SO).$(VERSION_MAJOR)
	@ln -sf $(TARGET_REL_FULL) \
		$(BUILDDIR)/$(RELDIR)/$(LIBDIR)/$(TARGET_REL_SO)

.PHONY: rel
rel: $(BUILDDIR)/$(RELDIR)/$(LIBDIR)/$(TARGET_REL_FULL)

-include $(SRCS_REL_OBJ:.o=.d)

.PHONY: clean
clean:
	@echo "Cleaning source tree..."
	@rm -rf .cache/
	@rm -rf build/
	@rm -f compile_commands.json
	@find . -name "*.o" -delete
	@echo "Cleaned."
