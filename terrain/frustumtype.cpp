#include "frustumtype.h"
using namespace std;

FrustumType::FrustumType()
{
 
}
bool FrustumType::PointInFrustum( float x, float y, float z )
{
   int p;

   for( p = 0; p < 6; p++ )
      if( planes[p][0] * x + planes[p][1] * y + planes[p][2] * z + planes[p][3] <= 0 )
         return false;
   return true;
}

bool FrustumType::Point3fvInFrustum( float *vec)
{
   int p;

   for( p = 0; p < 6; p++ )
      if( planes[p][0] * vec[0] + planes[p][1] * vec[1] + planes[p][2] * vec[2] + planes[p][3] <= 0 )
         return false;
   return true;
}

bool FrustumType::LineIntersectFrustum(float *v1, float *v2)
{
  // would be nice if this worked right now!
  
// QB code to tell what side of a line that a point is on in 2D:  
//  wall(i).facing = 
//  (player.y - wall(i).p1.y) * 
//  (wall(i).p2.x - wall(i).p1.x) - (player.x - wall(i).p1.x) *
//  (wall(i).p2.y - wall(i).p1.y)
  int vert;
  int side;
  bool inPoly;
  float intersect[3];
  float *vert1, *vert2;
  int axis1, axis2;
  float XYresult, XZresult, YZresult;
  bool foundIntersect = true;
  
  for (side = 0; side < 6; side++)
  {
    if (GetPlaneLineIntersection(&planes[side][0], v1, v2, intersect))
    { 
    
      inPoly = true;
      for (vert = 0; vert < 4; vert++)
      {
        // set the line segments..
        vert1 = polys[side * 4 + vert];
        if (vert == 3)  // connect last to first
        {
          vert2 = polys[side * 4 + 0];
        }
        else
        {
          vert2 = polys[side * 4 + (vert + 1)];
        }
        
        axis1 = 0; axis2 = 1;
        XYresult = (intersect[axis2] - vert1[axis2]) * 
              (vert2[axis1] - vert1[axis1]) - (intersect[axis1] - vert1[axis1]) *
              (vert2[axis2] - vert1[axis2]) ;
        
        axis1 = 0; axis2 = 2;
        XZresult = (intersect[axis2] - vert1[axis2]) * 
              (vert2[axis1] - vert1[axis1]) - (intersect[axis1] - vert1[axis1]) *
              (vert2[axis2] - vert1[axis2]) ;
        
        axis1 = 1; axis2 = 2;
        YZresult = (intersect[axis2] - vert1[axis2]) * 
              (vert2[axis1] - vert1[axis1]) - (intersect[axis1] - vert1[axis1]) *
              (vert2[axis2] - vert1[axis2]) ;
              
        if (XYresult < 0 || XZresult < 0 || YZresult < 0)
        {
          inPoly = false;
        } 
         
      } // end for line segment
       if (inPoly) return true;
    } // end if line intersect
   
  } // end for (this poly)
  return false;
}

//  wall(i).facing = 
//  (player.y - wall(i).p1.y) * 
//  (wall(i).p2.x - wall(i).p1.x) - (player.x - wall(i).p1.x) *
//  (wall(i).p2.y - wall(i).p1.y)

void FrustumType::FindPoints(void)
{
  /*
    Does intersection tests to find the points of the frustum 'box'. The polys[]
    array is then filled.
    
    Multidimentional arrays would have been cleaner, but ive been burned by that
    before.
    
    Points are ordered like this:
    the top of the frustum...
    0     3
    
    1     2
    
    bottom...
    4      6
    
    5      7
*/

  Get3PlaneIntersection(&planes[TOP_PLANE][0], &planes[LEFT_PLANE][0], &planes[FAR_PLANE][0], &points[0]); 
  Get3PlaneIntersection(&planes[TOP_PLANE][0], &planes[LEFT_PLANE][0], &planes[NEAR_PLANE][0], &points[1 * 3 + 0]); 
  Get3PlaneIntersection(&planes[TOP_PLANE][0], &planes[NEAR_PLANE][0], &planes[RIGHT_PLANE][0], &points[2 * 3 + 0]); 
  Get3PlaneIntersection(&planes[TOP_PLANE][0], &planes[FAR_PLANE][0], &planes[RIGHT_PLANE][0], &points[3 * 3 + 0]); 
 
  Get3PlaneIntersection(&planes[BOTTOM_PLANE][0], &planes[LEFT_PLANE][0], &planes[FAR_PLANE][0], &points[4 * 3 + 0]); 
  Get3PlaneIntersection(&planes[BOTTOM_PLANE][0], &planes[LEFT_PLANE][0], &planes[NEAR_PLANE][0], &points[5 * 3 + 0]);
  Get3PlaneIntersection(&planes[BOTTOM_PLANE][0], &planes[NEAR_PLANE][0], &planes[RIGHT_PLANE][0], &points[6 * 3 + 0]); 
  Get3PlaneIntersection(&planes[BOTTOM_PLANE][0], &planes[FAR_PLANE][0], &planes[RIGHT_PLANE][0], &points[7 * 3 + 0]); 
 
  // build polygons out of the frustum points
  polys[TOP_PLANE * 4 + 0] = &points[1 * 3];
  polys[TOP_PLANE * 4 + 1] = &points[0 * 3];
  polys[TOP_PLANE * 4 + 2] = &points[3 * 3];
  polys[TOP_PLANE * 4 + 3] = &points[2 * 3];

  polys[BOTTOM_PLANE * 4 + 0] = &points[4 * 3];
  polys[BOTTOM_PLANE * 4 + 1] = &points[5 * 3];
  polys[BOTTOM_PLANE * 4 + 2] = &points[6 * 3];
  polys[BOTTOM_PLANE * 4 + 3] = &points[7 * 3];
  
  polys[NEAR_PLANE * 4 + 0] = &points[2 * 3];
  polys[NEAR_PLANE * 4 + 1] = &points[6 * 3];
  polys[NEAR_PLANE * 4 + 2] = &points[5 * 3];
  polys[NEAR_PLANE * 4 + 3] = &points[1 * 3];
 
  polys[FAR_PLANE * 4 + 0] = &points[0 * 3];
  polys[FAR_PLANE * 4 + 1] = &points[4 * 3];
  polys[FAR_PLANE * 4 + 2] = &points[7 * 3];     
  polys[FAR_PLANE * 4 + 3] = &points[3 * 3];
  
  polys[LEFT_PLANE * 4 + 0] = &points[1 * 3];  
  polys[LEFT_PLANE * 4 + 1] = &points[5 * 3];  
  polys[LEFT_PLANE * 4 + 2] = &points[4 * 3];  
  polys[LEFT_PLANE * 4 + 3] = &points[0 * 3];  

  polys[RIGHT_PLANE * 4 + 0] = &points[3 * 3];  
  polys[RIGHT_PLANE * 4 + 1] = &points[7 * 3];  
  polys[RIGHT_PLANE * 4 + 2] = &points[6 * 3];  
  polys[RIGHT_PLANE * 4 + 3] = &points[2 * 3];  

}

