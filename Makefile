CC=g++
CFLAGS=-lrt -std=c++11

all: writer reader
	echo ""

writer: writer.cpp
	$(CC) -o $@ $^ $(CFLAGS)

reader: reader.cpp
	$(CC) -o $@ $^ $(CFLAGS)
