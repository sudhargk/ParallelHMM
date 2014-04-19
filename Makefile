#makefile
CXX=g++
OUTPUT=bin/
SRC=src/
TARGETS=matrixtest.o
all : $(TARGETS)
matrixtest.o : matrix.o
	$(CXX) $(SRC)matrixtest.c -o $(OUTPUT)$@
matrix.o : $(SRC)matrix.hh
	$(CXX) $(SRC)matrix.hh -o $(OUTPUT)$@
