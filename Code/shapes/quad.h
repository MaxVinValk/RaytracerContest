#ifndef QUAD_H_
#define QUAD_H_

#include "../object.h"

class Quad: public Object
{
    public:
        Quad(Point const &v1, Point const &v2, Point const &v3, Point const &v4);

        virtual Hit intersect(Ray const &ray);
        virtual Point uvCoord(Point const &hit);

        Point const vert1;
        Point const vert2;
        Point const vert3;
        Point const vert4;

    private:
        Hit triIntersect(Ray const &ray, int numTri);
};

#endif
