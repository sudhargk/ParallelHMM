#include "matrix.hh"
#include <iostream>
using namespace std;
int main(){
	Matrix m(2,2);
	m(0,0)=1;
	m(0,1)=2;
	m(1,1)=1;
	m(1,0)=2;
	cout<<"\nMatrix One"<<std::endl;
	cout<<m;
	Matrix n(2,2);
	n(0,0)=1;
	n(0,1)=2;
	n(1,1)=1;
	n(1,0)=2;
	cout<<"\nMatrix Two"<<std::endl;
	cout<<n;
	Matrix res(2,2);
	m.mult(n,res);
	cout<<"\nResult Matrix Mult"<<std::endl;
	std::cout<<res;

	m.maxmult(n,res);
	cout<<"\nResult Matrix Max Mult"<<std::endl;
	std::cout<<res;
	
	res.scale();
	cout<<"\nResult Matrix scale"<<std::endl;
	std::cout<<res;
	return 0;
}
