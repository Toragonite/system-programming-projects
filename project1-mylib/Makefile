CC=gcc
CFLAGS=-Wall -g

# Object files
OBJS=main.o list.o bitmap.o hash.o hex_dump.o parser.o store.o

# Header dependencies
DEPS=list.h bitmap.h hash.h hex_dump.h parser.h store.h debug.h limits.h round.h types.h

# Final binary
TARGET=testlib

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Individual source files
main.o: main.c $(DEPS)
	$(CC) $(CFLAGS) -c $<

hash.o: hash.c hash.h list.h types.h
	$(CC) $(CFLAGS) -c $<

list.o: list.c list.h parser.h store.h types.h
	$(CC) $(CFLAGS) -c $<

bitmap.o: bitmap.c bitmap.h limits.h round.h hex_dump.h parser.h store.h
	$(CC) $(CFLAGS) -c $<

hex_dump.o: hex_dump.c hex_dump.h round.h
	$(CC) $(CFLAGS) -c $<

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c $<

store.o: store.c store.h list.h hash.h bitmap.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: clean
