#include "../src/matrix.hh"
#include <iostream>
#include <vector>
using namespace std;
int main(){
    //Matrix m(2,2);
    //m(0,0)=1;
    //m(0,1)=2;
    //m(1,0)=2;
    //m(1,1)=1;
    //cout<<"\nMatrix One"<<std::endl;
    //cout<<m;
    //Matrix n(2,2);
    //n(0,0)=1;
    //n(0,1)=2;
    //n(1,0)=2;
    //n(1,1)=1;
    
	//cout<<"\nMatrix Two"<<std::endl;
	//cout<<n;

    Matrix p(1,2);
    p(0,0)=1.6e-13;
    p(0,1)=2.98323;

	cout<<"\nMatrix Three"<<std::endl;
	cout<<p;

    p.clear();
    cout<<"\nMatrix Three after clear()"<<endl;
    cout << p;
	
	//Matrix res(2,2);
	//m.mult(n,res);
	//cout<<"\nResult Matrix Mult"<<std::endl;
	//std::cout<<res;
	
	//m.maxmult(n,res);
	//cout<<"\nResult Matrix Max Mult"<<std::endl;
	//std::cout<<res;
	
	//res.scale();
	//cout<<"\nResult Matrix scale"<<std::endl;
	//std::cout<<res;
	
	
	//Matrix p(1,2);
	//p(0,0)=1;
	//p(0,1)=2;
	
	//m.diagmult(p,res);
	//cout<<"\nResult Diag Matrix Max Mult"<<std::endl;
	//std::cout<<res;
	//vector<Matrix> k;
	/*Matrix m = k[0];
	m.allocate(2,2);
	m(0,0)=2;
	m(0,1)=2;
	m(1,0)=2;
	m(1,1)=2;
	*/
	//k.resize(4);
	//for (vector<Matrix>::iterator i = k.begin(); i != k.end(); ++i) {
		//cout<<"\nMatrix One"<<std::endl;
		//cout<<*i;
	//}
	
	return 0;
}
