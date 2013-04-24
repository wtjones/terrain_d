/* 
terrainapp.cpp
March'03 
William Travis Jones

Description:
  Demo app for TerrainType class.
  Loads an 8-bit greyscale targa file to use as a heightmap, then renders a 
  terrain using a quad-tree based level-of-detail scheme
  
Build Info:
  under mingw/dev-c++ build with: -lglut32 -lopengl32 -lglu32 
  under vc++ 6, add opengl32.lib glu32.lib glut32.lib

WORK LOG:
---
d_13 - 4/7/03
 view culling now works, but need to put the triangles into index array
---
d_16 - 4/8/03
 texturing now works

d_20 - 4/11/03
 mouse control! w00t!!!
 
---
TODO LIST:

-lighting
-box culling instead of square culling to avoid pop-in
*/


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef WIN32
  #include <windows.h>
#endif
#include <GL/glut.h>

#include "cameratype.h"
#include "terraintype.h"
#include "frustumtype.h"
#include "travmath.h"
#include "tgatype.h"

using namespace std;

// CONSTANTS ////////////////////////////
#define CONSOLE_OUTPUT  //comment out to stop cout calls in classes
#define LOOP_WAIT 12
#define TERRAIN_LIST 1
#define TERRAIN_UNITS 10
#define TERRAIN_HEIGHT_SCALE 0.5f
#define MOVE_SPEED_START 8.0f
#define ROTATE_SPEED 3.0f
#define DETAIL_START 20.0f


// PROTOTYPES //////////////
void display(void);
void OnMouseMove(int, int);
void myinit();
int GetHeightData(int r, int c);
void DrawText(float r, float g, float b);


// GLOBALS /////////////////////////
TerrainType terrain;
CameraType cam;
FrustumType frustum;
int clientWidth, clientHeight;
float deltaMove = 0.0;
float deltaYaw = 0.0;
float deltaPitch = 0.0;
float deltaRoll = 0.0;  
float deltaMouseYaw = 0.0f;
float deltaMousePitch = 0.0f;
int view = 0;
bool wireframeMode = true;
long startTime = 0L;
long fps = 0;
long countFrames = 0;
float moveSpeed = MOVE_SPEED_START;
bool cullMode = true;
int viewedTriangles;
bool viewCullingMode = true;
bool drawTextMode = true;
int mouseX, mouseY;
int rightButtonState, leftButtonState = GLUT_UP;
float detailValue = DETAIL_START;
GLuint		texture[1];		// land texture for opengl
TGAType landTexture;


///////////////////////////////////////////////
// GLUT CALLBACK FUNCTIONS /////////////////////////////
///////////////////////////////////////////////

void OnMouseClick(int button, int state, int x, int y)
{
  if (button == GLUT_RIGHT_BUTTON)
  {
    rightButtonState = state;
    if (state == GLUT_DOWN)
    {
      //if (rightButtonState != leftButtonState)
        deltaMove = -moveSpeed;
    }
    else
    {
      deltaMove = 0;
    }
  }
  else
  {
    leftButtonState = state;
    if (state == GLUT_DOWN)
    {
      //if (rightButtonState != leftButtonState)
        deltaMove = moveSpeed;
    }
    else
      deltaMove = 0;
  }
}

void OnMousePassiveMove(int x, int y)
{
  //cout << x <<" : "<<y<<endl;
  
 
}

void OnMouseMove(int x, int y)
{
//cout << x <<" : "<<y<<endl;
  if (leftButtonState == GLUT_DOWN || rightButtonState == GLUT_DOWN)
  {
  deltaMouseYaw = (float)-(x - mouseX);
  deltaMousePitch = (float)(y - mouseY);
  if (deltaMousePitch > 20.0f || deltaMousePitch < -20.0f)
    deltaMousePitch = 0.0f;

  mouseY = y;
  mouseX = x;
  }
  
}

