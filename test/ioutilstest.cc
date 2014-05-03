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
	
	std::cout<<"Configuration File"<<std::endl;
	HMM hmm;
	readConfigFile(configFile,hmm);
	hmm.print();
	
	vector<Sequence> sequences;
	std::cout<<"Sequence File"<<std::endl;
	readSeqFile(seqFile,sequences);
	for(int  i = 0 ; i < sequences.size() ; i++ ){
		cout << "Length of sequence "<< i << " : "<< sequences[i].length()<<endl;
		cout<<sequences[i];
 	}
}
