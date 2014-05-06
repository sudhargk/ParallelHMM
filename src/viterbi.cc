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

struct BThreadArgs{
	Sequence* sequence;
	HMM* hmm;
	JobQ* jobq;
	int lastProcessedHead;
	BThreadArgs(HMM* hmm,JobQ* jobq,Sequence* sequence,int lastProcessedHead) 	:sequence(sequence),hmm(hmm),jobq(jobq),lastProcessedHead(lastProcessedHead){}
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
			jobq->results[bindex].allocate(1,hmm->noOfStates);
			jobq->results[bindex]=res;
		}else{
			break;
		}
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

void backTrackM(int begin,int end,JobQ& jobq,int lastHeadProcessed,int currbindex,Sequence sequence,HMM* hmm){
	Matrix argCost(hmm->noOfStates,end-begin+1);
	Matrix temp(hmm->noOfStates,1);Matrix res(hmm->noOfStates,1);Matrx argres(hmm->noOfStates,1);
	int idx = lastHeadProcessed,sidx; 
	res = jobq.results[idx];
	for(int idx=lastHeadProcessed;idx<currbindex;idx++){
		jobq.results[idx].maxmult(res,temp);
		res=temp;
	}
	for(sidx = begin;sidx<=end;sidx++,idx++){
		(hmm->transientC)[sequence(sidx)].argmaxmult(res,temp,argres);
		res=temp;
		argCost.assignVector(argres,idx);
	}
	if(currbindex<jobq.numBlocks-1){
		Matrix costUpTable(jobq.numBlocks-currbindex);
		for(int idx = currbindex;idx<jobq.numBlocks;idx++){
			jobq.results[idx].argmaxmult(res,temp,argres);
			res=temp;
			costUpTable.assignVector(argres,idx);
		}
		res = temp;
		
		jobq.results[currbindex+1].argmaxmult(res,temp,argres);
	}else{
		
	}
	int maxState= -1;
	for(int state=0;state<hmm->noOfStates;state++){
		if(maxState<argres(state,1)){
			maxState = argres(state,1);
		}
	}
	idx=0;
	
	
	for(idx=idx-1,sidx=sidx-1;idx>=0;sidx--,idx--){
		sequence.setState(maxState+1,sidx);
		maxState = argCost(maxState,idx);
	}
}

void * backM(void *arg){
	BThreadArgs* threadargs= (ThreadArgs*)(arg);
	HMM* hmm = threadargs->hmm;
	JobQ* jobq = threadargs->jobq;
	Matrix temp(hmm->noOfStates,hmm->noOfStates);
	Matrix res (hmm->noOfStates,hmm->noOfStates);
	while(!jobq->isEmpty()){
		int bindex= jobq->remove(false);
		if(!jobq->isEmptyIndex(bindex)){
			int begin = jobq->getBlockEnd(bindex),end= jobq->getBlockStart(bindex);
			backTrackM(begin,end,jobq,threadargs->lastProcessedHead,begin,threadargs->sequence,hmm);
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
	Matrix argres(this->noOfStates,1);
	Matrix costTable(this->noOfStates,1);
	Matrix costIDXTable(this->noOfStates,1);
	
	int idx = 0; 
	res = jobq.results[idx-1];
	double tempVal;
	for(;idx<jobq.getHeadIdx();idx++){
		costTable.assignVector(jobq.results[idx],idx);
	}
	int lastHeadProcessed = jobq.getHeadIdx();
	for(;idx<numBlocks;idx++){
		jobq.results[idx].argmaxmult(res,temp,argres);
		costTable.assignVector(res,idx);
		costIDXTable.assignVector(argres,idx);
		res=temp;
	}
} 
