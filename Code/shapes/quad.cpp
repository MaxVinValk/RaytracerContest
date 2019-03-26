#include "quad.h"

#include <cmath>

using std::numeric_limits;

Hit Quad::intersect(Ray const &ray)
{
    Hit tri = triIntersect(ray, 0);

    if (tri.t < numeric_limits<double>::infinity())
    {
        return tri;
    }

    tri = triIntersect(ray, 1);

    if (tri.t < numeric_limits<double>::infinity())
    {
        return tri;
    }

    return Hit::NO_HIT();

}

Point Quad::uvCoord(Point const &hit) {
    return Point(0, 0, 0);
}

Hit Quad::triIntersect(Ray const &ray, int numTri)
{
    //choose which verts to use
    //this looks a bit messy, but this implementation allows us to store 4 points,
    //instead of storing 2 triangles which would require 6.

    Vector vec1 = vert1;
    Vector vec2 = vert3;
    Vector vec3 = vert4;

    if (numTri) {
        vec2 = vert2;
        vec3 = vert3;
    }

    double t;
    //first we calculate the normal
    Vector v0 = vec2 - vec1;
    Vector v1 = vec3 - vec1;
    Vector N = (v0).cross(v1).normalized();

    //Now we know the normal, we can see if the ray intersects the triangle plane.

    double div = ray.D.dot(N);
    double loc;

    Point hit;

    if (div < 0) {
        loc = (vec1 - ray.O).dot(N)/div;

        //makes certain the object is not behind the ray origin
        if (loc < 0) {
            return Hit::NO_HIT();
        }

        hit = ray.at(loc);
        t = sqrt(pow(ray.O.x - hit.x, 2) + pow(ray.O.y - hit.y, 2) + pow(ray.O.z - hit.z, 2));

    } else {
        return Hit::NO_HIT();
    }

    //if we reach this point, that's great, because now we know that the ray intersects
    //the plane of the triangle at ray.at(loc).
    //Method as described on:
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution

    Vector edge1 = vec2 - vec1;
    Vector edge2 = vec3 - vec2;
    Vector edge3 = vec1 - vec3;

    Vector coord1 = hit - vec1;
    Vector coord2 = hit - vec2;
    Vector coord3 = hit - vec3;

    if (N.dot(edge1.cross(coord1)) > 0 &&
        N.dot(edge2.cross(coord2)) > 0 &&
        N.dot(edge3.cross(coord3)) > 0)
    {
        return Hit(t, N);
    }

    return Hit::NO_HIT();
}

Quad::Quad(Point const &v1, Point const &v2, Point const &v3, Point const &v4)
:
    vert1(v1),
    vert2(v2),
    vert3(v3),
    vert4(v4)
{}
