/* cameratype.h
  description is in .cpp file
*/

#ifndef CAMERATYPE_H
#define CAMERATYPE_H

#ifndef DEG2RAD
  #define DEG2RAD( x )     ((x) * PI / 180.0f)
#endif

#ifndef PI
  #define PI 3.141592
#endif

#include <iostream>
#include <math.h>

class CameraType 
{
private:
  
  
  float pos[3];  // camera coords in world space
  float look[3];  // unit vector where the camera looks
  float up[3];
  float upStart[3];
  float lookStart[3];
public:
  
  float yaw, pitch, roll; 
  CameraType();
  
  void SetPos(float, float, float);
  void SetUpStart(float, float, float);
  void SetLookStart(float, float, float);
  void CameraType::Rotate(float deltaRoll, float deltaPitch, float deltaYaw);
  void CameraType::MovePos(float deltaX, float deltaY, float deltaZ);
  float *CameraType::GetPos(void);
  float *CameraType::GetLook(void);
  float *CameraType::GetUp(void);
  float CameraType::GetPitch(void);
  float CameraType::GetRoll(void);
  float CameraType::GetYaw(void);
  void CameraType::MoveAlongLookVector(float move);
  void CameraType::RotateVector(float *v, float xAngle, float yAngle, float zAngle);
};

#endif
