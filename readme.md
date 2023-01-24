# terrain_d

Terrain demo from 2003 that utilizes a quad tree for LOD. OpenGL + GLUT is used for rendering.


## Overview

The original intention was to design a simple terrain LOD system from scratch. I built an algorithm to recurse into the data set as needed based on distance. I think of it as a quadtree, viewed from above, that is generated every frame.

The fatal flaw is a t-junction problem when the LOD changes. I have a few ideas on how to correct it, but the performance impact may necessitate the need to use another method.


## Build

### Linux

```
cmake -Bbuild
cmake --build build
cd build
./terrain_d
```

### Mac OS

Tested with Xcode 4. GLUT is bundled with the OS but you may need to install the command line tools for Xcode.

### Windows

Win32 port of GLUT is provided via http://user.xmission.com/~nate/glut.html


## TODO

- Fix z order issue with hud text on mac.
- Emscripten build.
