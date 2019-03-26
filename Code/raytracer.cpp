#include "raytracer.h"

#include "image.h"
#include "light.h"
#include "material.h"
#include "triple.h"

// =============================================================================
// -- Include all your shapes here ---------------------------------------------
// =============================================================================

#include "shapes/sphere.h"
#include "shapes/triangle.h"
#include "shapes/plane.h"
#include "shapes/quad.h"
#include "shapes/mesh.h"

// =============================================================================
// -- End of shape includes ----------------------------------------------------
// =============================================================================

#include "objloader.h"

#include "json/json.h"

#include <exception>
#include <fstream>
#include <iostream>

using namespace std;        // no std:: required
using json = nlohmann::json;

bool Raytracer::parseObjectNode(json const &node)
{
    ObjectPtr obj = nullptr;

// =============================================================================
// -- Determine type and parse object parametrers ------------------------------
// =============================================================================

    if (node["type"] == "sphere")
    {
        Point pos(node["position"]);
        double radius = node["radius"];

        if (node.count("rotation") == 1 && node.count("angle") == 1) {

            Point dimRotation(node["rotation"]);
            double angle = node["angle"];
            obj = ObjectPtr(new Sphere(pos, radius, dimRotation, angle));

        } else {
            obj = ObjectPtr(new Sphere(pos, radius));
        }

    }
    else if (node["type"] == "triangle")
    {
        Point v1(node["vertices"]["1"]);
        Point v2(node["vertices"]["2"]);
        Point v3(node["vertices"]["3"]);
        obj = ObjectPtr(new Triangle(v1, v2, v3));
    }
    else if (node["type"] == "plane")
    {
        Point pos(node["position"]);
        Vector norm(node["normal"]);
        obj = ObjectPtr(new Plane(pos, norm));
    }
    else if (node["type"] == "quad")
    {
        Point v1(node["vertices"]["1"]);
        Point v2(node["vertices"]["2"]);
        Point v3(node["vertices"]["3"]);
        Point v4(node["vertices"]["4"]);
        obj = ObjectPtr(new Quad(v1, v2, v3, v4));
    }
    else if (node["type"] == "mesh")
    {
        OBJLoader loader(node["object"].get<std::string>());
        //we use the stored position as offset in the scene, and the scale as model scale
        Point pos(node["position"]);
        double scale = node["scale"];

        if (node.count("rotation") == 1 && node.count("angle") == 1) {

            Point dimRotation(node["rotation"]);
            double angle = node["angle"];
            obj = ObjectPtr(new Mesh(loader.vertex_data(), pos, scale, dimRotation, angle));

        } else {
            obj = ObjectPtr(new Mesh(loader.vertex_data(), pos, scale));
        }
    }
    else
    {
        cerr << "Unknown object type: " << node["type"] << ".\n";
    }

// =============================================================================
// -- End of object reading ----------------------------------------------------
// =============================================================================

    if (!obj)
        return false;

    // Parse material and add object to the scene
    obj->material = parseMaterialNode(node["material"]);
    scene.addObject(obj);
    return true;
}

Light Raytracer::parseLightNode(json const &node) const
{
    Point pos(node["position"]);
    Color col(node["color"]);
    return Light(pos, col);
}

Material Raytracer::parseMaterialNode(json const &node) const
{
    Color color;

    if (node.count("color") == 1) {
        Color colorFound(node["color"]);
        color = colorFound;
    }

    double ka = node["ka"];
    double kd = node["kd"];
    double ks = node["ks"];
    double n  = node["n"];
    Material m(color, ka, kd, ks, n);

    if (node.count("texture") == 1) {
        m.tex = new Image(node["texture"].get<std::string>());
    }

    if (node.count("normalmap") == 1) {
        m.norm = new Image(node["normalmap"].get<std::string>());
    }

    if (node.count("specmap") == 1) {
        m.norm = new Image(node["normalmap"].get<std::string>());
    }

    if (node.count("ri") == 1 && node.count("trans") == 1) {
        m.ri = node["ri"];
        m.trans = node["trans"];
    }

    return m;
}

bool Raytracer::readScene(string const &ifname)
try
{
    // Read and parse input json file
    ifstream infile(ifname);
    if (!infile) throw runtime_error("Could not open input file for reading.");
    json jsonscene;
    infile >> jsonscene;

    cout << "Rendering from file:" << ifname << std::endl;

// =============================================================================
// -- Read your scene data in this section -------------------------------------
// =============================================================================

    Point eye(jsonscene["Eye"]);
    scene.setEye(eye);

    // TODO: add your other configuration settings here

    if (jsonscene.count("Shadows") == 1) {
        scene.shadows = jsonscene["Shadows"];
    }

    if (jsonscene.count("MaxRecursionDepth") == 1) {
        scene.maxRecDepth = jsonscene["MaxRecursionDepth"];
    }

    if (jsonscene.count("SuperSamplingFactor") == 1) {
        scene.superSamplingFactor = jsonscene["SuperSamplingFactor"];
    }

    //setting for drawing outlines

    if (jsonscene.count("Outline") == 1) {
        scene.outline = jsonscene["Outline"];
    }

    //background colour

    if (jsonscene.count("BackgroundColor") == 1) {
        Color b(jsonscene["BackgroundColor"]);
        scene.backgroundColor = b;
    }

    //settings for depth of field effects

    if (jsonscene.count("DepthOfFieldPos") == 1) {
        scene.maxEyePos = jsonscene["DepthOfFieldPos"];
    }

    if (jsonscene.count("DepthOfFieldOffset") == 1) {
        scene.depthOffset = jsonscene["DepthOfFieldOffset"];
    }

    //settings for post-processing

    if (jsonscene.count("Bloom") == 1) {
        scene.bloomThres = jsonscene["Bloom"];
    }

    if (jsonscene.count("colorRemapFloor") == 1 && jsonscene.count("colorRemapCeil") == 1) {
        Color f(jsonscene["colorRemapFloor"]);
        Color c(jsonscene["colorRemapCeil"]);
        scene.floorColor = f;
        scene.ceilColor = c;
    }

    for (auto const &lightNode : jsonscene["Lights"])
        scene.addLight(parseLightNode(lightNode));

    unsigned objCount = 0;
    for (auto const &objectNode : jsonscene["Objects"])
        if (parseObjectNode(objectNode))
            ++objCount;

    cout << "Parsed " << objCount << " objects.\n";

// =============================================================================
// -- End of scene data reading ------------------------------------------------
// =============================================================================

    return true;
}
catch (exception const &ex)
{
    cerr << ex.what() << '\n';
    return false;
}

void Raytracer::renderToFile(string const &ofname)
{
    // TODO: the size may be a settings in your file
    Image img(400, 400);
    cout << "Tracing...\n";
    scene.render(img);

    if (scene.bloomThres != scene.NO_BLOOM) {
        scene.bloom(img, scene.bloomThres);
    }

    if (scene.floorColor != scene.ceilColor) {
        scene.remap(img, scene.floorColor, scene.ceilColor);
    }

    cout << "Writing image to " << ofname << "...\n";
    img.write_png(ofname);
    cout << "Done.\n";
}
