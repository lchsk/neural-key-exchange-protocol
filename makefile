OBJS = main.o protocol.o util.o node.o config.o
CC = g++
DEBUG = -g
CFLAGS = $(DEBUG) -std=c++11
LIBS = 

all: kkk

kkk: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o kkk $(LIBS)

main.o: main.cpp
	$(CC) -c $(CFLAGS) main.cpp $(LIBS)

protocol.o: protocol.cpp
	$(CC) -c $(CFLAGS) protocol.cpp $(LIBS)

node.o: node.cpp
	$(CC) -c $(CFLAGS) node.cpp $(LIBS)

util.o: util.cpp
	$(CC) -c $(CFLAGS) util.cpp $(LIBS)

config.o: config.cpp
	$(CC) -c $(CFLAGS) config.cpp $(LIBS)

clean:
	rm -rf *o kkk