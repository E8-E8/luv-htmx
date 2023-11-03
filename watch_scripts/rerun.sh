#!/bin/bash

if ! tmux has-session -t c-run 2>/dev/null; then
  tmux new-session -d -s c-run
fi

tmux send-keys -t c-make "kill -9 $(pidof run)" C-m
tmux send-keys -t c-run "./run" C-m
