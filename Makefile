ifndef $(PLATFORM)
	ifeq ($(OS),Windows_NT)
			PLATFORM = Windows
	else
			PLATFORM := $(shell uname -s)
	endif
endif

ifndef $(ARCH)
	ARCH := x86_64
endif

LIBS_DIR            := lib
HEADERS_DIR         := include
BUILD_DIR           := build
EXEC_EXT            :=

ifeq ($(PLATFORM),Darwin)
	CC              := xcrun clang
	LIBS_DIR        := $(LIBS_DIR)/darwin
	DYNAMIC_EXT     := dylib
else ifeq ($(PLATFORM),Linux)
	CC              := clang
	LIBS_DIR        := $(LIBS_DIR)/linux/$(ARCH)
	DYNAMIC_EXT     := so
else ifeq ($(PLATFORM),Windows)
	CC              :=
	LIBS_DIR        := $(LIBS_DIR)/windows
	DYNAMIC_EXT     := dll
	EXEC_EXT        := .exe
endif

FLAGS               := -arch $(ARCH) -L$(LIBS_DIR) -L$(BUILD_DIR) -I$(HEADERS_DIR)

STATIC_LIB_TARGET   := $(BUILD_DIR)/libtvcontrol.a
STATIC_LIB_SRC      := libtvcontrol.c

SHARED_LIB_TARGET   := $(BUILD_DIR)/libtvcontrol.$(DYNAMIC_EXT)
SHARED_LIB_FLAGS    := -shared -Wl,-install_name,@loader_path/$(shell basename $(SHARED_LIB_TARGET))

STATIC_TOOL_TARGET  := $(BUILD_DIR)/tvcontrolutil_static$(EXEC_EXT)
STATIC_TOOL_FLAGS   := $(STATIC_LIB_TARGET)

TOOL_TARGET         := $(BUILD_DIR)/tvcontrolutil$(EXEC_EXT)
TOOL_SRC            := tvcontrolutil.c
TOOL_FLAGS          := -ltvcontrol

ifeq ($(PLATFORM),Darwin)
	SHARED_LIB_FLAGS  += -framework CoreFoundation -framework IOKit
	STATIC_TOOL_FLAGS += -framework CoreFoundation -framework IOKit
endif


.PHONEY: all clean
all: $(TOOL_TARGET) $(STATIC_TOOL_TARGET)


$(TOOL_TARGET): $(TOOL_SRC) | $(SHARED_LIB_TARGET)
	$(CC) -o $@ $(FLAGS) $(TOOL_FLAGS) $(TOOL_SRC)

$(STATIC_TOOL_TARGET): $(TOOL_SRC) | $(STATIC_LIB_TARGET)
	$(CC) -o $@ $(FLAGS) $(STATIC_TOOL_FLAGS) $(TOOL_SRC)

$(SHARED_LIB_TARGET): | $(STATIC_LIB_TARGET)
	$(CC) -o $@ $(FLAGS) $(SHARED_LIB_FLAGS) -Wl,-force_load,$(STATIC_LIB_TARGET)

$(patsubst %.a, %.o, $(STATIC_LIB_TARGET)): $(STATIC_LIB_SRC) | $(BUILD_DIR)
	$(CC) -c -o $@ $(FLAGS) $(STATIC_LIB_SRC)

$(STATIC_LIB_TARGET)_: $(LIBS_DIR)/libcyusbserial.a
	cp $^ $@

$(STATIC_LIB_TARGET): $(patsubst %.a, %.o, $(STATIC_LIB_TARGET)) | $(STATIC_LIB_TARGET)_
	ar -rcT $(STATIC_LIB_TARGET)_ $<
	mv $(STATIC_LIB_TARGET)_ $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)
