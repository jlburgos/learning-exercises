## C++ Compiler
CXX=clang++

## C++ standard
CXX_STD=-std=c++20

## Custom SFML includes/libraries
INC=-I/opt/homebrew/Cellar/sfml/2.6.1/include
LIB=-L/opt/homebrew/Cellar/sfml/2.6.1/lib

## SFML flags
SFML_FLAGS=-lsfml-graphics -lsfml-window -lsfml-system

## OPTIMIZATION
OPTIMIZATION_DEBUG=\
	-O0 \
	-g
OPTIMIZATION_RELEASE=\
	-O3

## Compiler flags to check "almost everything" because g++ doesn't have a "-Weverything-i-want" flag :P
## Notes: https://stackoverflow.com/questions/5088460/flags-to-enable-thorough-and-verbose-g-warnings
CXX_COMPILER_FLAGS=\
	-Wall \
	-Wextra \
	-Werror \
	-Wmaybe-uninitialized \
	-Wempty-body \
	-Warray-bounds \
	-pedantic \
	-pedantic-errors \
	-Wcast-align \
	-Wcast-qual \
	-Wconversion \
	-Wdisabled-optimization \
	-Wfloat-equal \
	-Wlong-long \
	-Wimport \
	-Winit-self \
	-Winline \
	-Wmissing-field-initializers \
	-Wmissing-format-attribute \
	-Wmissing-include-dirs \
	-Wmissing-noreturn \
	-Wpacked -Wpointer-arith \
	-Wredundant-decls \
	-Wshadow \
	-Wstack-protector \
	-Wstrict-aliasing=2 \
	-Wformat=2 \
	-Wformat-nonliteral \
	-Wformat-security \
	-Wformat-y2k \
	-Wswitch-enum \
	-Wvariadic-macros \
	-Wwrite-strings \
	-Wunreachable-code

## Identify OS
## Note: https://stackoverflow.com/questions/714100/os-detecting-makefile
OS_SPECIFIC_FLAGS=
ifeq ($(OS),Windows_NT)
    OS_SPECIFIC_FLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        OS_SPECIFIC_FLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            OS_SPECIFIC_FLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            OS_SPECIFIC_FLAGS += -D IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        OS_SPECIFIC_FLAGS += -D LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        OS_SPECIFIC_FLAGS += -D OSX
				## Switch from -Wmaybe-uninitialized to -Wuninitialized for OSX
	  		CXX_COMPILER_FLAGS := $(filter-out -Wmaybe-uninitialized,$(CXX_COMPILER_FLAGS))
	  		CXX_COMPILER_FLAGS += -Wuninitialized
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        OS_SPECIFIC_FLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        OS_SPECIFIC_FLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        OS_SPECIFIC_FLAGS += -D ARM
    endif
endif

## Make sure we compile for 64-bit
OS_SPECIFIC_FLAGS+=\
	-m64

## Aggregated compiler flags
OPTS=\
	$(CXX_STD) \
	$(CXX_COMPILER_FLAGS) \
	$(OS_SPECIFIC_FLAGS)

BIN_DIR   = bin
BUILD_DIR = build
DIRS      := $(BIN_DIR) $(BUILD_DIR)
OBJS_SRC  := $(wildcard *.cpp)
OBJS_O_DEBUG    := $(addprefix $(BUILD_DIR)/, $(OBJS_SRC:%.cpp=%.debug.o))
OBJS_O_RELEASE  := $(addprefix $(BUILD_DIR)/, $(OBJS_SRC:%.cpp=%.release.o))

## Parallel build
MAKEFLAGS += -j$(NPROCS)

## Set up final targets
## Note: https://semver.org/
BIN_NAME = demo
BUILD_VERSION   ?= 0.0.0
BIN_DEBUG_BIN   := $(BIN_DIR)/$(BIN_NAME)-$(BUILD_VERSION)-debug.bin
BIN_RELEASE_BIN := $(BIN_DIR)/$(BIN_NAME)-$(BUILD_VERSION)-release.bin
BIN_DEBUG_LNK   := $(BIN_NAME)-$(BUILD_VERSION)-debug.lnk
BIN_RELEASE_LNK := $(BIN_NAME)-$(BUILD_VERSION)-release.lnk

## User targets
default: debug-build release-build
	@echo "------------------------------------------------------------------------------"
	@echo "--- Built Executable: '$(BIN_DEBUG_LNK)' ---> '$(BIN_DEBUG_BIN)'"
	@echo "--- Built Executable: '$(BIN_RELEASE_LNK)' ---> '$(BIN_RELEASE_BIN)'"
	@echo "------------------------------------------------------------------------------"
debug-build: $(DIRS) $(BIN_DEBUG_LNK)
release-build: $(DIRS) $(BIN_RELEASE_LNK)

## Create bin and build directories
$(DIRS):
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(BUILD_DIR)

## Link to executables
$(BIN_DEBUG_LNK): $(BIN_DEBUG_BIN)
	@ln -s $(BIN_DEBUG_BIN) $(BIN_DEBUG_LNK) || true

$(BIN_RELEASE_LNK): $(BIN_RELEASE_BIN)
	@ln -s $(BIN_RELEASE_BIN) $(BIN_RELEASE_LNK) || true

## Compile final binary
$(BIN_DEBUG_BIN): $(OBJS_O_DEBUG)
	$(CXX) -o "$@" $(OBJS_O_DEBUG) $(OPTIMIZATION_DEBUG) $(LIB) $(SFML_FLAGS)

$(BIN_RELEASE_BIN): $(OBJS_O_RELEASE)
	$(CXX) -o "$@" $(OBJS_O_RELEASE) $(OPTIMIZATION_RELEASE) $(LIB) $(SFML_FLAGS)

## Compile individual CPP object files (debug & release)
$(BUILD_DIR)/%.debug.o: %.cpp $(DIRS)
	$(CXX) -c -o '$@' '$<' $(OPTIMIZATION_DEBUG) $(OPTS) $(INC)

$(BUILD_DIR)/%.release.o: %.cpp $(DIRS)
	$(CXX) -c -o '$@' '$<' $(OPTIMIZATION_RELEASE) $(OPTS) $(INC)

## Compile web-assembly version using docker container
## Note: Disabling compile pipeline experiments for now
#wasm:
#	docker run \
#		--rm \
#		--volume $(ROOT_DIR):/$(BUILD_DIR) \
#		emscripten/emsdk /bin/bash -c \
#			"$(PIP) install requests && $(EMXX) /$(BUILD_DIR)/$(SRC_DIR)/*.cpp -o /$(BUILD_DIR)/$(BIN_DIR)/$(BIN_NAME).html $(WASM_OPTS)"

PHONY += clean
clean:
	@$(RM) -r $(BUILD_DIR) $(BIN_DIR) $(wildcard *.lnk) 

## Set up phony targets
.PHONY := $(PHONY)
