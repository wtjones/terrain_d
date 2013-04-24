#ifndef TERRAINTYPE_H
#define TERRAINTYPE_H

#include <iostream>
#include <fstream>
#ifdef __APPLE__
    #include <GLUT/GLUT.h>
#elif defined WIN32
    #include <windows.h>
    #include <glut.h>
#endif
#include "travmath.h"
#include "frustumtype.h"


#define TERRAIN_SUCESS 0
#define TERRAIN_ERROR 1
#define MAX_VIEW_VERTS 100000
#define CULL_VALUE 10
#define MIN_RECURSE_ROWS 4
#define MAX_DEPTH 50
#define NUM_TRI_STRIP_LISTS 10000
#define MAX_TRI_STRIP_VERTS 20  //even this is overkill


using namespace std;

typedef int TerrainError;

struct TriStripListType
{
  int *stripIndices;
  int listCount;
};

class TerrainType
{
  private:
    
    ofstream out;
    int unitScale;
    float *viewer;
    FrustumType *frustumPtr;
    float cullValue;
  public: 
   
    bool buildDebugOutput; //if true, the quadtree will be output to a file
                           //for one frame, then set back to false

    float *colors;
    float *vertices;
    float *textureCoords;
    int dataWidth, dataHeight;    // dimension of grid row and col (not worldspace)
    int width, height;
    int numTriangles;
    int numVertices;
 
    unsigned int *triStripMeshIndices;
    int numTriStripMeshIndices;
    int triStripMeshWidth;
    // view culling variables
    int maxViewVerts;
    TriStripListType triStripList[NUM_TRI_STRIP_LISTS];
    unsigned int *viewTriIndices;
    // view counters
    int viewIndexCount;
    int viewTriCount;
    int viewTriStripCount;
    
    TerrainType();
    ~TerrainType();
    TerrainError LoadHeightmap(char *, int, float);
    float *GetVertex(int r, int c);
    void BuildView(float);
    
    void SetView(FrustumType *, float *);
    //bool TerrainType::PointInClipPlanes( float x, float y, float z );
    void TerrainSplit(int rowBL, int colBL, int rowTR, int colTR, int depth);
};



#endif
