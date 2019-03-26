#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../object.h"

class Triangle: public Object
{
    public:
        Triangle(Point const &v1, Point const &v2, Point const &v3);
        Triangle(Point const &v1, Point const &v2, Point const &v3,
            Point const &n1, Point const &n2, Point const &n3);

        Triangle(Point const &v1, Point const &v2, Point const &v3,
                Point const &n1, Point const &n2, Point const &n3,
                Point const &t1, Point const &t2, Point const &t3);

        Triangle(Point const &v1, Point const &v2, Point const &v3, Vector const &normal);

        bool onTriangle(Point loc);
        Point barycentricCoordinates(Point loc);

        virtual Hit intersect(Ray const &ray);
        virtual Point uvCoord(Point const &hit);

        Point const vert1;
        Point const vert2;
        Point const vert3;

        Point normal;

        Point shadingNorm1;
        Point shadingNorm2;
        Point shadingNorm3;

        Point texCoord1;
        Point texCoord2;
        Point texCoord3;

        Point texPoint;


};

#endif