void SpecialKeyDown(int key, int x, int y) {

  switch (key) {
      
    case GLUT_KEY_LEFT:
      deltaYaw = moveSpeed;    
      break;
		case GLUT_KEY_RIGHT:
		  deltaYaw = -moveSpeed;      
      break;
		case GLUT_KEY_UP: 
      deltaMove = moveSpeed;
      break;
		case GLUT_KEY_DOWN:
		  deltaMove = -moveSpeed;    
      break;
      case GLUT_KEY_F11:
      detailValue -= 0.5f;
      break;
      
    case GLUT_KEY_F12:
      detailValue += 0.5f;
      break;
    case GLUT_KEY_F2:
      if (drawTextMode)
        drawTextMode = false;
      else drawTextMode = true;
      break;
    case GLUT_KEY_F1:
      if (viewCullingMode)
        viewCullingMode = false;
      else viewCullingMode = true;
      break;
  }      
}

void SpecialKeyUp(int key, int x, int y) 
{
  switch (key)
  {
    case GLUT_KEY_LEFT:
      deltaYaw = 0;    
      break;
		case GLUT_KEY_RIGHT:
		  deltaYaw = 0;      
      break;
		case GLUT_KEY_UP: 
      deltaMove = 0;
      break;
		case GLUT_KEY_DOWN:
		  deltaMove = 0;    
      break;
  }
}

void keyboardDown(unsigned char key, int x, int y)
{
  if (isdigit(key))
  {
    float newSpeed;
    newSpeed = (int)key - 48;
    if (newSpeed > 0 && newSpeed < 10) moveSpeed = newSpeed*2;
  }
  
   switch (key) 
   {
    
      
    case 'a':
      deltaPitch = -ROTATE_SPEED /2; 
      break;
      
    case 'z':
      deltaPitch = ROTATE_SPEED/2;       
      break;
      
     case 's':
      deltaRoll = -ROTATE_SPEED;        
      break;  
     
    case 'd':
      deltaRoll = ROTATE_SPEED;       
      break;
      
    case 'w':
      if (wireframeMode) wireframeMode = false;
      else wireframeMode = true;
      
    case 'v':
      view = (view + 1) % 4;
      break;
    case 'c':
      if (cullMode) cullMode = false;
        else cullMode = true;
      break;
    case 'r':
      terrain.buildDebugOutput = true; 
      break;
   }
}

void keyboardUp(unsigned char key, int x, int y)
{
   switch (key) {
   
    case 'a':
      deltaPitch = 0.0;     
      break;
      
    case 'z':
      deltaPitch = 0.0;      
      break;
      
     case 's':
      deltaRoll = 0.0;
      break;
      
    case 'd':
      deltaRoll = 0.0;
      break;
      
   }
}

void reshape (int w, int h)
{ 		
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClearDepth( 1.0f );
	
  glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glFrustum (-1.0, 1.0, -1.0, 1.0, 1.5, 8000.0);
  glMatrixMode (GL_MODELVIEW);
}


