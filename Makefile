#makefile
CFLAGS = -c -O
CXX=g++
OUTPUT=bin/
SRC=src/
TEST=test/
TARGETS=matrixtest ioutilstest hmmtest
all : $(TARGETS)

hmmtest : hmmtest_c matrix_c hmm eval sequence
	$(CXX) $(OUTPUT)hmmtest_c $(OUTPUT)matrix_c $(OUTPUT)hmm $(OUTPUT)sequence $(OUTPUT)eval -o $(OUTPUT)$@

ioutilstest : matrix_c hmm ioutilstest_c sequence
	$(CXX) $(OUTPUT)ioutilstest_c $(OUTPUT)matrix_c $(OUTPUT)hmm $(OUTPUT)sequence -o $(OUTPUT)$@

hmmtest_c : 
	$(CXX) $(CFLAGS) $(TEST)hmmtest.cc -o $(OUTPUT)$@
ioutilstest_c :
	$(CXX) $(CFLAGS) $(TEST)ioutilstest.cc -o $(OUTPUT)$@

matrixtest : matrix_c matrixtest_c
	$(CXX) $(OUTPUT)matrixtest_c $(OUTPUT)matrix_c -o $(OUTPUT)$@
	
eval : $(SRC)hmm.hh $(SRC)evaluation.cc
	$(CXX) $(CFLAGS) $(SRC)evaluation.cc -o $(OUTPUT)$@

hmm : 	$(SRC)hmm.hh $(SRC)hmm.cc
	$(CXX) $(CFLAGS) $(SRC)hmm.cc -o $(OUTPUT)$@

sequence : $(SRC)sequence.hh $(SRC)sequence.cc
	$(CXX) $(CFLAGS) $(SRC)sequence.cc -o $(OUTPUT)$@
	  
matrixtest_c : $(SRC)matrix.hh $(TEST)matrixtest.c
	$(CXX) $(CFLAGS) $(TEST)matrixtest.c -o $(OUTPUT)$@

matrix_c : $(SRC)matrix.hh $(SRC)matrix.cc
	$(CXX) $(CFLAGS) $(SRC)matrix.cc -o $(OUTPUT)$@


