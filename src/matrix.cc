#include "matrix.hh"
#include <iostream>
#include<cstdlib>

std::ostream &operator<<(std::ostream &out, const Matrix &mat){
    for(size_t i = 0; i < mat.getRSize(); ++i){
        for(size_t j = 0; j < mat.getCSize(); ++j)
            out << mat(i, j) << " ";
        out << std::endl;
    }
    return out;
}


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

void Matrix::diagmult(const Matrix& second, Matrix& res){
	Matrix& first = *this;
	for(size_t rindex=0;rindex<first.rsize;rindex++){
		for(size_t cindex=0;cindex<first.csize;cindex++){
			res(rindex,cindex) = first.matrix[cindex*first.rsize+rindex]*second(0, rindex);
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