void display(void)
{
  int i;
  long timeNow; // timing temp var
  float * pos = NULL, *look = NULL, *up = NULL;
  long thisFrameStart = glutGet(GLUT_ELAPSED_TIME);  // get the start time of this frame
  if (deltaMouseYaw != 0.0f || deltaMousePitch != 0.0f) 
  {
  
  //  if (deltaMousePitch + cam.pitch > 80 || deltaMousePitch + cam.pitch < 340)
    //  deltaMousePitch = 0.0f;
      
      cam.Rotate(deltaRoll, deltaMousePitch, deltaMouseYaw);
  }
  else
  {
    cam.Rotate(deltaRoll, deltaPitch, deltaYaw);
  }
  
  deltaMouseYaw = deltaMousePitch = 0.0f;
  cam.MoveAlongLookVector(deltaMove);
  

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();
  
   // Draw text if enabled
  glDisable(GL_TEXTURE_2D);	
  if (drawTextMode) DrawText(1.0f, 1.0f, 1.0f);

  if (wireframeMode)
  {
    glDisable(GL_TEXTURE_2D);	
    glEnableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else
  { 
  	glEnable(GL_TEXTURE_2D);	
    glDisableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  if(cullMode)
    glEnable(GL_CULL_FACE);
  else glDisable(GL_CULL_FACE);
  


  // point camera
  pos = cam.GetPos(); 
  look = cam.GetLook();
  up = cam.GetUp();

  gluLookAt( pos[0], pos[1], pos[2],
             look[0],look[1], look[2],
             up[0], up[1], up[2] );
  
  
  glBindTexture(GL_TEXTURE_2D, texture[0]);


  /*
    If view culling is enabled: 
    1.  prepare the frustum for the terrain class:
    2.  call BuildView(), which builds a list of triangle strips based on 
        detailView.
    3.  render each triangle strip
  */
  if (viewCullingMode)
  {  
    frustum.ExtractFrustum();
    terrain.SetView(&frustum, cam.GetPos());
    terrain.BuildView(detailValue);
    viewedTriangles = terrain.viewTriCount;
    for (i = 0; i < terrain.viewTriStripCount; i++)
    {
      glDrawElements(GL_TRIANGLE_STRIP, terrain.triStripList[i].listCount , GL_UNSIGNED_INT, terrain.triStripList[i].stripIndices);
    }
  }  
  else
  {  
    /*
      view culling is not enabled, so just use brute-force
    */
    int curRow = 0;
    viewedTriangles = terrain.numTriangles;
    for (int i = 0; i<terrain.dataHeight -1; i++)
    {
      glDrawElements(GL_TRIANGLE_STRIP, terrain.triStripMeshWidth, GL_UNSIGNED_INT, &terrain.triStripMeshIndices[curRow]);
      
      curRow += terrain.triStripMeshWidth;
    }    
  }
  

  glutSwapBuffers();  // copy back buffer to screen
  
  countFrames++;
  
  // throttle the frame rate for non-crappy systems
  while ((glutGet(GLUT_ELAPSED_TIME) - thisFrameStart) < LOOP_WAIT);
  timeNow = glutGet(GLUT_ELAPSED_TIME);
  if (timeNow - startTime >= 1000)
  {
    fps = countFrames;
    countFrames = 0;
    startTime = timeNow;
    Sleep(3); // compensate for glut's ability to take up 100% cpu
  }  
  Sleep(3);
} // END display()


///////////////////////////////////////////////
// PROGRAM SPECIFIC FUNCTIONS /////////////////////////////
///////////////////////////////////////////////

void DrawText(float r, float g, float b)
{
  // onscreen text
  char str[80];
  float textY = 6.2;
  float textOffset = -.3;
  int i;
  
  sprintf(str, "fps: %i\0", fps);
  glColor3f(r, g, b);
  glRasterPos3f(-6.5, textY, -10);
  for(i=0; i<strlen(str); i++)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]); 


  textY+=textOffset;
  sprintf(str, "Camera:  roll: %f pitch: %f yaw: %f\0", cam.roll, cam.pitch, cam.yaw);
  glRasterPos3f(-6.5, textY, -10);
  for(i=0; i<strlen(str); i++)
         glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]); 
         
  textY+=textOffset;
  sprintf(str, "Total Data Triangles: %i\0", terrain.numTriangles);
  glRasterPos3f(-6.5, textY, -10);
  for(i=0; i<strlen(str); i++)
         glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]); 

  textY+=textOffset;
  sprintf(str, "cullMode: %i\0", cullMode);
  glRasterPos3f(-6.5, textY, -10);
  for(i=0; i<strlen(str); i++)
         glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]); 

  textY+=textOffset;
  sprintf(str, "Level Of Detail: %f  Change with F11/F12\0", detailValue);
  glRasterPos3f(-6.5, textY, -10);
  for(i=0; i<strlen(str); i++)
         glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);                
         
        
  textY+=textOffset;
  sprintf(str, "Viewed triangles %d\0", viewedTriangles);
  glRasterPos3f(-6.5, textY, -10);
  for(i=0; i<strlen(str); i++)
         glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);                    
  
  textY+=textOffset;
  sprintf(str, "viewTriStripCount %d\0", terrain.viewTriStripCount);
  glRasterPos3f(-6.5, textY, -10);
  for(i=0; i<strlen(str); i++)
         glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);               

  textY+=textOffset;
  sprintf(str, "Keys: F1 to toggle view culling, F2 to disable text output\0");
  glRasterPos3f(-6.5, textY, -10);
  for(i=0; i<strlen(str); i++)
         glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);                  
       
 textY+=textOffset;
  sprintf(str, "    'w' for wireframe mode, 'c' to toggle backface culling\0");
  glRasterPos3f(-6.5, textY, -10);
  for(i=0; i<strlen(str); i++)
         glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);     
         
 }


