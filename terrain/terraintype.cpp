/* 
terraintype.cpp
*/

#include "terraintype.h"
#include "tgatype.h"


TerrainType::TerrainType()
{
  int i;
  vertices = NULL;
  textureCoords = NULL;
  triStripMeshIndices = NULL;
  colors = NULL;
  viewIndexCount = 0;
  viewTriIndices = NULL;
 
  buildDebugOutput = false;
  for (i = 0; i < NUM_TRI_STRIP_LISTS; i++)
  {
    triStripList[i].stripIndices = NULL;
    triStripList[i].listCount = 0;
  } 
}



TerrainType::~TerrainType()
{
  int i;
  if (vertices)
    delete vertices;
  if (triStripMeshIndices)
    delete triStripMeshIndices;
  if (colors)
    delete colors;
  if (viewTriIndices) delete viewTriIndices;
  if (textureCoords) delete textureCoords;
  for (i = 0; i < NUM_TRI_STRIP_LISTS; i++)
  {
    delete triStripList[i].stripIndices;
  } 
}

TerrainError TerrainType::LoadHeightmap(char *fileName, int inUnitScale, float heightScale)
{
  TGAType heightFile;
  unitScale = inUnitScale;
  float uvStep, u, v;
  int pos = 0, r, c, vertIndex, uvVertIndex;
  float color = 0.0f, red = 0.1f, blue = 0.1f, green = 0.0f;
  int curMeshVert = 0;
  int cellIndex[4];
  if (heightFile.LoadFile(fileName) == TGA_SUCCESS)
  {
    if (heightFile.GetBpp() == 8)
    {
      cout << "Heightmap loaded!!!   " << heightFile.GetHeight() 
      << " X " << heightFile.GetWidth() << endl;
    }
    else
    {
      #ifdef CONSOLE_OUTPUT                  
      cout << "Targa file must be 8 bpp greyscale!!!" << endl;
      system("PAUSE"); 
      #endif
      return TERRAIN_ERROR;
    }
	}
	else 
  {
    #ifdef CONSOLE_OUTPUT
    cout << "error loading file: " << fileName <<" TGA error: "<< heightFile.LastError() << endl;
    system("PAUSE");
    #endif
    return TERRAIN_ERROR;
  
  }
  
  dataHeight = heightFile.GetHeight();
  dataWidth = heightFile.GetWidth();
  width = dataWidth * unitScale;
  height = dataHeight * unitScale;
  numTriangles = (dataWidth - 1) * (dataHeight - 1) * 2;
  numVertices = dataWidth * dataHeight;
  
  numTriStripMeshIndices = dataWidth * 2 * (dataHeight - 1);
  triStripMeshWidth = dataWidth * 2;
  
  // assume that the culling code could extract more than the total number of 
  // verts, since duplication is needed to connect everything
  maxViewVerts = (dataWidth + dataWidth /2) * (dataHeight + dataHeight /2);
  viewTriIndices = new unsigned int[MAX_VIEW_VERTS];
  
  // allocate vertices
  vertices = new float[dataWidth * dataHeight * 3];
  colors  = new float[dataWidth * dataHeight  *3];
  textureCoords = new float[dataWidth * dataHeight * 2];
  
  //allocate triangle indices
  triStripMeshIndices = new unsigned int[dataWidth * 2 * (dataHeight - 1)];
  
  // set up terrain vertices and colors (wireframe mode) using height data
  // texture coords are also prepared
  color = 0.0f, red = 0.1f, blue = 0.1f, green = 0.0f;
  vertIndex = 0; uvVertIndex = 0;
  uvStep = 1.0f / 256.0f; u = 0; v = 0;
  for (r = 0; r < dataHeight; r++)
  {
    for (c = 0; c < dataWidth; c++)
    {
      //heightData[r * terrainWidth + c] = heightFile.buffer[pos];
      colors[vertIndex] = red;
     
      vertices[vertIndex++] = c * unitScale;
      
      colors[vertIndex] = (float)((float)heightFile.buffer[pos] / 256.0f) + 0.2f;
      
      vertices[vertIndex++] = heightFile.buffer[pos] * heightScale;
      
      colors[vertIndex] = blue;
      vertices[vertIndex++] = r * unitScale;
      pos++;
      
      textureCoords[uvVertIndex++] = u;
      textureCoords[uvVertIndex++] = v;
      u += uvStep;
    } 
    u = 0; v += uvStep;
  }

  heightFile.Free(); 
  
  // build the triangle strip array in case brute-force is desired
  curMeshVert = 0;
  for (r = 0; r < dataHeight -1; r++)
  {
    for (c = 0; c < dataWidth; c++)
    {
      cellIndex[0] = r * dataWidth  + (c );
      cellIndex[1] = (r+1) * dataWidth  + (c );
      cellIndex[2] = (r+1) * dataWidth  + ((c+1) );
      cellIndex[3] = r * dataWidth  + ((c+1) );
      
      triStripMeshIndices[curMeshVert++] = cellIndex[0];
      triStripMeshIndices[curMeshVert++] = cellIndex[1];
      
    }
  }  
  
  return TERRAIN_SUCESS;  
}


