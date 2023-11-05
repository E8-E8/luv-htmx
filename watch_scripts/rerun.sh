#!/bin/bash

tmux send-keys -t c-make "kill -9 $(pidof run)" C-m
tmux send-keys -t c-run "./run" C-m
