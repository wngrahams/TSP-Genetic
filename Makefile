CC = gcc
CXX = g++

INCDIR = 
INCDEP = 

CFLAGS = -g -Wall 
CXXFLAGS = -g -Wall 

LDFLAGS = -g

LDLIBS = -lm

objs = tsp.o tsp-random.o tsp-sahc.o

.PHONY: default
default: tsp

tsp: $(objs) $(INCDEP)
	$(CC) $(CFLAGS) $(INCDIR) $(LDLIBS) -o tsp $(objs) $(LDFLAGS)

tsp.o: ./src/tsp.c ./src/tsp.h
	$(CC) $(CFLAGS) $(INCDIR) -c ./src/tsp.c

tsp-random.o: ./src/tsp-random.c ./src/tsp-random.h ./src/tsp.h
	$(CC) $(CFLAGS) $(INCDIR) -c ./src/tsp-random.c

tsp-sahc.o: ./src/tsp-sahc.c ./src/tsp-sahc.h ./src/tsp.h
	$(CC) $(CFLAGS) $(INCDIR) -c ./src/tsp-sahc.c

.PHONY: clean
clean:
	rm -rf *.o a.out core tsp *.dSYM

.PHONY: all
all: clean default

