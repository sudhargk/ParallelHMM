#ifndef HMM_HH_
#define HMM_HH_

#include <iostream>
#include<cstdlib>
#include<math.h>
#include "matrix.hh"
#include "sequence.hh"

class HMM{
public:
	Matrix piMat;
	Matrix transMat;
	vector<Matrix> emissionMat;
	vector<Matrix> transientC;
	size_t noOfStates;
	size_t noOfSymbols;
	void set(size_t noOfStates, size_t noOfSymbols){
		this->noOfStates=noOfStates;
		this->noOfSymbols=noOfSymbols;
		this->piMat.allocate(noOfStates,1);
		this->transMat.allocate(noOfStates,noOfStates);
		//this->emissionMat.resize(noOfSymbols);
		for(int index=0;index<noOfSymbols;index++){
			this->emissionMat.push_back(Matrix(1,noOfStates));
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
