#Contest Submission

I have made a couple of expansions based on that which was required, some for the competition,
some over time. I will list that which is implemented here.

Note that few instances of code duplication exist, which is a shame, but I did not have the time to
wrap everything up neatly. My bad!

Running the program is like what you would expect for the raytracer. Run the
CMake file, then cd to build, run make, and run the program using ./ray ../Scenes/
scene_name.json

Note that running the scene scene_mesh_bloom_ss_dov will take a *very* long time.

##Parallel processing##

A quite easy quality-of-life improvement, can be found in the for loops of scene::render()

###Loading bar###

Implemented a little loading bar which indicates the process of rendering. It was implemented prior
to parallel processing, and as such, I tried to adapt it to work with parallel processing. This was tricky, and it sort of works right now, sometimes printing the same loading bar twice, but this mostly happens with simple scenes (where the loading bar is less relevant anyways).


##Meshes##

###Normal interprolation###

The shading normal used to evaluate shading after a tri has been intersected by the ray is
an interprolation of all 3 vertex normals interprolated using barymetric coordinates as weights,
leading to smooth shading of objects. This is well demonstrated in scene_cat_smooth.png

###Rotation###

Rotation for meshes around an arbitrary axis has been implemented.
This is well demonstrated in scene_mesh_bloom.png

###Texture mapping###

Texture mapping on a mesh has been implemented by interprolating over the texture coordinates
of a tri with the barymetric coordinates as weights. For some reason which was not known to me
at time of submission, I have to inverse my V coordinate (v = 1 - v) in order for it to work,
but other than that it functions well.
This is well demonstrated in scene_cat_mesh.png, scene_mesh_bloom.png,
scene_balls.png and scene_mesh_bloom_ss.png

###Specular mapping###

Specular map loading and using of specular map seems functional.
This is well demonstrated in scene_mesh_bloom_ss.png


##Spheres##

###Transparancy###

Spheres that are transparent have been added, which also properly refract light.
This is well demonstrated in scene_mesh_bloom_ss.png and scene_balls.png

##Other##

###Background color###

Added background colour specification to the scene files.
This is well demonstrated in scene_mesh_bloom.png

###Silhoutte detection###

Silhoutte detection has been implemented.
This is well demonstrated in scene_cat_mesh.png

###Depth-of-field effect###

Depth-of-field effect has been implemented by distributing the position of the camera and averaging the result.
This is well demonstrated in scene_cat_mesh.png


##Post-processing##

###Color mapping###

Functionality which allows mapping after rendering has been implemented, allowing the user to specify
color ceilings and floors in the document. Each color value will get remapped from range 0-1 to
floor-ceil (so say we map red to 0.5-0.9, then 0 in the original picture will be mapped as 0.5 and 1
as 0.9).
This is well demonstrated in scene_mesh_bloom_ss.png and scene_balls.png

###Bloom###

The bloom effect was added and can be altered by changing the bloom value. The bloom value is the threshold colour value that must be reached in order to add bloom, and is the sum of the r,g,b components that it compares to for each pixel.

This is well demonstrated in scene_mesh_bloom_ss.png and scene_balls.png
