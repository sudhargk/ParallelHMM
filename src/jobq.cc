#include "matrix.hh"
#include "hmm.hh"
#include <cmath>
#include <pthread.h>
#define EMPTY -999 
#include <iostream>
class JobQ{
    size_t numBlocks, block_size, seq_len, head, tail;
    pthread_mutex_t mutex_lock;
    public:
    vector<Matrix> results;
    vector<double> likelihood;
    vector<int> state_sequence;

    JobQ(size_t seq_len,size_t nBlocks):
        numBlocks(nBlocks),
        seq_len(seq_len),
        head(0), tail(nBlocks),
        results(nBlocks),
        likelihood(nBlocks,0.0),
        state_sequence(seq_len, -1) {
            pthread_mutex_init(&mutex_lock, 0);
            block_size = ceil(seq_len/nBlocks);
    }

    void resetHead(size_t head=0) {
        this->head=head;
    }
    void resetTail(int tail=-1) {
        this->tail = (tail<0) ? numBlocks : tail ;
    }

    int remove(bool start){
        size_t index=EMPTY;
        pthread_mutex_lock(&mutex_lock);
        if(!isEmpty()){
            if(start){
                index=head++;
            } else { 
                if(tail>1){
                    index=--tail;
                }
            }
            //cout<<"Index :: "<<index<<" Head :: "<<head<<" Tail :: "<<tail<<endl;
        }
        pthread_mutex_unlock(&mutex_lock);
        return index;
    }
    size_t getHeadIdx(){
        return head;
    }
    bool isEmpty(){
        if(head<tail){
            return false;
        }
        return true;
    }

    bool isEmptyIndex(int index){
        return index==EMPTY;
    }

    int getBlockStart(int index){
        return block_size*index;
    }
    int getBlockEnd(int index){
        return min(block_size*(index+1),seq_len);
    }

    ~JobQ(){
        pthread_mutex_destroy(&mutex_lock);
    }
};

template <typename T>
ostream& operator<< (ostream& o, vector<T> v) {
    o << v[0];
    for (size_t i = 1; i < v.size(); ++i) {
        o  << ' ' << v[i];
    }
    return o;
}