void TerrainType::SetView(FrustumType *inFrustum, float *viewVec)
{ 
  // store pointer to frustum and position of viewer (vector)
  frustumPtr = inFrustum;
  viewer = viewVec;
}


void TerrainType::BuildView(float inCullValue)
{
  // reset counters and call recursive TerrainSplit to build a list of triangle
  // strips based on current view and the detail level
  
  cullValue = inCullValue;
  
  if (buildDebugOutput) out.open("out.txt", ios::out);
  
  // reset counts for this frame
  viewIndexCount = 0; 
  viewTriCount = 0;
  viewTriStripCount = 0;
  
  TerrainSplit(dataHeight - 1, 0, 0, dataWidth - 1, 0);

  if(buildDebugOutput)
  {
    out << "viewIndexCount: "<<viewIndexCount<<endl;
    buildDebugOutput = false;
    out.close();
  }
}

void TerrainType::TerrainSplit(int rowBL, int colBL, int rowTR, int colTR, int depth)
{
  /* recursive function to either subdivide a terrain region, or make into 
      triangles based on distance and level of detail
      
     vectors of square like so:
      v0       v3
      |         |
      |         |
      |         |
      v1-------v2
  
  */
  int i;
  int r, c;
  int indexCount = 0;
  int size = (colTR - colBL + 1) * unitScale;
  float distFromViewer = 0.0;
  float *midPoint;
  int numRows = rowBL - rowTR + 1;
  int numCols = colTR - colBL + 1;
  int midRow = rowTR + numRows / 2;
  int midCol = colBL + numCols / 2;
  int cellIndex[4];
  int intersectCount, blockSide;
  bool intersectTest;
  bool tooFar = false;
  bool cull = true;   // assume we cull this square
  
 
  float *blockVecs[4];
  float *vec0, *vec1, *vec2, *vec3;
  float *startVec, *endVec;
  blockVecs[0] = vec0 = GetVertex(rowTR, colBL);
  blockVecs[1] = vec1 = GetVertex(rowBL, colBL);
  blockVecs[2] = vec2 = GetVertex(rowBL, colTR);
  blockVecs[3] = vec3 = GetVertex(rowTR, colTR);
  
  
  if (buildDebugOutput) 
  { 
    out << endl<<"**** Depth: " <<depth << " datacoords: " << rowBL<<" "<<colBL<<" "<<rowTR<<" "<<colTR<<
    " mid: "<<midRow<<" "<<midCol<<" Size: "<<size<< endl;
    out << "numRows: " <<numRows<<endl;
    out << "vec0: "<<vec0[0]<<" "<<vec0[1]<<" "<<vec0[2]<<endl;
    out << "vec1: "<<vec1[0]<<" "<<vec1[1]<<" "<<vec1[2]<<endl;
    out << "vec2: "<<vec2[0]<<" "<<vec2[1]<<" "<<vec2[2]<<endl;
    out << "vec3: "<<vec3[0]<<" "<<vec3[1]<<" "<<vec3[2]<<endl;

  }

  // check if any of the block's points are in the frustum
  if (frustumPtr->Point3fvInFrustum(vec0) == true ||
      frustumPtr->Point3fvInFrustum(vec1) == true ||
      frustumPtr->Point3fvInFrustum(vec2) == true ||
      frustumPtr->Point3fvInFrustum(vec3) == true)
  {
    cull = false;
    if (buildDebugOutput) out << "at least one point is in frustum"<<endl;
  }  
  // check if viewer is in the block on XZ plane
  else if (viewer[0] > vec0[0] && viewer[0] < vec3[0] && 
      viewer[2] > vec0[2] && viewer[2] < vec1[2])
  {
    cull = false;
    if (buildDebugOutput) out << "NO points are in frustum"<<endl;
    if (buildDebugOutput) out << "viewer is inside the block"<<endl;
  }
  else  
  {
    if (buildDebugOutput) out << "viewer is NOT inside the block"<<endl;
   
    // check if at least one of the block's sides intersect 2 or more planes
    intersectTest = false;
    for (blockSide = 0; blockSide < 4; blockSide++)
    {
      if (blockSide < 3)
      {
        startVec = blockVecs[blockSide];
        endVec = blockVecs[blockSide + 1];
      }
      else
      {
        startVec = blockVecs[blockSide];
        endVec = blockVecs[0];
      }
      intersectCount = 0;
      for (i = 0; i < 6; i++)
      {
        if (PlaneLineIntersect(&frustumPtr->planes[i][0], startVec, endVec))
        {
          intersectCount++;
          if (intersectCount > 1) intersectTest = true;
        }
      }
    }
    if (intersectTest)
    {
      cull = false;
      if (buildDebugOutput) out << "a side intersects at least 2 planes"<<endl;
    }
    else
       if (buildDebugOutput) out << "no sides intersect at least 2 planes"<<endl;
  }
  
  // return if this block isnt visible
  if (cull) 
  {
    if (buildDebugOutput) out << "this block is getting culled" <<endl;
    return;
  }
  midPoint = GetVertex(midRow, midCol); // get mid point of midRow/midCol
                                        // rather than dividing by 2

  distFromViewer = VectorsDistanceApart(midPoint, viewer) - size;
  if (distFromViewer < 0 ) distFromViewer = 0;
  
  if (buildDebugOutput)
  { 
    out <<"dist from viewer: " << distFromViewer <<endl;
    out << "midPoint: "<<midPoint[0]<<" "<<midPoint[1]<<" "<<midPoint[2]<<endl;
  }
  
  // use detail value use determine if the block needs more detail
  if (size * cullValue > distFromViewer)
  {
    tooFar = true;
  } 
    
  if (tooFar == true && 
      numRows >= MIN_RECURSE_ROWS && 
      depth < MAX_DEPTH)
  {
    if (buildDebugOutput) out <<"recursing down!!!" <<endl;
    // recurse TL, BL, BR, TR (same as diagram above)
    TerrainSplit(midRow, colBL, rowTR, midCol, depth + 1);
    TerrainSplit(rowBL, colBL, midRow, midCol, depth + 1);
    TerrainSplit(rowBL, midCol, midRow, colTR, depth + 1);
    TerrainSplit(midRow, midCol, rowTR, colTR, depth +1 );  
  }
  else // stop here and add triangles
  {
    if (numRows < MIN_RECURSE_ROWS && tooFar) //need just add all the cells
    {
      if (buildDebugOutput) out <<"only "<<numRows<<" rows, just adding them all"<<endl;
      for (r = rowTR; r < rowBL ; r++)
      {
        // do we need to allocate a list?
        if (triStripList[viewTriStripCount].stripIndices == NULL)
        {   
            if (buildDebugOutput) out <<"adding an array!!!"<<endl;
            triStripList[viewTriStripCount].stripIndices =
              new int[MAX_TRI_STRIP_VERTS];
            triStripList[viewTriStripCount].listCount = 0;            
        }
        else
        {
          triStripList[viewTriStripCount].listCount = 0;
        }
        
        indexCount = 0;
        for (c = colBL; c <= colTR ; c++)
        {
          
          cellIndex[0] = r * dataWidth  + c;
          cellIndex[1] = (r+1) * dataWidth  + c;
          cellIndex[2] = (r+1) * dataWidth  + (c+1);
          cellIndex[3] = r * dataWidth  + (c+1);
          
          triStripList[viewTriStripCount].stripIndices[indexCount++] = cellIndex[0];
          triStripList[viewTriStripCount].stripIndices[indexCount++] = cellIndex[1];

          viewTriCount += 2;
        }
        triStripList[viewTriStripCount].listCount = indexCount;
        if (buildDebugOutput) out <<"listCount: " << indexCount<<endl;
         viewTriStripCount++;
      }  
    } 
    else // build 2 triangles out of this square
    {
      if (buildDebugOutput) out <<"building 2 triangles out of this"<<endl;
      cellIndex[0] = rowTR * dataWidth  + colBL;
      cellIndex[1] = rowBL * dataWidth  + colBL;
      cellIndex[2] = rowBL * dataWidth  + colTR;
      cellIndex[3] = rowTR * dataWidth  + colTR;
      
       // do we need to allocate a list?
      if (triStripList[viewTriStripCount].stripIndices == NULL)
      {  
          if (buildDebugOutput) out <<"adding an array!!!"<<endl;
          triStripList[viewTriStripCount].stripIndices =
            new int[MAX_TRI_STRIP_VERTS];
          triStripList[viewTriStripCount].listCount = 0;            
      }
      else
      {
        triStripList[viewTriStripCount].listCount = 0;
      }
    
      triStripList[viewTriStripCount].stripIndices[0] = cellIndex[0];
      triStripList[viewTriStripCount].stripIndices[1] = cellIndex[1];
      triStripList[viewTriStripCount].stripIndices[2] = cellIndex[3];
      triStripList[viewTriStripCount].stripIndices[3] = cellIndex[2];
      triStripList[viewTriStripCount].listCount = 4;
   
      viewTriCount += 2;
      
      viewTriStripCount++;
    }
  }
}

float * TerrainType::GetVertex(int r, int c)
{
  return &vertices[r * dataWidth * 3 + (c*3)];
}

