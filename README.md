# vanche_3D
 A 3d vtuber software based on OpenGL

A 3d vtuber software based on the VRM-1.0 specification. Uses OpenGL to display the model and uses dlib for facial recognition. 

# To build
Have OpenCV installed locally and added to your path
- git submodule update --init
- mkdir build
- cd build
- cmake .. --DCMAKE_BUILD_TYPE=Release
- cmake --build .