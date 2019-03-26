#ifndef OBJECT_H_
#define OBJECT_H_

#include "material.h"

// not really needed here, but deriving classes may need them
#include "hit.h"
#include "ray.h"
#include "triple.h"

#include <memory>
#include <string>

class Object;
typedef std::shared_ptr<Object> ObjectPtr;

class Object
{
    public:
        Material material;
        std::string name = "Object";
        int objIndex = -1;

        Point dimRotation;
        double angle = 0;

        ~Object()
        {
            delete material.tex;
        }

        void setName(std::string newName) {
            name = newName;
        }

        void setIndex(int idx) {
            objIndex = idx;
        }

        void setRotationAxis(Vector axis) {
            dimRotation = axis;
        }

        void setRotAngle(double a) {
            angle = a;
        }



        virtual Hit intersect(Ray const &ray) = 0;  // must be implemented
                                                    // in derived class
        virtual Point uvCoord(Point const &hit) = 0;
};

#endif
