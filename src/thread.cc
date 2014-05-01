#include<pthread.h>
#include "matrix.hh"

struct Block{
	int start;
	int end;
	Matrix result;
	bool setseen(){
		if (pthread_mutex_trylock()){
			seen = true;
			pthread_mutex_lock();
			return true;
		}
		return false;
	}
private:
	pthread_mutex_t lock;
	bool seen = false;
};


class Thread{
	void*  argument;
	void*
	pthread_t threadId;
	pthread_attr_t attr;
public :
	Thread(){
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	}
	bool run(){
		pthread_create()
		
	}
	~Thread(){
		 pthread_attr_destroy(&attr);
	}
};

