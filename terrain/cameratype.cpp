/*
cameratype.cpp
W Travis Jones '03
maintains camera position, up and forward vectors
designed for gluLookAt()

first set a position:
  cam.SetPos(terrain.width/4, 120.0, terrain.height/4); // middle of terrain
  cam.Rotate(0.0, 0.0, 200.0); // facing forward
  
to give to opengl:
  //extract the positions
  pos = cam.GetPos(); 
  look = cam.GetLook();
  up = cam.GetUp();

  gluLookAt( pos[0], pos[1], pos[2],
             look[0],look[1], look[2],
             up[0], up[1], up[2] );

note: Roll is messed up, dont do it
*/

#include "cameratype.h"
using namespace std;


CameraType::CameraType()
{
  pos[0] = 0;
  pos[1] = 0;
  pos[2] = 0;
  
  look[0] = 0.0;
  look[1] = 0.0;
  look[2] = 0.0;
  
  lookStart[0] = 0.0;
  lookStart[1] = 0.0;
  lookStart[2] = 1.0;
  
  upStart[0] = up[0] = 0.0;
  upStart[1] = up[1] = 1.0;
  upStart[2] = up[2] = 0.0;
  
  roll = 0.0;
  pitch = 0.0;
  yaw = 0.0;
  Rotate(0.0, 0.0, 0.0);
}

void CameraType::SetPos(float x, float y, float z)
{
  pos[0] = x;
  pos[1] = y;
  pos[2] = z;
  
  Rotate(0.0, 0.0, 0.0);  // reset the look unit vector
}

void CameraType::SetUpStart(float x, float y, float z)
{
  upStart[0] = x;
  upStart[1] = y;
  upStart[2] = z;
}

void CameraType::SetLookStart(float x, float y, float z)
{
  lookStart[0] = x;
  lookStart[1] = y;
  lookStart[2] = z;
}

void CameraType::MovePos(float deltaX, float deltaY, float deltaZ)
{
  pos[0] += deltaX;
  look[0] += deltaX;
  pos[1] += deltaY;
  look[1] += deltaY;
  pos[2] += deltaZ;
  look[2] += deltaZ;
}

void CameraType::MoveAlongLookVector(float move)
{
  float viewVector[3];
  
  for (int i = 0; i < 3; i++) 
    viewVector[i] = look[i] - pos[i];
  
  pos[0] += viewVector[0] * move;
  pos[1] += viewVector[1] * move;
  pos[2] += viewVector[2] * move;
  look[0] += viewVector[0] * move;
  look[1] += viewVector[1] * move;
  look[2] += viewVector[2] * move; 
 
}

float * CameraType::GetPos(void)
{
  return &pos[0];
}

float * CameraType::GetLook(void)
{
  return &look[0];
}

float * CameraType::GetUp(void)
{
  return &up[0];
}

float CameraType::GetPitch(void) {return pitch;}

float CameraType::GetRoll(void) {return roll;}

float CameraType::GetYaw(void) {return yaw;}

void CameraType::Rotate(float deltaRoll, float deltaPitch, float deltaYaw)
{ 
  int i;
  roll += deltaRoll; 
  if (roll >= 360) roll = (0 + roll - 360);
  if (roll < 0) roll = (359 + roll);
  
  pitch += deltaPitch; 
  if (pitch >= 360) pitch = (0 + pitch - 360);
  if (pitch < 0) pitch = (359 + pitch);

  yaw += deltaYaw; 
  if (yaw >= 360) yaw = 0;
  if (yaw < 0) yaw = (359 + yaw);
  
  for (i = 0; i < 3; i++) 
  {
    look[i] = lookStart[i];
    up[i] = upStart[i];
  }
  
  RotateVector(look, pitch, yaw, roll);
  RotateVector(up, pitch, yaw, roll);

  // move look vector back to world space
  for (i = 0; i < 3; i++) look[i] += pos[i];    
}


void CameraType::RotateVector(float *v, float xAngle, float yAngle, float zAngle)
{
  float temp[3] =  {0.0, 0.0, 0.0};
  
 //-rotate on x axis-
  temp[1] = cos(DEG2RAD(xAngle)) * v[1] - sin(DEG2RAD(xAngle)) * v[2];
  temp[2] = sin(DEG2RAD(xAngle)) * v[1] + cos(DEG2RAD(xAngle)) * v[2];

  //-put new values into translated array-
  v[1] = temp[1];
  v[2] = temp[2];

  //-rotate on y axis-
  temp[0] = cos(DEG2RAD(yAngle)) * v[0] + sin(DEG2RAD(yAngle)) * v[2];
  temp[2] = (-sin(DEG2RAD(yAngle))) * v[0] + cos(DEG2RAD(yAngle)) * v[2];

  v[0] = temp[0];
  v[2] = temp[2];

  //-rotate on z axis-
  temp[0] = cos(DEG2RAD(zAngle)) * v[0] - sin(DEG2RAD(zAngle)) * v[1]; 
  temp[1] = sin(DEG2RAD(zAngle)) * v[0] + cos(DEG2RAD(zAngle)) * v[1]; 
  v[0] = temp[0];
  v[1] = temp[1];
}
