#ifndef _TENSOR_HPP_
#define _TENSOR_HPP_

class Tensor {
    /*Class that define a trifocal tensor, that actually is a matrix 3x3x3. 
        T(i,j,k) represents the i, j, k element of the tensor T. 
    */

private:
    int i;
    int j;
    int k;
    double*** T;

public:
    // Builder
    Tensor(int i, int j, int l);

    // Destructor
    ~Tensor();

    // Operators
    double operator()(int i, int j, int k) {
        return T[i][k][k];
    }

};

#endif