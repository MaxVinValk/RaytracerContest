#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"
#include "matrix4x4.h"

#include "shapes/plane.h"

#include <cmath>
#include <limits>
#include <omp.h>

//own inclusion to show progress line
#include <iostream>

using namespace std;

Color Scene::trace(Ray const &ray, unsigned int recDepth)
{

    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != objects.size(); ++idx)
    {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t)
        {
            min_hit = hit;
            obj = objects[idx];
        }
    }

    // No hit? Return background color.
    if (!obj) return backgroundColor;

    Material material = obj->material;          //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector

    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector + Vector    vector sum
    *        Vector - Vector    vector difference
    *        Point - Point      yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double * Color     scales each color component (r,g,b)
    *        Color * Color      dito
    *        pow(a,b)           a to the power of b
    ****************************************************/

    bool front = false;

    if (N.dot(ray.D) <  -0.00001) {
        front = true;
    }


    //resulting color of pixel
    Color color;

    //set material color (based on texture or material)
    Color matCol;
    double specComp;

    obj->setIndex(min_hit.tri);
    Point UV = obj->uvCoord(hit);

    if (material.tex == nullptr) {
        matCol = material.color;
    } else {
        matCol = material.tex->colorAt(UV.x, UV.y);
    }

    if (material.spec == nullptr) {
        specComp = material.ks;
    } else {
        Color colAt = material.spec->colorAt(UV.x, UV.y);
        //get intensity at the point, scale it to range 0-1
        specComp = (colAt.x + colAt.y + colAt.z)/3.0;
    }

    if (material.norm != nullptr) {
        //shift from range 0-1 to -1, 1
        //Vector imgNorm = (2*material.norm->colorAt(UV.x, UV.y))-1;
        //N = rotatePoint(imgNorm, obj->dimRotation, obj->angle);
        //N = N*imgNorm;
    }

    if (front)
    {
        //if we want to get an edge outline around objects:
        if (outline != 0)
        {
            if (fabs(N.dot(V)) < outline && obj->name.compare("Plane") != 0) {

                color.set(0, 0, 0);
                return color;
            }
        }

        //Ambient component
        color += material.ka*matCol;

        for (unsigned idx = 0; idx < lights.size(); ++idx)
        {
            Color lightColor = lights[idx]->color;

            ObjectPtr closestObjPtr = nullptr;
            Hit closestObj(numeric_limits<double>::infinity(), Vector());
            Ray lightRay(lights[idx]->position, (hit - lights[idx]->position).normalized());

            if (shadows)
            {
                for (unsigned idx = 0; idx != objects.size(); ++idx)
                {
                    Hit hit(objects[idx]->intersect(lightRay));
                    if (hit.t < closestObj.t)
                    {
                        closestObj = hit;
                        closestObjPtr = objects[idx];
                    }
                }
            }

            //If we do not want to use shadows, we just compute lighting. If we do,
            //then we check to see if the object which is closest to the light ray in the
            //direction of the hitpoint is the same object, and the second check we do
            //to distinguish between different hits on faces of the same object (this is not a problem
            //for spheres, but might be for meshes or shapes such as a torus.
            //If both the light-ray and the view-ray are at the same point in worldspace we can conclude
            //that they hit the same spot on an object.

            if (!shadows || (closestObjPtr == obj && lightRay.at(closestObj.t) == ray.at(min_hit.t)))
            {
                //diffuse
                Vector L = (lights[idx]->position - hit).normalized();
                color += lightColor * material.kd * matCol * fmax(0.0, N.dot(L));

                //specular, material.n = p in phong lighting terms
                Vector R = ((2 * (N.dot(L))*N - L).normalized());
                color +=  lightColor * specComp * pow(fmax(0.0, V.dot(R)), material.n);
            }
        }

        //reflections
        if (recDepth < maxRecDepth && material.ks != 0) {
            Vector R = ((2 * (N.dot(V))*N - V).normalized());
            Ray reflectRay(hit, R);
            color += trace(reflectRay, recDepth+1) * specComp;
        }
    }
    //transparancy

    if (recDepth < maxRecDepth && material.trans != 1) {

        float ni, nt;

        //entering the object
        if (front) {
            ni = 1;
            nt = material.ri;
        } else {
            ni = material.ri;
            nt = 1;
        }
        Ray transRay(hit, refractRay(ni, nt, ray.D, N));

        Color res = trace(transRay, recDepth+1);

        //if (res.x == 0 && res.y == 0 && res.z == 0)
        color *= material.trans;
        color += (1 - material.trans) * res;
    }

    //Toggling this option allows for some distance visualisation for debugging purposes
    // color = color * ((min_hit.t - 600)/200);

    return color;
}

