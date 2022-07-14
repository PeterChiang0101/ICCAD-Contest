ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    detected_OS := Windows
	remove_command := del
	output_name := testing.exe
else
    detected_OS := $(shell uname)  # same as "uname -s"
	remove_command := rm
	output_name := testing
endif

testcase1_dir = ./TestingCase/test_A.txt
testcase1_out_dir = ./TestingCase/test_A_Ans.txt
testcase2_dir = ./TestingCase/test_B.txt
testcase2_out_dir = ./TestingCase/test_B_Ans.txt
testcase3_dir = ./TestingCase/test_C.txt
testcase3_out_dir = ./TestingCase/test_C_Ans.txt

testcase_all: testcase1 testcase2 testcase3

testcase1: testing.exe
	$(output_name) $(testcase1_dir) $(testcase1_out_dir)
testcase2: testing.exe
	$(output_name) $(testcase2_dir) $(testcase2_out_dir)
testcase3: testing.exe
	$(output_name) $(testcase3_dir) $(testcase3_out_dir)

testing.exe:main.cpp
	g++ -std=c++0x -Wall -o $(output_name) main.cpp 

scorer.o:scorer.cpp scorer.h 
	g++ -c -Wall -o scorer.o scorer.cpp 

inputoutput.o:inputoutput.cpp inputoutput.h
	g++ -c -Wall -o inputoutput.o inputoutput.cpp

Segment.o:Segment.cpp Segment.h
	g++ -c -Wall -o Segment.o Segment.cpp

.PHONY: clean
clean:
	$(remove_command) testing.exe scorer.o Segment.o