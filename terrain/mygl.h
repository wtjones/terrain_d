#ifndef MYGL_H
#define MYGL_H


#ifdef __APPLE__
#include <GLUT/glut.h>
#elif _WIN32
#include <windows.h>
#include <glut.h>
#else
#include <GL/glut.h>
#endif

/*typedef struct
{
   short  bpp;
   long   height;
   long   width;
   long   id;
   long   size;
   unsigned char  *buffer;
} TextureType;
*/

// PROTOTYPES
bool myglBindTGA (char *, int texIndex);








#endif
