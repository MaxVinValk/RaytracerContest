#ifndef MATRIX4x4_H_
#define MATRIX4x4_H_

#include "triple.h"

class Matrix4x4
{
    public:
        Matrix4x4();

        void setIdentity();

        void setRotMatrix(double angle);

        void setField(int x, int y, double val);
        void flipRowCol();

        Vector multiply(Vector v);

        //debug
        void printMatrix();

    private:

        double matrix[4][4];
};

#endif
