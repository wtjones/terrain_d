/*  tgatype.h -------- W Travis Jones ----------------------------
    check the .cpp file for info
*/

#ifndef TGATYPE_H
#define TGATYPE_H
#include <iostream>
#include <fstream>

using namespace std;


#define TGA_SUCCESS 0
#define TGA_FILE_ERROR 1
#define TGA_INVALID_BPP 2
#define TGA_INVALID_TYPE 3
#define TGA_MEMORY_ERROR 10

typedef int TGAError;

typedef struct
{
   unsigned short width;
   unsigned short height;
   unsigned char  bpp;
   unsigned char  type;
}  TGAHeaderType;

class TGAType 
{
  private:
   TGAHeaderType header;
   bool isLoaded;
   int errorCode;
  public:
   TGAType();
   ~TGAType();
   int  LoadFile(char *);
   long  GetSize(void);
   long  GetBpp(void);
   long  GetWidth(void);
   long  GetHeight(void);
   long  GetType(void);
   void  Free(void);
   bool  IsLoaded();
   int   LastError(void);
   unsigned char *buffer;  //hold the image data
};

#endif
