CC = gcc
CXX = g++

INCDIR = 
INCDEP = 

CFLAGS = -g -Wall 
CXXFLAGS = -g -Wall 

LDFLAGS = -g

LDLIBS = -lm 

.PHONY: default
default: tsp

tsp: tsp.o tsp-random.o $(INCDEP)
	$(CC) $(CFLAGS) $(INCDIR) $(LDLIBS) -o tsp tsp.o tsp-random.o $(LDFLAGS)

tsp.o: ./src/tsp.c ./src/tsp.h
	$(CC) $(CFLAGS) $(INCDIR) -c ./src/tsp.c

tsp-random.o: ./src/tsp-random.c ./src/tsp-random.h ./src/tsp.h
	$(CC) $(CFLAGS) $(INCDIR) -c ./src/tsp-random.c

.PHONY: clean
clean:
	rm -rf *.o a.out core tsp *.dSYM

.PHONY: all
all: clean default

