#include "matrix4x4.h"

#include <cmath>

Matrix4x4::Matrix4x4()
{
    setIdentity();
}

void Matrix4x4::setIdentity()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            matrix[i][j] = (i == j);
        }
    }
}

void Matrix4x4::setRotMatrix(double angle)
{
    setIdentity();
    setField(0, 0, cos(angle));
    setField(0, 1, -sin(angle));
    setField(1, 0, sin(angle));
    setField(1, 1, cos(angle));
}

void Matrix4x4::setField(int x, int y, double val)
{
    if (x >= 0 && x < 4 && y >= 0 && y < 4)
    {
        matrix[x][y] = val;
    }
}

void Matrix4x4::flipRowCol()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = i; j < 4; j++)
        {
                double temp = matrix[i][j];
                matrix[i][j] = matrix[j][i];
                matrix[j][i] = temp;
        }
    }
}

Vector Matrix4x4::multiply(Vector v)
{
    Vector res;

    res.x = matrix[0][0] * v.x + matrix[0][1] * v.y + matrix[0][2] * v.z;
    res.y = matrix[1][0] * v.x + matrix[1][1] * v.y + matrix[1][2] * v.z;
    res.z = matrix[2][0] * v.x + matrix[2][1] * v.y + matrix[2][2] * v.z;

    return res;

}

//debug
void Matrix4x4::printMatrix()
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%.2lf\t", matrix[i][j]);
        }
        printf("\n");
    }
}
