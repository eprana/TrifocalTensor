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

    inline int getI() {
        return this->i;
    }

    inline int getJ() {
        return this->j;
    }

    inline int getK() {
        return this->k;
    }

    inline double*** getT() {
        return this->T;
    }

    inline double getT(int i, int j, int k) {
        return this->T[i][j][k];
    }

    inline void setT(int i, int j, int k, double val) {
        this->T[i][j][k] = val;
    }

    // Operators
    double operator()(int i, int j, int k) {
        return T[i][j][k];
    }

};

#endif