#ifndef HIT_H_
#define HIT_H_

#include "object.h"

#include <limits>

class Hit
{
    public:
        double t;   // distance of hit
        Vector N;   // Normal at hit
        int tri = -1; //used to find right triangle when hitting a mesh

        Hit(double time, Vector const &normal)
        :
            t(time),
            N(normal)
        {}

        Hit(double time, Vector const &normal, int index)
        :
            t(time),
            N(normal),
            tri(index)
        {}

        static Hit const NO_HIT()
        {
            static Hit no_hit(std::numeric_limits<double>::quiet_NaN(),
                              Vector(std::numeric_limits<double>::quiet_NaN(),
                                     std::numeric_limits<double>::quiet_NaN(),
                                     std::numeric_limits<double>::quiet_NaN()));
            return no_hit;
        }
};

#endif
