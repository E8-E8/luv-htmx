# Compiler
CC := gcc

SRC_DIR := src
LIB_DIR := lib
LIBS := -luv

SRC_FILES := $(shell find $(SRC_DIR) -name "*.c")

CFLAGS := -Wall -Wextra -pedantic -g

EXECUTABLE := run

$(EXECUTABLE): $(SRC_FILES)
	$(CC) $(CFLAGS) $^ -o $@ -L$(LIB_DIR) $(LIBS)

all: $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

# Watch for changes and rebuild
watch:
	tmux send-keys -t c-run "./run" C-m
	while inotifywait -r -e modify,create,delete ./src; do \
		./watch_scripts/recompile.sh & \
		./watch_scripts/rerun.sh & \
	done
