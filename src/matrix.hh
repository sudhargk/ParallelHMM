#ifndef MATRIX_HH_
#define MATRIX_HH_
#include <string.h>
#include <iostream>
#include<cstdlib>
#include<math.h>


class Matrix{
#define idx(r,c) ((r)*csize + (c))
	double* matrix;
	size_t rsize,csize;
	
public :
	Matrix(): matrix(NULL), rsize(0), csize(0) { }
	Matrix(const Matrix& add){
		this->allocate(add.rsize,add.csize);
		for(int i=0;i<rsize*csize;i++){
			this->matrix[i]=add.matrix[i];
		}
	}
	
	Matrix(size_t rsize,size_t csize)
		:matrix(new double[rsize*csize]),rsize(rsize),csize(csize){};

	void allocate(size_t rsize,size_t csize){
		this->rsize = rsize;
		this->csize = csize;
		this->matrix = new double[rsize*csize];
	}
	size_t getRSize() const {return rsize;};
	size_t getCSize() const {return csize;};
	void mult(const Matrix& second,Matrix& res);
	void diagmult(const Matrix& second,Matrix& res);
	void diagmaxmult(const Matrix& second,Matrix& res);
	void maxmult(const Matrix& second,Matrix& res);
	void argmaxmult(const Matrix& second,Matrix& res,Matrix& argres);
	void assignVector(const Matrix&vector, int cindex);
	double scale();
	Matrix &operator=(const Matrix &a){
		for(int i=0;i<rsize*csize;i++){
			this->matrix[i]=a.matrix[i];
		}
		return *this;
	}
	double &operator()(size_t r,size_t c){return matrix[idx(r,c)];}
	double operator()(size_t r,size_t c) const {return matrix[idx(r,c)];}
	
	 
	~Matrix(){
        delete[] matrix;
     }
};


std::ostream &operator<<(std::ostream &out, const Matrix &mat);
#endif
