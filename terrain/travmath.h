#ifndef TRAVMATH_H
#define TRAVMATH_H

#include <math.h>
#include <iostream>
using namespace std;

const double	EPSILON = 1e-5;

void MatrixMul4x4 (float *source1, float *source2, float *dest);
void Normalize(float *vec);
float DotProduct(float *v1, float *v2);
void CrossProduct(float *v1, float *v2, float *result);
bool PlaneLineIntersect(float *plane, float *v1, float *v2);
float PlaneLineIntersectVal(float *plane, float *v1, float *v2);
float DistancePointPlane(float *, float *);
bool GetPlaneLineIntersection(float *plane, float *v1, float *v2, float *intersect);
float VectorsDistanceApart(float *, float *);
bool Get3PlaneIntersection(float *p1, float *p2, float *p3, float *point);

#endif
