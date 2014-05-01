#include "sequence.hh"
#include "hmm.hh"
#include <vector>
#include <thread>
#include <pthread.h>
#include <cmath>
#define BLOCK_SIZE 5
#define MAX_PROCESSOR 4

struct Block{
	int start;
	int end;
	Block():start(-1),seen(false){}
	Matrix result;
	bool setseen(){
		seen = true;
	}
	bool is_null(){
		if(start==-1){
			return true;
		}
		return false;
	}
private:
	bool seen;
};

class BlockQ{
	vector <Block> blocks;
	pthread_mutex_t qlock;
	size_t rear;
public:
	BlockQ(size_t seq_len,int base_size){
		int i=base_size;
		Block mBlock;
		for(;i<seq_len;i++){
			mBlock.start = i;
			mBlock.end = i+BLOCK_SIZE;
			blocks.push_back(mBlock);
		}
		blocks[blocks.size()-1].end = seq_len;
		rear = blocks.size();
	}
	Block pop(){
		Block ret;
		pthread_mutex_lock(&qlock);
		if(!isEmpty()){
			rear--;
			ret = blocks[rear];
		}
		pthread_mutex_unlock(&qlock);
	}
	bool isEmpty(){
		if(rear<0){
			return true;
		}
		return false;
	}
};

struct VThreadArgs{
	Sequence sequence;
	Block block;
	VThreadArgs(Sequence sequence,Block block)
		:sequence(sequence),block(block){}
};

struct MThreadArgs{
	Sequence sequence;
	BlockQ blockq;
	MThreadArgs(Sequence sequence,BlockQ blockq)
		:sequence(sequence),blockq(blockq){}
};


double HMM::evaluate (const Sequence sequence){
// 	size_t numP = std::thread::hardware_concurrency();
	size_t numP = MAX_PROCESSOR;
	size_t block_size = ceil(noOfStates*sequence.length()/(noOfStates+numP-1));
	
	Block vblock;
	vblock.start=0;
	vblock.end=block_size;
	VThreadArgs vthreadArg(sequence,vblock);
	
	
	BlockQ blocks(sequence.length(),block_size);
	MThreadArgs mthreadArg(sequence,blocks);
	
	pthread_t vthreadId,mthreadId[numP-1];
	pthread_create(&vthreadId,NULL,evalM,&vblock);
	
	for(int i=0;i<numP-1;i++){
		pthread_create(&mthreadId[i],NULL,evalV,&blocks);
	}
}

void* HMM::evalV(void *arg){
	VThreadArgs threadargs = *(VThreadArgs*)(arg);
	Matrix *temp = new Matrix(1,noOfStates),*res=new Matrix(1,noOfStates); 
	res= &transientC[threadargs.sequence[threadargs.block.start]];
	for(int i=threadargs.block.start;i<threadargs.block.end;i++){
		res->mult(&transientC[threadargs.sequence[i]],temp);
		*res=*temp;
	}
	delete temp;
	delete res;
}

void* HMM:: evalM(void *arg){
	MThreadArgs threadargs= *(MThreadArgs*)(arg);
	Matrix *temp = new Matrix(noOfStates,noOfStates),*res=new Matrix(noOfStates,noOfStates);
	while(threadargs.blockq.isEmpty()){
		Block block = threadargs.blockq.pop();
		if(!block.is_null()){
			res= &transientC[threadargs.sequence[block.start]];
			for(int i=block.start;i<block.end;i++){
				res->mult(&transientC[threadargs.sequence[i]],temp);
				*res=*temp;
			}
		}
	}
	delete temp;delete res;
}

