#include "sphere.h"
#include "../matrix4x4.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    *
    * Insert calculation of ray/sphere intersection here.
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/

    //own code
    double a = ray.D.dot(ray.D);
    double b = 2 * ray.D.dot(ray.O - position);
    double c = (ray.O - position).dot(ray.O - position) - r*r;

    double d = b*b - 4*a*c;

    double t;
    Point hit;

    if (d < 0) {
      return Hit::NO_HIT();
    } else {

        double sol1 = (-b + sqrt(d))/(2*a);
        double sol2 = (-b - sqrt(d))/(2*a);

      //since we want closest intersection to ray origin, and we know that sol1 and sol2 specify a t for
      //which the ray collides with the sphere, we want the closest point (so smallest t)

        if (sol1 <= sol2 && sol1 > 0.0001) {
            hit = ray.at(sol1);
        } else if (sol2 > 0.0001) {
            hit = ray.at(sol2);
        } else {
            return Hit::NO_HIT();
        }

      t = sqrt(pow(ray.O.x - hit.x, 2) + pow(ray.O.y - hit.y, 2) + pow(ray.O.z - hit.z, 2));
    }

    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    *
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/

    //Given two points we can calculate the vector between them by subtracting the location
    //the vector is going to from where we want to originate it from. Then we should normalize it,
    //but since the object is a sphere we know that the length of this ray is r, so we divide by it
    //to normalize
    Vector N = ((hit - position)/r);

    return Hit(t,N);
}

Point Sphere::uvCoord(Point const &hit) {

    //get point on sphere coordinates
    Point relHit = (position - hit);

    if (angle != 0)
    {
        relHit = rotatePoint(relHit);
    }

    return Point(
        0.5 + (atan2(relHit.y, relHit.x)/(2*M_PI)),
        1 - acos(relHit.z/r)/M_PI,
        0
    );

}

Point Sphere::rotatePoint(Point const &pos)
{
    Vector rotated = pos;
    //rotation code as by: http://paulbourke.net/geometry/rotate/
    //normalize vector of rotation axis
    Vector u = dimRotation.normalized();
    double d = (sqrt(u.y*u.y + u.z*u.z));

    //xz projection
    Matrix4x4 xz;
    xz.setField(1, 1, u.z/d);
    xz.setField(1, 2, -u.y/d);
    xz.setField(2, 1, u.y/d);
    xz.setField(2, 2, u.z/d);

    //z projection
    Matrix4x4 z;
    z.setField(0, 0, d);
    z.setField(0, 2, -u.x);
    z.setField(2, 0, u.x);
    z.setField(2, 2, d);

    rotated = xz.multiply(rotated);
    rotated = z.multiply(rotated);


    //rotation
    Matrix4x4 rot;
    rot.setRotMatrix((-angle/360)*2*M_PI);
    rotated = rot.multiply(rotated);


    //inverse z projection
    z.flipRowCol();
    xz.flipRowCol();

    rotated = z.multiply(rotated);
    rotated = xz.multiply(rotated);

    return rotated;
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius)
{}

Sphere::Sphere(Point const &pos, double radius, Point const &dimRot, double rotAngle)
:
    position(pos),
    r(radius)
{
    setRotationAxis(dimRot);
    setRotAngle(rotAngle);
}
