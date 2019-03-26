#ifndef SCENE_H_
#define SCENE_H_

#include "light.h"
#include "object.h"
#include "triple.h"

#include <vector>

// Forward declerations
class Ray;
class Image;

class Scene
{
    std::vector<ObjectPtr> objects;
    std::vector<LightPtr> lights;   // no ptr needed, but kept for consistency
    Point eye;

    public:

        // trace a ray into the scene and return the color
        Color trace(Ray const &ray, unsigned int recDepth = 0);

        // render the scene to the given image
        void render(Image &img);


        void addObject(ObjectPtr obj);
        void addLight(Light const &light);
        void setEye(Triple const &position);

        unsigned getNumObject();
        unsigned getNumLights();

        //additional variables for lighting/shadows/reflections
        bool shadows = false;
        unsigned int maxRecDepth = 0;
        unsigned int superSamplingFactor = 1;
        double outline = 0;

        Color backgroundColor;

        //settings for depth of Field
        unsigned int maxEyePos = 1;
        unsigned int depthOffset = 10;

        //postprocessing
        const float NO_BLOOM = 5.0;

        float bloomThres = NO_BLOOM;

        Color floorColor;
        Color ceilColor;


        void bloom(Image &img, float thres);
        void remap(Image &img, Color base, Color ceiling);

    private:
        Vector refractRay(float ni, float nt, Vector in, Vector N);
        void printProgress(double progress, unsigned int numDots);

};

#endif
