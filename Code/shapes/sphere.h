#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"

class Sphere: public Object
{
    public:
        Sphere(Point const &pos, double radius);
        Sphere(Point const &pos, double radius, Point const &dimRot, double rotAngle);

        virtual Hit intersect(Ray const &ray);
        virtual Point uvCoord(Point const &hit);

        Point rotatePoint(Point const &pos);

        Point const position;
        double const r;

};

#endif
