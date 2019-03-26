#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "../object.h"

class Example: public Object
{
    public:
        Example(/* YOUR DATA MEMBERS HERE*/);

        virtual Hit intersect(Ray const &ray);
        virtual Point uvCoord(Point const &hit);
        /* YOUR DATA MEMBERS HERE*/
};

#endif
