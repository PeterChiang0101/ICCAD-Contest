ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    detected_OS := Windows
	remove_command := del
	output_name := testing.exe
else
    detected_OS := $(shell uname)  # same as "uname -s"
	remove_command := rm
	output_name := ./cade0025_beta
endif

separated: new_main.cpp VectorOp.o FileIO.o Buffer.o Silkscreen.o
	g++ -std=c++0x -Wall -o $(output_name) new_main.cpp VectorOp.o FileIO.o Buffer.o Silkscreen.o

VectorOp.o: VectorOp.cpp VectorOp.h
	g++ -std=c++0x -Wall -c -o VectorOp.o VectorOp.cpp

FileIO.o: FileIO.cpp FileIO.h
	g++ -std=c++0x -Wall -c -o FileIO.o FileIO.cpp

Buffer.o: Buffer.cpp Buffer.h
	g++ -std=c++0x -Wall -c -o Buffer.o Buffer.cpp

Silkscreen.o: Silkscreen.cpp Silkscreen.h
	g++ -std=c++0x -Wall -c -o Silkscreen.o Silkscreen.cpp

.PHONY: clean
clean:
	$(remove_command) $(output_name) *.o 