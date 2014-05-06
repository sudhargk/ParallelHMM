#include<iostream>
#include<string>
#include<vector>
#include<sys/time.h>
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
	//std::cout<<"Configuration File"<<std::endl;
	readConfigFile(configFile,hmm);
	
	vector<Sequence> sequences;
	//std::cout<<"Sequence File"<<std::endl;
	readSeqFile(seqFile,sequences);
	
	struct timeval start_time, end_time;
	gettimeofday(&start_time,NULL);
	long t1 = start_time.tv_sec*1000000 + start_time.tv_usec;
	hmm.precomputeTransientC();
	
	/*for(int i=0;i<hmm.noOfSymbols;i++){
		cout<<"Traisient for symbool :: "<<i<<endl;
		cout<<hmm.transientC[i];
	}*/
	
	int idx;
	for(idx=0;idx<sequences.size();idx++){
		//std::cout<<"Length :: "<<sequences[idx].length()<<endl;
		//std::cout<<sequences[idx]<<endl;
// 		std::cout<<"Likelihood :: "<<endl;
		hmm.evaluate(sequences[idx]);//<<endl;
	}
	
	gettimeofday(&end_time, NULL);
	long t2 = end_time.tv_sec*1000000 + end_time.tv_usec;
	cout << "Time taken : " << t2-t1 << " microseconds" << endl;
}
