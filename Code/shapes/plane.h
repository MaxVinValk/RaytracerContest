#ifndef PLANE_H_
#define PLANE_H_

#include "../object.h"

class Plane: public Object
{
    public:
        Plane(Point const &pos, Vector const& norm);
        virtual Hit intersect(Ray const &ray);
        virtual Point uvCoord(Point const &hit);

        Point  const position;
        Vector const normal;
};

#endif
