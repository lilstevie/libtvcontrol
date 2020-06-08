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
	CC              := clang
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

LIB_TARGET          := $(BUILD_DIR)/libtvcontrol.$(DYNAMIC_EXT)
LIB_SRC             := libtvcontrol.c
LIB_FLAGS           := -shared -lcyusbserial.0.1 -Wl,-install_name,@loader_path/$(shell basename $(LIB_TARGET))

STATIC_LIB_TARGET   := $(BUILD_DIR)/libtvcontrol.a
STATIC_LIB_FLAGS    := $(LIBS_DIR)/libcyusbserial.a

TOOL_TARGET         := $(BUILD_DIR)/tvcontrolutil$(EXEC_EXT)
TOOL_SRC            := tvcontrolutil.c
TOOL_FLAGS          := -ltvcontrol

STATIC_TOOL_TARGET  := $(BUILD_DIR)/tvcontrolutil_static$(EXEC_EXT)
STATIC_TOOL_FLAGS   := $(STATIC_LIB_TARGET)

DEPS                := $(patsubst $(LIBS_DIR)/%, $(BUILD_DIR)/%, $(wildcard $(LIBS_DIR)/*.$(DYNAMIC_EXT)))

.PHONEY: all clean
all: $(TOOL_TARGET) $(LIB_TARGET) #$(STATIC_TOOL_TARGET) $(STATIC_LIB_TARGET)

$(TOOL_TARGET): $(TOOL_SRC) $(LIB_TARGET) | $(BUILD_DIR) $(DEPS)
	$(CC) -o $@ $(FLAGS) $(TOOL_FLAGS) $(TOOL_SRC)

$(STATIC_TOOL_TARGET): $(TOOL_SRC) $(STATIC_LIB_TARGET) | $(BUILD_DIR)
	$(CC) -o $@ $(FLAGS) $(STATIC_TOOL_FLAGS) $(TOOL_SRC)

$(LIB_TARGET): $(LIB_SRC) | $(BUILD_DIR)
	$(CC) -o $@ $(FLAGS) $(LIB_FLAGS) $(LIB_SRC)

$(patsubst %.a, %.o, $(STATIC_LIB_TARGET)): $(LIB_SRC) | $(BUILD_DIR)
	$(CC) -c -o $@ $(FLAGS) $(STATIC_LIB_FLAGS) $(LIB_SRC)

%.a: %.o
	ar rcs $@ $^

$(BUILD_DIR)/%.dylib: $(LIBS_DIR)/%.dylib
	cp $^ $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)
