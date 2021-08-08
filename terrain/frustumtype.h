#ifndef FRUSTUMTYPE_H
#define FRUSTUMTYPE_H

#include <iostream>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#include <glut.h>
#else
#include <GL/glut.h>
#endif
#include "travmath.h"

class FrustumType
{

  public:
    enum {RIGHT_PLANE = 0, LEFT_PLANE, BOTTOM_PLANE, TOP_PLANE, FAR_PLANE, NEAR_PLANE};

    float planes[6][4];
    float points[8 * 3];
    float *polys[6 * 4];
    FrustumType();
    void ExtractFrustum();
    bool PointInFrustum( float x, float y, float z );
    bool Point3fvInFrustum( float *vec);
    bool LineIntersectFrustum(float *v1, float *v2);
    void FindPoints(void);

};





#endif
