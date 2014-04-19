#ifndef MATRIX_HH_
#define MATRIX_HH_

#include <iostream>
#include <cmath>


class Matrix{
#define idx(r,c) ((r)*csize + (c))
	double* matrix;
	size_t rsize,csize;
	
public :
	Matrix(size_t rsize,size_t csize)
		:matrix(new double[rsize*csize]),rsize(rsize),csize(csize){};	
	size_t getRSize() const {return rsize;};
	size_t getCSize() const {return csize;};
	void mult(const Matrix& second,Matrix& res);
	void maxmult(const Matrix& second,Matrix& res);
	double scale();
	double &operator()(size_t r,size_t c){return matrix[idx(r,c)];}
	double operator()(size_t r,size_t c) const {return matrix[idx(r,c)];}
    friend std::ostream &operator<<(std::ostream &out, const Matrix &mat);
	
	~Matrix(){
        delete[] matrix;
     }
};

#endif
