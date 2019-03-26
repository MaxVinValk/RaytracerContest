![header img](/screenshots/scene_mesh_bloom_ss.png)

<h1>Contest Submission</h1>

I have made a couple of expansions based on that which was required, some for the competition,
some over time. I will list that which is implemented here.

Note that few instances of code duplication exist, which is a shame, but I did not have the time to
wrap everything up neatly. My bad!

Running the program is like what you would expect for the raytracer. Run the
CMake file, then cd to build, run make, and run the program using ./ray ../Scenes/
scene_name.json

Note that running the scene scene_mesh_bloom_ss_dov will take a *very* long time.

<h2>Parallel processing</h2>

A quite easy quality-of-life improvement, can be found in the for loops of scene::render()

<h2>Loading bar</h2>

Implemented a little loading bar which indicates the process of rendering. It was implemented prior
to parallel processing, and as such, I tried to adapt it to work with parallel processing. This was tricky, and it sort of works right now, sometimes printing the same loading bar twice, but this mostly happens with simple scenes (where the loading bar is less relevant anyways).

<h2>Meshes</h2>

<h3>Normal interprolation</h3>

The shading normal used to evaluate shading after a tri has been intersected by the ray is
an interprolation of all 3 vertex normals interprolated using barymetric coordinates as weights,
leading to smooth shading of objects. This is well demonstrated in scene_cat_smooth.png (no supersampling)

![cat smooth](/screenshots/scene_cat_smooth.png)

<h3>Rotation</h3>

Rotation for meshes around an arbitrary axis has been implemented.
This is well demonstrated in scene_mesh_bloom.png

![img](/screenshots/scene_mesh_bloom.png)

<h3>Texture mapping</h3>

Texture mapping on a mesh has been implemented by interprolating over the texture coordinates
of a tri with the barymetric coordinates as weights. For some reason which was not known to me
at time of submission, I have to inverse my V coordinate (v = 1 - v) in order for it to work,
but other than that it functions well.
This is well demonstrated in scene_cat_mesh.png, scene_mesh_bloom.png,
scene_balls.png and scene_mesh_bloom_ss.png

![img](/screenshots/scene_mesh_bloom_ss.png)

<h3>Specular mapping</h3>

Specular map loading and using of specular map seems functional.
This is well demonstrated in scene_mesh_bloom_ss.png


<h2>Spheres</h2>

<h3>Transparancy</h3>

Spheres that are transparent have been added, which also properly refract light.
This is well demonstrated in scene_mesh_bloom_ss.png and scene_balls.png

![img](/screenshots/scene_balls.png)

<h2>Other</h2>

<h3>Background color</h3>

Added background colour specification to the scene files.
This is well demonstrated in scene_mesh_bloom.png

<h3>Silhoutte detection</h3>

Silhoutte detection has been implemented.
This is well demonstrated in scene_cat_mesh.png

![img](/screenshots/scene_cat_mesh.png)

<h3>Depth-of-field effect</h3>

Depth-of-field effect has been implemented by distributing the position of the camera and averaging the result.

![img](/screenshots/scene_mesh_bloom_ss_dov.png)


<h2>Post-processing</h2>

![img](/screenshots/scene_balls.png)

<h3>Color mapping</h3>

Functionality which allows mapping after rendering has been implemented, allowing the user to specify
color ceilings and floors in the document. Each color value will get remapped from range 0-1 to
floor-ceil (so say we map red to 0.5-0.9, then 0 in the original picture will be mapped as 0.5 and 1
as 0.9).
This is well demonstrated in scene_mesh_bloom_ss.png and scene_balls.png

<h3>Bloom</h3>

The bloom effect was added and can be altered by changing the bloom value. The bloom value is the threshold colour value that must be reached in order to add bloom, and is the sum of the r,g,b components that it compares to for each pixel.

This is well demonstrated in scene_mesh_bloom_ss.png and scene_balls.png
