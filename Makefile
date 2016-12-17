TARGET := sync

LDFLAGS :=

LIBS := -lssl -lcrypto

GXX := g++
LD := g++

CXXFLAGS := -Wall

SRC_DIR := src
INC_DIRS := -Iinc
BUILD_DIR := build

SRCS_C := $(wildcard $(SRC_DIR)/*.c)
SRCS_CPP := $(wildcard $(SRC_DIR)/*.cpp)
SRCS_BASE := $(patsubst %.c,%,$(SRCS_C))
SRCS_BASE := $(SRCS_BASE) $(patsubst %.cpp,%,$(SRCS_CPP))
BUILD_BASE := $(addprefix $(BUILD_DIR)/, $(notdir $(SRCS_BASE)))
DEPS := $(addsuffix .d,$(BUILD_BASE))
OBJS := $(addsuffix .o,$(BUILD_BASE))


.PHONY: all clean
.SUFFIXES:
.SECONDARY:

include $(DEPS)

all: $(DEPS) $(TARGET)

build:
				mkdir -p $@

$(TARGET): $(OBJS) | build
				@echo "----> Building	 $@"
				$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c | build
				@echo "----> dependency $<"
				$(GXX) $(CXXFLAGS) $(INC_DIRS) -MM -MT$(@:.d=.o) -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | build
				@echo "----> compiling $<"
				$(GXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

$(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp | build
				@echo "----> dependency $<"
				$(GXX) $(CXXFLAGS) $(INC_DIRS) -MM -MT$(@:.d=.o) -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | build
				@echo "----> compiling $<"
				$(GXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

clean:
				@echo "----> Cleaning up"
				rm -rf src/*.o
				rm -rf src/*.d
				rm -rf $(BUILD_DIR) $(TARGET)

show:
	@echo 'TARGET      :' $(TARGET)
	@echo 'SRCS_C      :' $(SRCS_C)
	@echo 'SRCS_CPP    :' $(SRCS_CPP)
	@echo 'SOURCE_BASE :' $(SRCS_BASE)
	@echo 'BUILD_BASE  :' $(BUILD_BASE)
	@echo 'DEPS        :' $(DEPS)
	@echo 'OBJS        :' $(OBJS)
