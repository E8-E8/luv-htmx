#!/bin/bash

if ! tmux has-session -t c-make 2>/dev/null; then
  tmux new-session -d -s c-make
fi
tmux send-keys -t c-make "make all" C-m
