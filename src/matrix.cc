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




