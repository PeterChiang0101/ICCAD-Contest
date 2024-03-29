ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    detected_OS := Windows
	remove_command := del
	output_name := testing.exe
else
    detected_OS := $(shell uname)  # same as "uname -s"
	remove_command := rm
	output_name := ./cade0025_final
endif

testcase1_dir = ./TestingCase/test_A.txt
testcase1_out_dir = ./TestingCase/test_A_Ans.txt
testcase2_dir = ./TestingCase/test_B.txt
testcase2_out_dir = ./TestingCase/test_B_Ans.txt
testcase3_dir = ./TestingCase/test_C.txt
testcase3_out_dir = ./TestingCase/test_C_Ans.txt
testcase4_dir = ./TestingCase/test_1.txt
testcase4_out_dir = ./TestingCase/test_1_Ans.txt
testcase5_dir = ./TestingCase/test_2.txt
testcase5_out_dir = ./TestingCase/test_2_Ans.txt

testcase_all: testcase1 testcase2 testcase3 #testcase4 testcase5

testcase1: separated
	$(output_name) $(testcase1_dir) $(testcase1_out_dir)
testcase2: separated
	$(output_name) $(testcase2_dir) $(testcase2_out_dir)
testcase3: separated
	$(output_name) $(testcase3_dir) $(testcase3_out_dir)
testcase4: separated
	$(output_name) $(testcase4_dir) $(testcase4_out_dir)
testcase5: separated
	$(output_name) $(testcase5_dir) $(testcase5_out_dir)

separated: new_main.o VectorOp.o FileIO.o Buffer.o Silkscreen.o Point.o Graph.o scorer.o
	g++ -std=c++0x -Wall -o $(output_name) new_main.o ./classes/scorer.o ./classes/VectorOp.o ./classes/FileIO.o ./classes/Buffer.o ./classes/Silkscreen.o ./classes/Point.o ./classes/Graph.o

new_main.o: new_main.cpp
	g++ -std=c++0x -Wall -c -o new_main.o new_main.cpp

scorer.o: ./classes/scorer.cpp
	g++ -std=c++0x -Wall -c -o ./classes/scorer.o ./classes/scorer.cpp

VectorOp.o: ./classes/VectorOp.cpp ./classes/VectorOp.h
	g++ -std=c++0x -Wall -c -o ./classes/VectorOp.o ./classes/VectorOp.cpp

FileIO.o: ./classes/FileIO.cpp ./classes/FileIO.h
	g++ -std=c++0x -Wall -c -o ./classes/FileIO.o ./classes/FileIO.cpp

Buffer.o: ./classes/Buffer.cpp ./classes/Buffer.h
	g++ -std=c++0x -Wall -c -o ./classes/Buffer.o ./classes/Buffer.cpp

Silkscreen.o: ./classes/Silkscreen.cpp ./classes/Silkscreen.h
	g++ -std=c++0x -Wall -c -o ./classes/Silkscreen.o ./classes/Silkscreen.cpp

testing.exe:main.cpp
	g++ -std=c++0x -Wall -o $(output_name) main.cpp 

inputoutput.o:inputoutput.cpp inputoutput.h
	g++ -c -Wall -o inputoutput.o inputoutput.cpp

Segment.o:Segment.cpp Segment.h
	g++ -c -Wall -o Segment.o Segment.cpp

Graph.o: ./classes/Graph.cpp ./classes/Graph.h
	g++ -std=c++0x -Wall -c -o ./classes/Graph.o ./classes/Graph.cpp

Point.o: ./classes/Point.cpp ./classes/Point.h
	g++ -std=c++0x -Wall -c -o ./classes/Point.o ./classes/Point.cpp

.PHONY: clean
clean:
	$(remove_command) $(output_name) scorer.o Segment.o .\classes\*.o 