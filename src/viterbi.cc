#include "sequence.hh"
#include "hmm.hh"
#include "matrix.hh"
#include "jobq.cc"
#include <stdio.h>
#include <vector>
#include <cmath>
#include <pthread.h>
#define BLOCK_SIZE 5
#define MAX_PROCESSOR 4

struct ThreadArgs{
	Sequence sequence;
	HMM* hmm;
	JobQ* jobq;
	ThreadArgs(HMM* hmm,JobQ* jobq,Sequence sequence):sequence(sequence),hmm(hmm),jobq(jobq){}
};

void * vitV(void *arg){
	ThreadArgs* threadargs = (ThreadArgs*)(arg);
	HMM* hmm = threadargs->hmm;
	JobQ* jobq = threadargs->jobq;
	Matrix temp(hmm->noOfStates,1);
	Matrix res(hmm->noOfStates,1); 
	int bindex=-1;
	while(!jobq->isEmpty()){
		bindex= jobq->remove(true);
		if(!jobq->isEmptyIndex(bindex)){
			int i=jobq->getBlockStart(bindex);
			if (bindex==0) {
				i = 1;
				hmm->piMat.diagmult((hmm->emissionMat)[threadargs->sequence(0)],res);
			}
			for(;i<jobq->getBlockEnd(bindex);i++){
				(hmm->transientC)[threadargs->sequence(i)].maxmult(res,temp);
				res=temp;
			}
		}else{
			break;
		}
	}
	bindex = jobq->getHeadIdx()-1;
	if(bindex>=0){
		jobq->results[bindex].allocate(1,hmm->noOfStates);
		jobq->results[bindex]=res;
	}
}

void * vitM(void *arg){
	ThreadArgs* threadargs= (ThreadArgs*)(arg);
	HMM* hmm = threadargs->hmm;
	JobQ* jobq = threadargs->jobq;
	Matrix temp(hmm->noOfStates,hmm->noOfStates);
	Matrix res (hmm->noOfStates,hmm->noOfStates);
	while(!jobq->isEmpty()){
	int bindex= jobq->remove(false);
		if(!jobq->isEmptyIndex(bindex)){
			int i = jobq->getBlockStart(bindex);
			double logLikeliHood = 0.0;
			res = (hmm->transientC)[threadargs->sequence(i)];
			for(i=i+1;i<jobq->getBlockEnd(bindex);i++){
				(hmm->transientC)[threadargs->sequence(i)].maxmult(res,temp);
				res=temp;
			}
			jobq->results[bindex].allocate(hmm->noOfStates,hmm->noOfStates);
			jobq->results[bindex]=res;
		}else{
			break;
		}
	}
}

void HMM::viterbiAlign (Sequence sequence){
// 	size_t numP = std::thread::hardware_concurrency();
    size_t numP = MAX_PROCESSOR;
	size_t seq_len = sequence.length();
	size_t numBlocks = ceil(sqrt(seq_len));
	cout<<"Number of blocks :: "<<numBlocks<<endl;
	JobQ jobq(seq_len,numBlocks);
	ThreadArgs args(this,&jobq,sequence);
	pthread_t vthreadId;
	pthread_t mthreadId[numP-1];
    pthread_create(&vthreadId,NULL,vitV,&args);
    for(int i=0;i<numP-1;i++){
        pthread_create(&mthreadId[i],NULL,vitM,&args);
    }
    for(int i=0;i<numP-1;i++){
        pthread_join(mthreadId[i],NULL);
    }
    pthread_join(vthreadId,NULL);
	
	Matrix temp(this->noOfStates,1);
	Matrix res(this->noOfStates,1);
	int idx = jobq.getHeadIdx(); 
	res = jobq.results[idx-1];
	double tempVal;
	for(;idx<numBlocks;idx++){
		jobq.results[idx].maxmult(res,temp);
		res=temp;
	}
} 
