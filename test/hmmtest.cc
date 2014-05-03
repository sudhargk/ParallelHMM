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
		seqFile = "test/test.seq";
		configFile = "test/test.hmm";
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
	hmm.precomputeTransientC();
	
	for(int i=0;i<hmm.noOfSymbols;i++){
		cout<<"Traisient for symbool :: "<<i<<endl;
		cout<<hmm.transientC[i];
	}
	int idx;
	for(idx=0;idx<sequences.size();idx++){
		std::cout<<"Length :: "<<sequences[idx].length()<<endl;
		std::cout<<sequences[idx]<<endl;
		std::cout<<"Likelihood :: "<<hmm.evaluate(sequences[idx])<<endl;
	}
}
