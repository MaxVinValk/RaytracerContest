#include "mesh.h"
#include "../matrix4x4.h"

#include <cmath>

using std::numeric_limits;

Hit Mesh::intersect(Ray const &ray)
{
    Hit nearest = Hit::NO_HIT();

    nearest.t = numeric_limits<double>::infinity();

    for (unsigned int idx = 0; idx < tri.size(); ++idx)
    {
        Hit triHit = tri[idx].intersect(ray);
        if (triHit.t < nearest.t)
        {
            nearest = triHit;
            nearest.tri = idx;
        }
    }

    if (nearest.t < numeric_limits<double>::infinity())
    {
        return nearest;
    }

    return Hit::NO_HIT();
}

Point Mesh::uvCoord(Point const &hit) {
    if (objIndex == -1) {
        return Point(0, 0, 0);
    }

    return tri[objIndex].uvCoord(hit);

}

Point Mesh::rotatePoint(Point const &pos)
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

void Mesh::loadVertData(vector<Vertex> const &verts, Point const &pos, double const scale)
{
    //read all vertex data into triangles with normals
    for (unsigned int idx = 0; idx < verts.size(); idx+=3)
    {
        //We ignore the given normals per vertex, as these do not always sum to
        //the face normal, as these are the normals used for shading. Instead,
        //we calculate the normals based on the given vertices.

        Vector v1(verts[idx].x, verts[idx].y, verts[idx].z);
        Vector v2(verts[idx + 1].x, verts[idx + 1].y, verts[idx + 1].z);
        Vector v3(verts[idx + 2].x, verts[idx + 2].y, verts[idx + 2].z);

        Vector n1(verts[idx].nx, verts[idx].ny, verts[idx].nz);
        Vector n2(verts[idx + 1].nx, verts[idx + 1].ny, verts[idx + 1].nz);
        Vector n3(verts[idx + 2].nx, verts[idx + 2].ny, verts[idx + 2].nz);

        n1.normalize();
        n2.normalize();
        n3.normalize();

        Vector t1(verts[idx].u, verts[idx].v, 0);
        Vector t2(verts[idx + 1].u, verts[idx + 1].v, 0);
        Vector t3(verts[idx + 2].u, verts[idx + 2].v, 0);

        if (angle != 0) {
            v1 = rotatePoint(v1);
            v2 = rotatePoint(v2);
            v3 = rotatePoint(v3);

            n1 = rotatePoint(n1);
            n2 = rotatePoint(n2);
            n3 = rotatePoint(n3);

        }

        tri.push_back(Triangle(v1*scale + pos, v2*scale + pos, v3*scale + pos,
            n1, n2, n3, t1, t2, t3));
    }
}

Mesh::Mesh(vector<Vertex> const &verts, Point const &pos, double const scale)
{
    loadVertData(verts, pos, scale);
}

Mesh::Mesh(vector<Vertex> const &verts, Point const &pos, double const scale,
    Point const &dimRot, double rotAngle)
{
    setRotationAxis(dimRot);
    setRotAngle(rotAngle);
    loadVertData(verts, pos, scale);
}
