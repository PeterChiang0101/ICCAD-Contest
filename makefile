ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    detected_OS := Windows
	remove_command := del
	output_name := testing.exe
else
    detected_OS := $(shell uname)  # same as "uname -s"
	remove_command := rm
	output_name := testing
endif

testing.exe:main.cpp scorer.o Segment.o
	g++ -Wall -o $(output_name) main.cpp

scorer.o:scorer.cpp scorer.h
	g++ -c -Wall -o scorer.o scorer.cpp

Segment.o:Segment.cpp Segment.h
	g++ -c -Wall -o Segment.o Segment.cpp

.PHONY: clean
clean:
	$(remove_command) testing.exe scorer.o Segment.o