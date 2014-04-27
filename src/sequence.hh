#ifndef SEQUENCE_HH_
#define SEQUENCE_HH_

#include <string.h>
#include <iostream>
#include<vector>

using namespace std;

class Sequence{
	vector<size_t> sequence;
	vector<size_t> states;
public :
	size_t &operator()(size_t idx){return sequence[idx];}
	size_t operator()(size_t idx) const {return sequence[idx];}
	void insert(size_t value);
	size_t length() const{return sequence.size();}
};


std::ostream &operator<<(std::ostream &out,const Sequence &seq);
#endif
