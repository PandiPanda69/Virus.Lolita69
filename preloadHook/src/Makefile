
CC=g++
CFLAGS=
LDFLAGS=
EXEC=testUnpacker

#all: $(EXEC)

$(EXEC): testUnpacker.o unpacker.o
	$(CC) testUnpacker.o unpacker.o -o testUnpacker $(CFLAGS)


#	$(CC) -o test3 fileCounter.o virusChecker.o $(LDFLAGS)

testUnpacker.o: testUnpacker.cpp unpacker.h
	$(CC) -o testUnpacker.o -c testUnpacker.cpp $(CFLAGS)

unpacker.o: unpacker.cpp 
	$(CC) -o unpacker.o -c unpacker.cpp $(CFLAGS)

clean:
	rm -rf *.o
