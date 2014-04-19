#ifndef MATRIX_HH_
#define MATRIX_HH_

#include <iostream>
#include<cstdlib>
#include<math.h>


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
	
	 
	~Matrix(){
        delete[] matrix;
     }
};

void Matrix::mult(const Matrix& second, Matrix& res){
	const size_t depth = second.rsize;
	Matrix& first = *this;
	for(size_t rindex=0;rindex<first.rsize;rindex++){
		for(size_t cindex=0;cindex<second.csize;cindex++){
			res(rindex,cindex) = first(rindex,0)*second(0,cindex);
			for(size_t kindex=1;kindex<depth;kindex++){
				res(rindex,cindex) += first(rindex,kindex)*second(kindex,cindex);
			}
		}
	}
}
void Matrix::maxmult(const Matrix& second, Matrix& res){
	const size_t depth = second.rsize;
	Matrix& first = *this;
	for(size_t rindex=0;rindex<first.rsize;rindex++){
		for(size_t cindex=0;cindex<second.csize;cindex++){
			res(rindex,cindex) = first(rindex,0)*second(0,cindex);
			for(size_t kindex=1;kindex<depth;kindex++){
				res(rindex,cindex) = fmax(res(rindex,cindex),first(rindex,kindex)*second(kindex,cindex));
			}
			
		}
	}
}
double Matrix::scale(){
	double sum = 0.0;
	for(size_t i = 0; i < rsize*csize; ++i)
		sum += matrix[i];
	for(unsigned i = 0; i < rsize*csize; ++i)
		matrix[i] /= sum;
	return sum;
}

std::ostream &operator<<(std::ostream &out, const Matrix &mat){
    for(size_t i = 0; i < mat.getRSize(); ++i){
        for(size_t j = 0; j < mat.getCSize(); ++j)
            out << mat(i, j) << " ";
        out << std::endl;
    }
    return out;
}
#endif
