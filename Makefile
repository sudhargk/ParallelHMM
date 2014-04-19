CXX=g++
CXXFLAGS=-Wall -Werror -std=c++0x
LDFLAGS=-lm
BIN=bin
SRC=src
OBJ=obj

$(BIN)/matrixtest: $(OBJ)/matrix.o $(OBJ)/matrixtest.o
	if [ ! -e $(BIN) ]; then mkdir bin; fi;
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ)/%.o : $(SRC)/%.cc $(SRC)/matrix.hh
	if [ ! -e $(OBJ) ]; then mkdir obj; fi;
	$(CXX) $(CXXFLAGS) -o $@ -c $<

