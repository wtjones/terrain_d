#include <GL/glut.h>
#include "mygl.h"
#include "tgatype.h"

extern GLuint texID[];

bool myglBindTGA (char * fileName, int texIndex)
{
   TGAType tga; int i;
   if (!tga.LoadFile(fileName)) return false;

   // disabled due to error regardin extern on newer compiler
   //glBindTexture(GL_TEXTURE_2D, texID[texIndex]);
   //glPixelStorei (GL_UNPACK_ALIGNMENT, texID[texIndex]);

   //Dont seem to need this:
      //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


   glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glTexImage2D(  GL_TEXTURE_2D, 0, GL_RGB, tga.GetWidth(),
            tga.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, tga.buffer);
   //Equivalent code to the above call:
      //   gluBuild2DMipmaps(  GL_TEXTURE_2D, GL_RGB, tgaFile.GetWidth(),
      //           tgaFile.GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, buffer);

   return true;
}



