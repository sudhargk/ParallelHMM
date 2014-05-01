#include "hmm.hh"
#include <iostream>
using namespace std;


void HMM::print(){
	cout<<"# of States "<<noOfStates<<endl;
	cout<<"# of Symbols "<<noOfSymbols<<endl;
	
	cout<<"Transition Probability"<<std::endl;
	cout<<transMat;
	cout<<"Emission Probability"<<endl;
	for(int idx=0;idx<noOfStates;idx++){
		cout<<emissionMat[idx];
	}
	cout<<"Pi"<<std::endl;
	cout<<piMat;
}

void HMM::precomputeTransientC(){
	for (int k=0;k<noOfSymbols;k++){	
		Matrix C(noOfStates,noOfStates);
		transMat.diagmult(emissionMat[k],C);
		transientC.push_back(C);
	}
}