void FrustumType::ExtractFrustum()
{
  // ripped right from http://www.markmorley.com/opengl/frustumculling.html
   float   proj[16];
   float   modl[16];
   float   clip[16];
   float   t;

   /* Get the current PROJECTION matrix from OpenGL */
   glGetFloatv( GL_PROJECTION_MATRIX, proj );

   /* Get the current MODELVIEW matrix from OpenGL */
   glGetFloatv( GL_MODELVIEW_MATRIX, modl );

   /* Combine the two matrices (multiply projection by modelview) */
   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

   /* Extract the numbers for the RIGHT plane */
   planes[0][0] = clip[ 3] - clip[ 0];
   planes[0][1] = clip[ 7] - clip[ 4];
   planes[0][2] = clip[11] - clip[ 8];
   planes[0][3] = clip[15] - clip[12];

   /* Normalize the result */
   t = sqrt( planes[0][0] * planes[0][0] + planes[0][1] * planes[0][1] + planes[0][2] * planes[0][2] );
   planes[0][0] /= t;
   planes[0][1] /= t;
   planes[0][2] /= t;
   planes[0][3] /= t;

   /* Extract the numbers for the LEFT plane */
   planes[1][0] = clip[ 3] + clip[ 0];
   planes[1][1] = clip[ 7] + clip[ 4];
   planes[1][2] = clip[11] + clip[ 8];
   planes[1][3] = clip[15] + clip[12];

   /* Normalize the result */
   t = sqrt( planes[1][0] * planes[1][0] + planes[1][1] * planes[1][1] + planes[1][2] * planes[1][2] );
   planes[1][0] /= t;
   planes[1][1] /= t;
   planes[1][2] /= t;
   planes[1][3] /= t;

   /* Extract the BOTTOM plane */
   planes[2][0] = clip[ 3] + clip[ 1];
   planes[2][1] = clip[ 7] + clip[ 5];
   planes[2][2] = clip[11] + clip[ 9];
   planes[2][3] = clip[15] + clip[13];

   /* Normalize the result */
   t = sqrt( planes[2][0] * planes[2][0] + planes[2][1] * planes[2][1] + planes[2][2] * planes[2][2] );
   planes[2][0] /= t;
   planes[2][1] /= t;
   planes[2][2] /= t;
   planes[2][3] /= t;

   /* Extract the TOP plane */
   planes[3][0] = clip[ 3] - clip[ 1];
   planes[3][1] = clip[ 7] - clip[ 5];
   planes[3][2] = clip[11] - clip[ 9];
   planes[3][3] = clip[15] - clip[13];

   /* Normalize the result */
   t = sqrt( planes[3][0] * planes[3][0] + planes[3][1] * planes[3][1] + planes[3][2] * planes[3][2] );
   planes[3][0] /= t;
   planes[3][1] /= t;
   planes[3][2] /= t;
   planes[3][3] /= t;

   /* Extract the FAR plane */
   planes[4][0] = clip[ 3] - clip[ 2];
   planes[4][1] = clip[ 7] - clip[ 6];
   planes[4][2] = clip[11] - clip[10];
   planes[4][3] = clip[15] - clip[14];

   /* Normalize the result */
   t = sqrt( planes[4][0] * planes[4][0] + planes[4][1] * planes[4][1] + planes[4][2] * planes[4][2] );
   planes[4][0] /= t;
   planes[4][1] /= t;
   planes[4][2] /= t;
   planes[4][3] /= t;

   /* Extract the NEAR plane */
   planes[5][0] = clip[ 3] + clip[ 2];
   planes[5][1] = clip[ 7] + clip[ 6];
   planes[5][2] = clip[11] + clip[10];
   planes[5][3] = clip[15] + clip[14];

   /* Normalize the result */
   t = sqrt( planes[5][0] * planes[5][0] + planes[5][1] * planes[5][1] + planes[5][2] * planes[5][2] );
   planes[5][0] /= t;
   planes[5][1] /= t;
   planes[5][2] /= t;
   planes[5][3] /= t;
   
   FindPoints();
}

