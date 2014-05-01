#include<iostream>
#include<string>
#include<vector>
#include "../src/ioutils.cc"
#include "../src/hmm.hh"
#include "../src/sequence.hh"

int main(int argc, char *argv[]){
	string seqFile;
	string configFile;
	
	if( argc < 3 ){
		seqFile = "test.seq";
		configFile = "test.hmm";
	}
	else{
		seqFile = argv[1];
		configFile = argv[2];
	}
	
	HMM hmm;
	std::cout<<"Configuration File"<<std::endl;
	readConfigFile(configFile,hmm);
	
	vector<Sequence> sequences;
	std::cout<<"Sequence File"<<std::endl;
	readSeqFile(seqFile,sequences);
	
	int idx;
	for(idx=0;idx<sequences.size();idx++){
		std::cout<<sequences[idx];
		std::cout<<hmm.evaluate(sequences[idx]);
	}
}
