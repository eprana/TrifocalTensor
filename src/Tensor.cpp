#include "Tensor.hpp"

Tensor::Tensor(int i, int j, int k) {

    // Initialisation of the variables 
    this->i = i;
    this->j = j;
    this->k = k;

    // Creation of the matrix (maybe we can use the Eigen librabry ... )
    T = new double**[i];
    for(int l = 0; l < i; ++l) {
        T[l] = new double*[j];
        for(int m = 0; m < j; ++m) {
            T[l][m] = new double[k];
        }
    }
}

Tensor::~Tensor() {
    for(int l = 0; l < this->i; ++l) {
        for(int m = 0; m < this->j; ++m) {
            delete [] T[l][m];
        }
        delete [] T[l];
    }
    delete [] T;
}
