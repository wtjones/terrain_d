// travmath.cpp

#include "travmath.h"

void MatrixMul4x4(float *source1[4], float *source2[4], float *dest[4])
{ //i wouldnt trust this right now, in fact dont use it
  int i, j, k;

  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 4; j++)
    {
      dest[i][j] = 0;
      for (k = 0; k < 4; k++)
      {
        dest[i][j] += source1[i][j] * source2[k][j];
      }
    }
  }
}
   
void Normalize(float *vec)
{
  float length;

  length = (float)sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
  vec[0] /= length;
  vec[1] /= length;
  vec[2] /= length;
}
   
float DotProduct(float *v1, float *v2)
{
  //return v1[0] * v2[0] * v1[1] * v2[1] * v1[2] * v2[2];
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void CrossProduct(float *v1, float *v2, float *result)
{
  result[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
  result[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
  result[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
}

float VectorsDistanceApart(float *v1, float *v2)
{
  //If the two points are (x1,y1,z1) and (x2,y2,z2) then the distance is
  //sqrt( (x1-x2)^2 + (y1-y2)^2 + (z1-z2)^2 )
  return (float)sqrt( 
        (v1[0] - v2[0]) * (v1[0] - v2[0]) +          
        (v1[1] - v2[1]) * (v1[1] - v2[1]) +
        (v1[2] - v2[2]) * (v1[2] - v2[2]));
}


bool PlaneLineIntersect(float *plane, float *v1, float *v2)
{
/*
quote from http://nate.scuzzy.net/docs/normals/ 
"If we have a point p1, we can find out its position with respect to the plane
 by plugging it into the plane equation:

value = (A * p1.x) + (B * p1.y) + (C * p1.z) + D

After plugging the point into the plane equation we get a number, value, 
which provides a great amount of information to us. If value is less than 0, 
the point is behind the plane. If value is greater than zero, the point is in 
front of the plane."
*/

  float v1pos, v2pos;
  v1pos = plane[0] * v1[0] + plane[1] * v1[1] + plane[2] * v1[2] + plane[3];
  v2pos = plane[0] * v2[0] + plane[1] * v2[1] + plane[2] * v2[2] + plane[3];
  
  if (v1pos > 0 && v2pos > 0) return false;
  else if (v1pos < 0 && v2pos < 0) return false;
  else return true;
}


bool GetPlaneLineIntersection(float *plane, float *v1, float *v2, float *intersect)
{
  /*Calculate intersection of line and plane.
    Returns true/false depending on intersection, and fills intersect (vector)
    with the point of intersection
    Borrowed heavily from http://folk.uio.no/stefanha/Tutorials.html
  */
  int i;
  float direction[3];//, tempDirection[3];
  double num, denom, percentage;
  
  if (!PlaneLineIntersect(plane, v1, v2))
    return false;
    
  for (i = 0; i < 3; i++)
    direction[i] = v2[i] - v1[i];
    
  Normalize(direction);
  denom = DotProduct(plane, direction);
  
  if (fabs(denom) < EPSILON)
    return false;
  
 
  num = -DistancePointPlane(v1, plane);
  percentage = num / denom;
  
  for (i = 0; i < 3; i++)
    intersect[i] = v1[i] + direction[i] * percentage;
    
  //psuedo code for returning percentage:
  //  Percentage		= Percentage / ( End - Start ).Magnitude ( );
  return true;
}


float DistancePointPlane(float *v, float *plane)
{
  return fabs(DotProduct(plane, v) + plane[3]);
}


float PlaneLineIntersectVal(float *plane, float *v1, float *v2)
{
  /*
    Subject 5.05: How do I find the intersection of a line and a plane?
    If the plane is defined as:
        a*x + b*y + c*z + d = 0
    and the line is defined as:
        x = x1 + (x2 - x1)*t = x1 + i*t
        y = y1 + (y2 - y1)*t = y1 + j*t
        z = z1 + (z2 - z1)*t = z1 + k*t
    Then just substitute these into the plane equation. You end up    with:
        t = - (a*x1 + b*y1 + c*z1 + d)/(a*i + b*j + c*k)
    When the denominator is zero, the line is contained in the plane
    if the numerator is also zero (the point at t=0 satisfies the
    plane equation), otherwise the line is parallel to the plane.
  */
  float i = v2[0] - v1[0];
  float j = v2[1] - v1[1];
  float k = v2[2] - v1[2];

  return -(DotProduct(v1, plane)+ plane[3]) /
    (plane[0] * i + plane[1] * j + plane[2] * k);
}


bool Get3PlaneIntersection(float *p1, float *p2, float *p3, float *point)
{
  float crossTemp[3];
  float cross1[3], cross2[3], cross3[3];
  float part1[3], part2[3], part3[3];
  int i;
  
  CrossProduct(p2, p3, cross1);
  float denom = DotProduct(p1, cross1);
  
  if (denom == 0) return false;

  CrossProduct(p3, p1, cross2);
  CrossProduct(p1, p2, cross3);
  
  for (i = 0; i < 3; i++)
  {
    part1[i] = cross1[i] * -p1[3];
    part2[i] = cross2[i] * p2[3];
    part3[i] = cross3[i] * p3[3];
  }
  
  for (i = 0; i < 3; i++)
  {
    point[i] = ( part1[i] - part2[i] - part3[i] ) / denom;
  }
  
  return true;
  
  /*
  v = ( a.n.Cross ( b.n ) * -d - b.n.Cross ( n )  * a.d - n.Cross ( a.n ) * b.d ) / denom;
  
  part1: a.n.Cross ( b.n ) * -d
  part2: b.n.Cross ( n )  * a.d
  part3: n.Cross ( a.n ) * b.d
  
  v = (part1 - part2 - part3) / denom;
  */
    
}


