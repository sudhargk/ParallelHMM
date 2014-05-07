#include "sequence.hh"
#include "hmm.hh"
#include "matrix.hh"
#include "jobq.cc"
#include <stdio.h>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <cassert>
#define BLOCK_SIZE 5
#define MAX_PROCESSOR 1

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
	int numBlocksHead;
    Matrix *costTable;
    vector<int> state_seq;
	BThreadArgs(HMM* hmm,JobQ* jobq,Sequence* sequence,int numBlocksHead, Matrix *costTable, vector<int> &state_seq):
        sequence(sequence),
        hmm(hmm),
        jobq(jobq),
        numBlocksHead(numBlocksHead),
        costTable(costTable),
        state_seq(state_seq)
    {}
};

pthread_mutex_t print_lock;

void* vitV(void *arg){
	ThreadArgs* threadargs = (ThreadArgs*)(arg);
	HMM* hmm = threadargs->hmm;
	JobQ* jobq = threadargs->jobq;
	Matrix temp(hmm->noOfStates,1);
	Matrix res(hmm->noOfStates,1); 
	int bindex=-1;
	while(!jobq->isEmpty()){
		bindex = jobq->remove(true);
		if(!jobq->isEmptyIndex(bindex)){
			int i = jobq->getBlockStart(bindex);
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
    pthread_exit(NULL);
}

void * vitM(void *arg){
	ThreadArgs* threadargs = (ThreadArgs*)(arg);
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
    pthread_exit(NULL);
}

void backTrackM(JobQ& jobq,
        int currbindex,
        Sequence sequence, HMM* hmm,
        int start_state, int end_state) {
    int begin = jobq.getBlockStart(currbindex);
    int end = jobq.getBlockEnd(currbindex);
    int block_len = end-begin;
    Matrix costTable(hmm->noOfStates, block_len);
	Matrix costIDXTable(hmm->noOfStates, block_len);
	Matrix temp(hmm->noOfStates, 1);
    Matrix res(hmm->noOfStates, 1);
    Matrix argres(hmm->noOfStates, 1);

    res = jobq.results[currbindex-1];
	for(int i=0; i < block_len; ++i){
		(hmm->transientC)[sequence(begin+i)].argmaxmult(res, temp, argres);
		costTable.assignVector(res, i);
		costIDXTable.assignVector(argres, i);
		res = temp;
	}

    int state = end_state;
	for(int i=block_len-1; i >= 0; --i){
        jobq.state_sequence[begin+i] = state;
        state = costIDXTable(state, i);
    }
    assert(state == start_state);

    pthread_mutex_lock(&print_lock);
    cout << "State sequence before: " << jobq.state_sequence << endl;
    cout << "Block: " << currbindex << ", Begin: " << begin << ", End: " << end << endl;
    cout << "State sequence after: " << jobq.state_sequence << endl << endl;
    pthread_mutex_unlock(&print_lock);
}

void* backM(void *arg) {
	BThreadArgs* threadargs = (BThreadArgs*)(arg);
	JobQ* jobq = threadargs->jobq;
    int lastProcessedHead = threadargs->numBlocksHead - 1;
	//Matrix temp(hmm->noOfStates,hmm->noOfStates);
	//Matrix res (hmm->noOfStates,hmm->noOfStates);
	while (!jobq->isEmpty()) {
		int bindex = jobq->remove(false);
        int seq_index = bindex - lastProcessedHead;
		if (!jobq->isEmptyIndex(bindex)) {
			backTrackM(*jobq, bindex,
                   *(threadargs->sequence),
                   threadargs->hmm,
                   threadargs->state_seq[seq_index-1],
                   threadargs->state_seq[seq_index]);
		}
	}		
    pthread_exit(NULL);
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
    pthread_mutex_init(&print_lock, NULL);
    pthread_create(&vthreadId, NULL, vitV, &args);
    for(size_t i=0; i<numP-1; i++) {
        pthread_create(&mthreadId[i], NULL, vitM, &args);
    }
    for(size_t i=0; i<numP-1; i++) {
        pthread_join(mthreadId[i], NULL);
    }
    pthread_join(vthreadId, NULL);
	
    size_t numBlocksHead = jobq.getHeadIdx();
    size_t numBlocksTail = numBlocks - numBlocksHead;
	Matrix temp(this->noOfStates,1);
	Matrix res(this->noOfStates,1);
	Matrix argres(this->noOfStates,1);
	Matrix costTable(this->noOfStates, numBlocksTail);
	Matrix costIDXTable(this->noOfStates, numBlocksTail);
	
    // Compute the max path probs and prev state table (DP) at the block level
	res = jobq.results[numBlocksHead-1];
	for(size_t i=0; i < numBlocksTail; i++) {
		jobq.results[numBlocksHead+i].argmaxmult(res, temp, argres);
		costTable.assignVector(res, i);
		costIDXTable.assignVector(argres, i);
		res = temp;
	}

    // Backtrack over blocks to get the states in optimal state seq at block boundaries
    vector<int> state_seq(numBlocksTail+1);
    int state = costTable.argmaxInCol(numBlocks-1);
    state_seq[numBlocksTail] = state;
    for (int i=numBlocksTail-1; i >= 0; --i) {
        state = costIDXTable(state, i);
        state_seq[i] = state;
    }

    // Compute the optimal state sequence for all blocks except the first sequential ones
    jobq.resetTail();
	BThreadArgs bargs(this, &jobq, &sequence, numBlocksHead, &costTable, state_seq);
    // re-using mthreadid[]
    for(size_t i=0;i<numP-1;i++){
        pthread_create(&mthreadId[i], NULL, backM, &bargs);
    }

    // Recompute all the intermediate vectors (thread 1)
    int noOfSymbols = jobq.getBlockStart(jobq.getHeadIdx());
	temp.clear();
	res.clear(); 
	argres.clear();
	Matrix costTableV(noOfStates, noOfSymbols-1);
	Matrix costIDXTableV(noOfStates, noOfSymbols-1);
    piMat.diagmult(emissionMat[sequence(0)],res);
    for (int i=0; i < noOfSymbols-1; ++i) {
        transientC[sequence(i+1)].argmaxmult(res, temp, argres);
        costTableV.assignVector(res, i);
        costIDXTableV.assignVector(argres, i);
        res = temp;
    }

    // Compute the optimal state sequence for initial blocks
    state = state_seq[0];
    jobq.state_sequence[noOfSymbols-1] = state;
    for (int i = noOfSymbols-2; i >= 0; --i) {
        state = costIDXTable(state, i);
        jobq.state_sequence[i] = state;
    }

    pthread_mutex_lock(&print_lock);
    cout << "State sequence before: " << jobq.state_sequence << endl;
    cout << "Block: " << 0  << ", Begin: " << 0 << ", End: " << noOfSymbols-1 << endl;
    cout << "State sequence after: " << jobq.state_sequence << endl << endl;
    pthread_mutex_unlock(&print_lock);

    for(size_t i=0;i<numP-1;i++){
        pthread_join(mthreadId[i], NULL);
    }
    pthread_mutex_destroy(&print_lock);
} 
