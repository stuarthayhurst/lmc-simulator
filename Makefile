CFLAGS += -Wall -Wextra -Werror -std=c++20 -march=native -O3

ifeq ($(DEBUG),true)
  CFLAGS += -g
endif

simulator: src/main.cpp src/*.hpp src/common/*.hpp
	$(CXX) "src/main.cpp" -o "$@" $(CFLAGS)

.PHONY: clean
clean:
	@rm -fv simulator
