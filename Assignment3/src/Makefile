CC = gcc
CFLAGS = -Wall -Wextra

SOURCES = main.c scheduler.c fcfs.c priority.c round_robin.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = sim

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)