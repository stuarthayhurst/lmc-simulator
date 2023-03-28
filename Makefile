CFLAGS += -Wall -Wextra -Werror -std=c11 -march=native -flto=auto
ifeq ($(FAST),true)
  CFLAGS += -Ofast
else
  CFLAGS += -O3
endif

ifeq ($(DEBUG),true)
  CFLAGS += -g
endif

simulator: simulator.c
	$(CC) "$@.c" -o "$@" $(CFLAGS)

.PHONY: clean
clean:
	@rm -fv simulator
