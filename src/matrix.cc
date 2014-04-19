#include "matrix.hh"
#include <iostream>

void Matrix::mult(const Matrix& second, Matrix& res){
	const size_t depth = second.rsize;
	Matrix& first = *this;
	for(size_t i=0;i<first.rsize;i++){
		for(size_t j=0;j<second.csize;j++){
			res(i,j) = first(i,0)*second(0,j);
			for(size_t k=1;k<depth;k++){
				res(i,j) += first(i,k)*second(k,j);
			}
		}
	}
}

void Matrix::maxmult(const Matrix& second, Matrix& res){
	const size_t depth = second.rsize;
	Matrix& first = *this;
	for(size_t i=0;i<first.rsize;i++){
		for(size_t j=0;j<second.csize;j++){
			res(i,j) = first(i,0)*second(0,j);
			for(size_t k=1;k<depth;k++){
				res(i,j) = fmax(res(i,j), first(i,k)*second(k,j));
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
