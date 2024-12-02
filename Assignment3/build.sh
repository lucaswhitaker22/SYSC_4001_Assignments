#!/usr/bin/env bash
cd "$(dirname "$0")/src"



CC = gcc
CFLAGS = -Wall -Wextra

SOURCES = main.c scheduler.c fcfs.c priority.c round_robin.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = scheduler

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)