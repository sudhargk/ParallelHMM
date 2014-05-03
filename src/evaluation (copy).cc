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
	JobQ jobq;
	ThreadArgs(Sequence sequence,HMM* hmm,JobQ jobq):sequence(sequence),hmm(hmm),jobq(jobq){}
};

void * evalV(void *arg){
	ThreadArgs* threadargs = (ThreadArgs*)(arg);
	HMM* hmm = threadargs->hmm;
	JobQ* jobq = &threadargs->jobq;
	Matrix *temp = new Matrix(1,hmm->noOfStates);
	Matrix *res=new Matrix(1,hmm->noOfStates); 
	size_t bindex=-1;
	bool first= true;
	double logLikeliHood = 0.0;
	while(!jobq->isEmpty()){
		bindex= jobq->remove(true);
		printf("VINDEX (%d)\n",(int)bindex);
		if(!jobq->isEmptyIndex(bindex)){
			if(first){
				*res= hmm->piMat;
				first=false;
			}
			int i=jobq->getBlockStart(bindex);
			for(;i<jobq->getBlockEnd(bindex);i++){
				res->mult((hmm->transientC)[threadargs->sequence(i)],*temp);
				*res=*temp;
				logLikeliHood+=log(res->scale());
			}
		}else{
			break;
		}
	}
	if(bindex>0){
		jobq->results[bindex].allocate(1,hmm->noOfStates);
		jobq->results[bindex]=*res;
		jobq->likelihood[bindex]=logLikeliHood;
		//cout<<jobq->results[bindex];
	}
	delete temp;delete res;
}

void * evalM(void *arg){
	ThreadArgs* threadargs= (ThreadArgs*)(arg);
	HMM* hmm = threadargs->hmm;
	JobQ* jobq = &threadargs->jobq;
	Matrix* temp = new Matrix(hmm->noOfStates,hmm->noOfStates);
	Matrix* res = new Matrix(hmm->noOfStates,hmm->noOfStates);
	while(!jobq->isEmpty()){
		size_t bindex= jobq->remove(false);
		printf("MINDEX (%d)\n",(int)bindex);
		if(!jobq->isEmptyIndex(bindex)){
			int i=jobq->getBlockStart(bindex);
			double logLikeliHood = 0.0;
			*res= (hmm->transientC)[threadargs->sequence(i)];
			for(i=i+1;i<jobq->getBlockEnd(bindex);i++){
				res->mult((hmm->transientC)[threadargs->sequence(i)],*temp);
				*res=*temp;
				logLikeliHood+=log(res->scale());
			}
			jobq->results[bindex].allocate(hmm->noOfStates,hmm->noOfStates);
			jobq->results[bindex]=*res;
			jobq->likelihood[bindex]=logLikeliHood;
		}else{
			break;
		}
	}
	delete temp;delete res;
}

double HMM::evaluate (const Sequence sequence){
// 	size_t numP = std::thread::hardware_concurrency();
	size_t numP = MAX_PROCESSOR;
	size_t seq_len = sequence.length();
	size_t numBlocks = ceil(sqrt(seq_len));
	cout<<"Number of blocks :: "<<numBlocks<<endl;
	JobQ jobq(seq_len,numBlocks);
	ThreadArgs args(sequence,this,jobq);
	pthread_t vthreadId;
	pthread_t mthreadId[numP-1];
	std::cout<<"Evaluation....";
   	pthread_create(&vthreadId,NULL,evalV,&args);
	for(int i=0;i<numP-1;i++){
		pthread_create(&mthreadId[i],NULL,evalM,&args);
	}
	for(int i=0;i<numP-1;i++){
		pthread_join(mthreadId[i],NULL);
	}
   	pthread_join(vthreadId,NULL);
	
	Matrix *temp = new Matrix(1,this->noOfStates);
	Matrix *res = new Matrix(1,this->noOfStates);
	
	if(jobq.getHeadIdx()==0){
		*res = piMat;
	}else{
		*res = jobq.results[jobq.getHeadIdx()];
	}
	double likelihood = jobq.likelihood[jobq.getHeadIdx()];
	for(int idx=jobq.getHeadIdx()+1;idx<numBlocks;idx++){
		res->mult(jobq.results[idx],*temp);
		likelihood+=log(res->scale())+jobq.likelihood[idx];
		*res=*temp;
	}
	delete res;delete temp;
	return likelihood;
}



 