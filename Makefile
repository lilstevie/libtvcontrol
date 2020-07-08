ifndef $(PLATFORM)
    ifeq ($(OS),Windows_NT)
            PLATFORM    = Windows
    else
            PLATFORM    := $(shell uname -s)
    endif
endif

ifndef $(ARCH)
    ARCH                ?= $(shell uname -m)
endif

ifndef $(PREFIX)
    PREFIX              ?= /usr/local
endif
ifndef $(UDEV_RULES_DIR)
    UDEV_RULES_DIR      ?= /lib/udev/rules.d
endif

SED                     ?= sed

CFLAGS                  ?= -Wall -fPIC -O3

LIB_NAME                := libtvcontrol
TOOL_NAME               := tvcontrolutil

LIBS_DIR                := lib
HEADERS_DIR             := include
BUILD_DIR               := build
EXEC_EXT                :=

ifeq ($(PLATFORM),Darwin)
    CC                  := xcrun clang
    ARCH_EXTRA          := -arch $(ARCH)
    LIBS_DIR            := $(LIBS_DIR)/darwin
    DYNAMIC_EXT         := dylib
else ifeq ($(PLATFORM),Linux)
    CC                  ?= clang
    LIBS_DIR            := $(LIBS_DIR)/linux/$(ARCH)
    DYNAMIC_EXT         := so
else ifeq ($(PLATFORM),Windows)
    CC                  :=
    LIBS_DIR            := $(LIBS_DIR)/windows
    DYNAMIC_EXT         := dll
    EXEC_EXT            := .exe
endif

FLAGS                   := $(CFLAGS) $(ARCH_EXTRA) -L$(LIBS_DIR) -L$(BUILD_DIR) -I$(HEADERS_DIR)

STATIC_LIB_TARGET       := $(BUILD_DIR)/$(LIB_NAME).a
STATIC_LIB_OBJ          := $(STATIC_LIB_TARGET:.a=.o)

SHARED_LIB_TARGET       := $(BUILD_DIR)/$(LIB_NAME).$(DYNAMIC_EXT)
SHARED_LIB_FLAGS        := -shared

LIB_SRC                 := libtvcontrol.c
LIB_DEPS                := $(LIBS_DIR)/libcyusbserial.a

STATIC_TOOL_TARGET      := $(BUILD_DIR)/$(TOOL_NAME)_static$(EXEC_EXT)
STATIC_TOOL_FLAGS       :=

TOOL_TARGET             := $(BUILD_DIR)/$(TOOL_NAME)$(EXEC_EXT)
TOOL_SRC                := tvcontrolutil.c
TOOL_FLAGS              := -ltvcontrol -Wl,-rpath,'$$ORIGIN'

ifeq ($(PLATFORM),Darwin)
    FRAMEWORKS          := -framework CoreFoundation -framework IOKit
    SHARED_LIB_FLAGS    += -Wl,-install_name,@loader_path/$(shell basename $(SHARED_LIB_TARGET)) $(FRAMEWORKS)
    STATIC_TOOL_FLAGS   += $(FRAMEWORKS)
else ifeq ($(PLATFORM),Linux)
    LIBRARIES           := -lpthread
    SHARED_LIB_FLAGS    += $(LIBRARIES)
    SHARED_TOOL_FLAGS   += $(LIBRARIES)
    LIB_DEPS            += $(LIBS_DIR)/libusb-1.0.a $(LIBS_DIR)/libudev.a
    STATIC_TOOL_FLAGS   += $(LIBRARIES) $(LIBS_DIR)/libusb-1.0.a $(LIBS_DIR)/libudev.a
    UDEV_TARGET         := udev/39-libtvcontrol.rules
    UDEV_INSTALL_TARGET := install-udev
endif

.PHONEY: all clean
all: $(TOOL_TARGET) $(STATIC_TOOL_TARGET) $(UDEV_TARGET)

$(TOOL_TARGET): $(TOOL_SRC) | $(SHARED_LIB_TARGET)
	$(CC) -o $@ $(TOOL_SRC) $(FLAGS) $(TOOL_FLAGS) $(SHARED_TOOL_FLAGS)

$(STATIC_TOOL_TARGET): $(TOOL_SRC) | $(STATIC_LIB_TARGET)
	$(CC) -o $@ $(TOOL_SRC) $(FLAGS) $(STATIC_LIB_TARGET) $(STATIC_TOOL_FLAGS)

$(SHARED_LIB_TARGET): $(LIB_SRC) $(LIB_DEPS) | $(BUILD_DIR)
	$(CC) -o $@ $(FLAGS) $(SHARED_LIB_FLAGS) $^

$(STATIC_LIB_OBJ): $(LIB_SRC) | $(BUILD_DIR)
	$(CC) -c -o $@ $(FLAGS) $<

$(STATIC_LIB_TARGET): $(LIB_DEPS) | $(STATIC_LIB_OBJ)
	cp $< $@_
	ar -rcs $@_ $(STATIC_LIB_OBJ)
	mv $@_ $@
	rm -rf $(STATIC_LIB_OBJ)

$(BUILD_DIR):
	mkdir -p $@

edit = \
	$(SED) -r \
	-e 's|@usergroup[@]|${1}|g' \
	< $< > $@ || rm $@

.usergroup:
	@usergroup=""; \
	for I in plugdev input admin adm sudo; do \
		if test -n "`groups |grep $$I`"; then \
			usergroup=$$I; \
			break; \
		fi; \
	done; \
	if test -z "$$usergroup"; then \
		echo "*** WARNING: Could not find a commonly known group name that the user `id -un` is a member of. Using 'plugdev' - make sure the user gets added to this group! ***"; \
		usergroup=plugdev; \
	fi; \
	echo $$usergroup > $@

$(UDEV_TARGET): $(UDEV_TARGET).in | .usergroup
	$(call edit,$(shell cat .usergroup))

$(UDEV_INSTALL_TARGET): $(UDEV_TARGET)
	install -m 0644 $^ $(UDEV_RULES_DIR)/

install: install-tool install-lib $(UDEV_INSTALL_TARGET)

install-tool: $(TOOL_TARGET) $(STATIC_TOOL_TARGET)
	mkdir -p $(PREFIX)/bin
	install -m 0755 $(TOOL_TARGET) $(PREFIX)/bin/
	install -m 0755 $(STATIC_TOOL_TARGET) $(PREFIX)/bin/

install-lib: $(STATIC_LIB_TARGET) $(SHARED_LIB_TARGET)
	mkdir -p $(PREFIX)/lib
	install -m 0644 $(STATIC_LIB_TARGET) $(PREFIX)/lib/
	install -m 0755 $(SHARED_LIB_TARGET) $(PREFIX)/lib/

clean:
	rm -rf $(BUILD_DIR)
	rm -f udev/39-libtvcontrol.rules
