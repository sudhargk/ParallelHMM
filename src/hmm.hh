#ifndef HMM_HH_
#define HMM_HH_

#include <iostream>
#include<cstdlib>
#include<math.h>
#include "matrix.hh"
#include "sequence.hh"

class HMM{
	size_t noOfStates;
	size_t noOfSymbols;	
	void* evalV(void*);
	void* evalM(void*);
public:
	Matrix piMat;
	Matrix transMat;
	vector<Matrix> emissionMat;
	vector<Matrix> transientC;

	void set(size_t noOfStates, size_t noOfSymbols){
			this->noOfStates=noOfStates;
			this->noOfSymbols=noOfSymbols;
			this->piMat.allocate(1,noOfSymbols);
			this->transMat.allocate(noOfStates,noOfStates);
			this->emissionMat.resize(noOfSymbols);
			for(int index=0;index<noOfSymbols;index++){
				this->emissionMat[index].allocate(1,noOfStates);
			}
	}
	void precomputeTransientC();
	void print();
	//Evaluation problem 1
	double evaluate(const Sequence sequence);
	//Viterbit Alignment problem 2
	void viterbiAlign(Sequence sequence);    
};

#endif
