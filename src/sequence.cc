#include <iostream>
#include "sequence.hh"
void Sequence::insert(size_t value){
	sequence.push_back(value);
}

void Sequence:: setState(size_t value,size_t index){
	states[index]=value;
}
	

std::ostream &operator<<(std::ostream &out,const Sequence &seq){
    for(size_t i = 0; i < seq.length(); ++i){
        out << seq(i)<< " ";
    }
    out << std::endl;
    return out;
}