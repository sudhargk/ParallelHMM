#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<sstream>
#include<stdlib.h>
#include "matrix.hh"
#include "hmm.hh"
#include "sequence.hh"

using namespace std;

void readConfigFile(string configFile,HMM& model){
	size_t noOfStates;
	size_t noOfSymbols;
	string line;
	cout<<"Configuration File : "<<configFile<<endl;
	ifstream input;
	input.open( configFile.c_str() );
	
	int k = 0;
	while( k < 2 ){
		string temp;
		int i = 0;
		getline( input, line, '\n');
		//cout<<line<<endl;
		istringstream iss(line);
		while( iss >> temp ){
			if( temp.compare("states:") ==0 || temp.compare("States:") ==0 )
			{
				iss >> noOfStates;
				continue;
			}
			else if
				( temp.compare("symbols:") == 0 || temp.compare("States:") ==0 )
			{
				iss >> noOfSymbols;
				continue;
			}
		}
		k++;
	}
	model.set(noOfStates,noOfSymbols);
	double recurTrans,nextTrans;
	k=0;
	while( k < noOfStates ){
		getline(input, line, '\n');
		if( line == ""){
			//cout << "BLANK LINE!" << endl;
			continue;
		}
		//cout<< "This line : " << line << endl;
		
		istringstream iss(line);
		iss>>recurTrans;
		model.transMat(k,k) = recurTrans;
		for( int j = 0 ; j < noOfSymbols ; j++ )
			iss >> model.emissionMat[j](0,k);
		getline( input, line, '\n');
		//cout<< "This line : " << line << endl;
		if(k<noOfStates-1){
			istringstream iss2(line);
			iss2 >> nextTrans;
			model.transMat(k,k+1)=nextTrans;
		}
		k++;
	}
	for (int i=0;i<noOfSymbols;i++){
	}
	for(k=0;k<noOfStates;k++){
		model.piMat(0,k)=(double)1/noOfStates;
	}
}

void readSeqFile(string seqFile,vector<Sequence>& sequences){
	cout<<"Sequence File : "<<seqFile<<endl;
	ifstream input;
	input.open( seqFile.c_str() );
	size_t element;
	string line;
	int i = 0;
	
	//Reading test.seq
	while( getline(input, line, '\n') ){
		Sequence sequence;
		istringstream iss(line);
		while( iss >> element ){
			sequence.insert(element);
		}
		sequences.push_back(sequence);
		i++;
	}
	input.close();
}