void myinit()
{

	glEnable( GL_DEPTH_TEST );
	glEnable(GL_TEXTURE_2D);	
	//glShadeModel(GL_SMOOTH);
	//glEnable( GL_POLYGON_SMOOTH );
		
	glGenTextures(1, &texture[0]);
  glBindTexture(GL_TEXTURE_2D, texture[0]);
  glTexImage2D(  GL_TEXTURE_2D, 0, GL_RGB, landTexture.GetWidth(),
            landTexture.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, landTexture.buffer);
    
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
	
	reshape(clientWidth, clientHeight);

}


void CleanUp(void)
{
  
}

int main(int argc, char *argv[])
{
	char mapFileName[30], textureFileName[30];
  atexit(CleanUp);  //termination callback
  clientWidth = clientHeight = 600;
  srand(time(NULL));
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
  glutInitWindowSize(clientWidth, clientHeight);
  glutCreateWindow("Terrain Demo");
  
  glutIdleFunc(display);          // allow for realtime rendering
  glutKeyboardFunc(keyboardDown);
  glutReshapeFunc(reshape);
  glutSpecialFunc(SpecialKeyDown);
  glutKeyboardUpFunc(keyboardUp);
  glutSpecialUpFunc(SpecialKeyUp);
  glutMouseFunc(OnMouseClick);
  glutPassiveMotionFunc(OnMousePassiveMove);
  glutMotionFunc(OnMouseMove);

  mouseX = mouseY = clientWidth/ 2;
  // load in heightmap tga file
  // TerrainType will build the vertex/color/texturecoord arrays
	if(argv[1]) 
  {
    strcpy(mapFileName, argv[1]);
  }
  else
    strcpy(mapFileName, "map.tga");
 
  if(argc == 3) 
  {
    strcpy(textureFileName, argv[2]);
  }
  else
    strcpy(textureFileName, "texture.tga");
    
  if (terrain.LoadHeightmap(mapFileName, 
                            TERRAIN_UNITS, 
                            TERRAIN_HEIGHT_SCALE) != TERRAIN_SUCESS)
  {
    cout << "Error Loading heightmap: " << mapFileName<< endl; system("PAUSE");
    return 0;
  }
  
  // load in terrain texture
  if (landTexture.LoadFile(textureFileName) == TGA_SUCCESS)
  {
    cout <<"land texture read!"<<endl;
  }
  else 
  {
    cout << "error loading land texture: "<<textureFileName <<endl;
    system("PAUSE");exit(0);
  }
  
  glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, terrain.vertices);	
  glTexCoordPointer(2, GL_FLOAT, 0, terrain.textureCoords);
  glColorPointer(3, GL_FLOAT, 0, terrain.colors);
 
  myinit();

  cam.SetPos(terrain.width/4, 120.0, terrain.height/4); // middle of terrain
  cam.Rotate(0.0, 0.0, 200.0); // facing forward
  
  startTime = glutGet(GLUT_ELAPSED_TIME); // start the fps counter
  glutDisplayFunc(display);

  glutMainLoop();
  
  return 0;
}
