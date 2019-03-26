#include "triangle.h"

#include <cmath>
#include <stdio.h>

using namespace std;

Hit Triangle::intersect(Ray const &ray)
{
    double t;

    //first we calculate if the ray intersects the triangle plane
    double div = ray.D.dot(normal);
    double loc;

    Point hit;

    if (div < -0.0001) {
        loc = (vert1 - ray.O).dot(normal)/div;

        //makes certain the object is not behind the ray origin
        if (loc < 0) {
            return Hit::NO_HIT();
        }

        hit = ray.at(loc);
        t = hit.distance(ray.O);
        //t = sqrt(pow(ray.O.x - hit.x, 2) + pow(ray.O.y - hit.y, 2) + pow(ray.O.z - hit.z, 2));

    } else {
        return Hit::NO_HIT();
    }

    //Here we know that the ray is in the triangle plane

    if (onTriangle(hit))
    {
        //here we compute the barycentric coordinates
        Vector sN = barycentricCoordinates(hit);
        texPoint = texCoord1 * sN.x + texCoord2 * sN.y + texCoord3 * sN.z;

        //terrible reason, I don't know why my current calculations flip the uv coord
        //y axis, but it has to be done
        texPoint.y = 1 - texPoint.y;

        sN = shadingNorm1*sN.x + shadingNorm2*sN.y + shadingNorm3*sN.z;

        return Hit(t, sN.normalized());
    }

    return Hit::NO_HIT();
}


//implemented same side of edge triangle checking
//by constructing 3 vectors which point in either clockwise or counter-clockwise
//fashion, iff a point is within the triangle, the dot product of each coordinate
//should be greater than 0.
//Method as described on:
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution

bool Triangle::onTriangle(Point loc) {
    Vector edge1 = vert2 - vert1;
    Vector edge2 = vert3 - vert2;
    Vector edge3 = vert1 - vert3;

    Vector coord1 = loc - vert1;
    Vector coord2 = loc - vert2;
    Vector coord3 = loc - vert3;

    if (normal.dot(edge1.cross(coord1)) > 0 &&
        normal.dot(edge2.cross(coord2)) > 0 &&
        normal.dot(edge3.cross(coord3)) > 0)
    {
        return true;
    }

    return false;
}

//assumes point is on tri
Point Triangle::uvCoord(Point const &hit) {
    return texPoint;
    //Point bc = barycentricCoordinates(hit);
    //return (texCoord1 * bc.x + texCoord2 * bc.y + texCoord3 * bc.z);
}

//returns barycentric coordnate, assumes that loc is on the given triangle
Point Triangle::barycentricCoordinates(Point loc)
{
    Vector na = (vert3-vert2).cross(loc-vert2);
    Vector nb = (vert1-vert3).cross(loc-vert3);
    Vector nc = (vert2-vert1).cross(loc-vert1);

    Vector newNorm = (vert2 - vert1).cross(vert3 - vert1);
    double denom = newNorm.length_2();

    double a = newNorm.dot(na)/denom;
    double b = newNorm.dot(nb)/denom;
    double c = newNorm.dot(nc)/denom;

    return Point(a, b, c);
    //vert1 = a, vert2 = b, vert3 = c

}

Triangle::Triangle(Point const &v1, Point const &v2, Point const &v3)
:
  vert1(v1),
  vert2(v2),
  vert3(v3)
{
    normal = (vert2 - vert1).cross(vert3 - vert1).normalized();
    shadingNorm1 = normal;
    shadingNorm2 = normal;
    shadingNorm3 = normal;
}

Triangle::Triangle(Point const &v1, Point const &v2, Point const &v3,
    Point const &n1, Point const &n2, Point const &n3)
:
    vert1(v1),
    vert2(v2),
    vert3(v3),
    shadingNorm1(n1),
    shadingNorm2(n2),
    shadingNorm3(n3)
{
    normal = (vert2-vert1).cross(vert3-vert1).normalized();
}

Triangle::Triangle(Point const &v1, Point const &v2, Point const &v3,
        Point const &n1, Point const &n2, Point const &n3,
        Point const &t1, Point const &t2, Point const &t3)
:
    vert1(v1),
    vert2(v2),
    vert3(v3),
    shadingNorm1(n1),
    shadingNorm2(n2),
    shadingNorm3(n3),
    texCoord1(t1),
    texCoord2(t2),
    texCoord3(t3)
{
    normal = (vert2-vert1).cross(vert3-vert1).normalized();
}

Triangle::Triangle(Point const &v1, Point const &v2, Point const &v3, Vector const &n)
:
    vert1(v1),
    vert2(v2),
    vert3(v3),
    normal(n)
{}
