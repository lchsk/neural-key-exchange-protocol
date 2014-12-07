OBJS = main.o protocol.o
CC = g++
DEBUG = 
CFLAGS = $(DEBUG) -std=c++11
LIBS = 

all: kkk

kkk: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o kkk $(LIBS)

main.o: main.cpp
	$(CC) -c $(CFLAGS) main.cpp $(LIBS)

protocol.o: protocol.cpp
	$(CC) -c $(CFLAGS) protocol.cpp $(LIBS)

clean:
	rm -rf *o kkk