void Scene::render(Image &img)
{
    unsigned w = img.width();
    unsigned h = img.height();

    unsigned int parsed = 0;
    unsigned int goal = w*h;
    unsigned int progressBarLength = 50;
    unsigned int steps = goal/progressBarLength;

    unsigned int parsedProcess = 0;

    printProgress(0, progressBarLength);

    #pragma omp parallel for
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            Color result;

            for (unsigned int eyePos = 0; eyePos < maxEyePos; ++eyePos)
            {
                Point eyePoint = eye;
                eyePoint.z += depthOffset*(eyePos);

                Color camPosResult;
                for (unsigned int i = 0; i < superSamplingFactor; ++i)
                {
                    for (unsigned int j = 0; j < superSamplingFactor; ++j)
                    {

                        Point pixel(x + i*((float)1 / superSamplingFactor) + 0.5/superSamplingFactor,
                        h - 1 - y + j*((float)1 / superSamplingFactor) + 0.5/superSamplingFactor);

                        Ray ray(eye, (pixel - eyePoint).normalized());
                        Color col = trace(ray);
                        col.clamp();
                        camPosResult += (float)1/(superSamplingFactor*superSamplingFactor) * col;

                    }
                }

                //we have the color for one frame
                result += (1.0/maxEyePos) * camPosResult;
            }

            //clamp in case of rounding mistakes due to floating point numbers
            result.clamp();
            img(x, y) = result;

            #pragma omp atomic update
            ++parsed;

            #pragma omp atomic update
            ++parsedProcess;

            if (parsedProcess >= steps) {
                parsedProcess = 0;
                printProgress((double)parsed/goal, progressBarLength);
            }
        }
    }

    //If thread 0 finishes before the other threads, then the loading bar
    //does not update anymore. We quickly update the progress in this case.
    printProgress((double)parsed/goal, progressBarLength);

    cout << '\n';
}

void Scene::bloom(Image &img, float thres)
{
    unsigned w = img.width();
    unsigned h = img.height();

    //create bright pass
    Image brightPass(w, h);

    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            float lum = img(x, y).x + img(x, y).y + img(x, y).z;

            double pixCol = fmax(0, lum - thres);
            Color pix(pixCol, pixCol, pixCol);

            brightPass(x, y) = pix;
        }
    }

    //blurring
    for (unsigned b = 0; b < 4; ++b)
    {
        Image res(w, h);
        for (unsigned y = 0; y < h; ++y)
        {
            for (unsigned x = 0; x < w; ++x)
            {
                Color sum;

                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        unsigned int locX = x-1+i;
                        unsigned int locY = y-1+j;

                        if (locX >= 0 && locX < w && locY >= 0 && locY < h) {
                            sum += brightPass(locX, locY);
                        }

                    }
                }
                //Absent pixels are treated as black
                res(x, y) = sum / 9;
            }
        }

        brightPass = res;
    }
    //img = brightPass;
    //return;
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            img(x, y) += brightPass(x, y);
            img(x, y).clamp();
        }
    }

}

void Scene::remap(Image &img, Color base, Color ceiling) {

    if (base.x > ceiling.x || base.y > ceiling.y || base.z > ceiling.z) {
        cout << "ERROR in remap: Given values for base exceed that of ceiling" << std::endl;
        exit(1);
    }

    //Original color range we treat as 0-1 per color
    Color percentage((ceiling.x-base.x),
                        (ceiling.y-base.y),
                        (ceiling.z-base.z)
                    );

    unsigned w = img.width();
    unsigned h = img.height();

    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < w; ++x) {
            img(x, y) = base + (img(x, y) * percentage);
        }
    }
}

//Function I added so that we have a progress bar on rendering scenes,
//which is especially useful when the scene contains a lot of tris.

void Scene::printProgress(double progress, unsigned int numDots)
{
    unsigned int upTo = ((progress) * numDots);

    cout << "\33[2K\r Progress: [";

    for (unsigned int idx = 0; idx < upTo; ++idx)
    {
        cout << '=';
    }

    for (unsigned int idx = upTo; idx < numDots; ++idx)
    {
        cout << '-';
    }

    cout << "]\r";
    cout << std::flush;
}

Vector Scene::refractRay(float ni, float nt, Vector in, Vector N)
{
    float dp = in.dot(N);
    Vector result = ((ni/nt) * (in - dp*N)) - N * sqrt(1 - (ni*ni*(1-(dp*dp)))/(nt*nt));
    return result;
}

// --- Misc functions ----------------------------------------------------------

void Scene::addObject(ObjectPtr obj)
{
    objects.push_back(obj);
}

void Scene::addLight(Light const &light)
{
    lights.push_back(LightPtr(new Light(light)));
}

void Scene::setEye(Triple const &position)
{
    eye = position;
}

unsigned Scene::getNumObject()
{
    return objects.size();
}

unsigned Scene::getNumLights()
{
    return lights.size();
}
