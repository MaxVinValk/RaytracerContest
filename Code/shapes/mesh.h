#ifndef MESH_H_
#define MESH_H_

#include "../object.h"
#include "triangle.h"
#include "../vertex.h"

using std::vector;

class Mesh: public Object
{
    public:
        Mesh(vector<Vertex> const &verts, Point const &pos, double const scale);
        Mesh(vector<Vertex> const &verts, Point const &pos, double const scale,
            Point const &dimRot, double rotAngle);


        Point rotatePoint(Point const &pos);

        virtual Hit intersect(Ray const &ray);
        virtual Point uvCoord(Point const &hit);

        vector<Triangle> tri;
    private:
        void loadVertData(vector<Vertex> const &verts, Point const &pos, double const scale);
};

#endif
