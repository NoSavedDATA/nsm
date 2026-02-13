CXX := clang++-19 -std=c++17
CXXFLAGS := -O3 -rdynamic -march=native -fno-exceptions
LLVM_CONFIG := llvm-config-19 --link-static --libs core orcjit native
SYSTEM_LIBS := -ldl -lrt -pthread
OTHER_FLAGS := -D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH -flto -finline-functions -funroll-loops -w

# Get LLVM flags (must be from static LLVM build)
LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)
LLVM_LDFLAGS := $(shell $(LLVM_CONFIG) --ldflags)
LLVM_SYSTEM_LIBS := $(shell $(LLVM_CONFIG) --system-libs)
LLVM_LIBS := $(shell $(LLVM_CONFIG) --libs core orcjit native)

SYS_LIBS += -Wl,--disable-new-dtags \
           -Wl,-rpath,'$$ORIGIN/../sys_lib'

# Combine all flags
CXXFLAGS += $(LLVM_CXXFLAGS) -mavx -w
LDFLAGS := $(LLVM_LDFLAGS) -static-libstdc++ -static-libgcc
LDFLAGS += $(SYS_LIBS)
LIBS := $(LLVM_LIBS) $(LLVM_SYSTEM_LIBS) $(SYSTEM_LIBS)



# Directories
OBJ_DIR = obj
BIN_DIR = bin
SRC_DIR = src
LIB_DIR := obj_static



# C++ Source and Object Files
CXX_SRC = $(shell find $(SRC_DIR) -name "*.cpp")
CXX_OBJ = $(CXX_SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
CXX_DIR = $(sort $(dir $(CXX_OBJ)))

OBJ_DIRS := $(sort $(CXX_DIR))




# Executable name
LIB_PARSER := bin/lib_parser.o
OBJ := bin/nsm
SRC := main.cpp


BUILD_FLAG := .build_flag



$(info var is: ${OBJ_DIRS})
$(foreach dir, $(OBJ_DIRS), \
  $(info var is: $(dir)) \
  $(shell mkdir -p $(dir)); \
)

$(shell mkdir -p $(BIN_DIR);)
$(shell mkdir -p $(LIB_DIR);)






all: $(CXX_OBJ) $(OBJ) check_done


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<


$(OBJ): $(SRC) $(CXX_OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SRC) $(CXX_OBJ) $(LIBS) $(OTHER_FLAGS) -MMD -MP -o $(OBJ) 
	@echo "\033[1;32m\nBuild completed [✓]\n\033[0m"
	@touch $(BUILD_FLAG)



check_done:
	@if [ ! -f $(BUILD_FLAG) ]; then \
		echo "\n\n\033[1;33mNo changes found [ ]\n\033[0m"; \
	fi
	@rm -f $(BUILD_FLAG)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

# Track dependencies
-include $(CXX_OBJ:.o=.d)
