CXXFLAGS += -Wall -Wextra -Werror -Wpedantic -std=c++20 -march=native -O3 -flto=auto
BUILD_DIR ?= build

ifeq ($(DEBUG),true)
  CXXFLAGS += -g
endif

OBJECTS := $(wildcard ./src/*.cpp) $(wildcard ./src/*/*.cpp)
OBJECTS := $(subst .cpp,.o,$(subst ./src/,$(BUILD_DIR)/,$(OBJECTS)))
HEADERS = $(wildcard ./src/*.hpp) $(wildcard ./src/*/*.hpp)

simulator: $(OBJECTS) $(HEADERS)
	$(CXX) -o "simulator" $(OBJECTS) $(CXXFLAGS)

$(BUILD_DIR)/%.o: ./src/%.cpp $(HEADERS)
	@mkdir -p "$$(dirname $@)"
	$(CXX) $< -c $(CXXFLAGS) -o "$@"

.PHONY: clean
clean:
	@rm -fv simulator
	@rm -rfv "$(BUILD_DIR)"
