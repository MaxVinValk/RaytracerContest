#include "plane.h"

#include <cmath>

Hit Plane::intersect(Ray const &ray)
{
    double t;

    double div = normal.dot(ray.D);
    double loc;
    Point hit;

    //since incoming ray to the normal is at more than 90 degrees if the surface faces
    //the ray, we check to see that the dot product is lower than a very low value so
    //that we don't have to deal with edge values.
    if (div < -0.00001)
    {
        loc = normal.dot(position - ray.O)/div;

        //makes certain the object is not behind the ray origin
        if (loc < 0) {
            return Hit::NO_HIT();
        }

        hit = ray.at(loc);
        t = sqrt(pow(ray.O.x - hit.x, 2) + pow(ray.O.y - hit.y, 2) + pow(ray.O.z - hit.z, 2));
    } else {
        return Hit::NO_HIT();
    }

    return Hit(t, normal);
}

Point Plane::uvCoord(Point const &hit) {
    return Point(0, 0, 0);
}

Plane::Plane(Point const &pos, Vector const& norm)
:
    position(pos),
    normal(norm)
{
    setName("Plane");
}
