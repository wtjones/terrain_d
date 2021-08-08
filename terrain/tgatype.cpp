/* tgatype.cpp -------- 2003 by W Travis Jones ----------------------------
   LoadTGA() Takes a tga file name and loads the attribs and
   image data into a buffer(allocated here), and then swaps to RGB, then
   returns true if sucessfull.

   NOTE: Once a file is loaded, the image data stays allocated untill Free()
   is called. The destructor calls Free() as well.
   4/17/01 -  First version, uses cout for feedback.
   4/19/01 -  Working for 24-bit RGB images
   3/3/03 -   Loads 8-bit greyscale images (type 3)
              Added error codes, check the .h file

  Known supported formats:
   type 2 uncompressed true-color 24-bit
   type 3 uncompressed black and white 8-bit
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include "tgatype.h"


TGAType::TGAType()
{
   buffer = NULL;
   memset(&header, 0, sizeof(header));
   isLoaded = false;
   errorCode = 0; // no error
}

TGAType::~TGAType()
{
   Free();
}

long TGAType::GetWidth(void)
{
// returns the width image buffer in pixels
   return header.width;
}

long TGAType::GetHeight()
{
// returns the width image buffer in pixels

   return header.height;
}

long TGAType::GetBpp(void)
{
   // returns the bpp of the image data in bytes per pixel
   // Divide the result by 8 to get bytes per pixel (duh)
   return header.bpp;
}

int TGAType::LastError(void)
{
  return errorCode;
}

void TGAType::Free(void)
{
   if (buffer) delete buffer;
   buffer = NULL;
   isLoaded = false;
}

long TGAType::GetSize(void)
// returns the size of the image(not the file) in bytes
{
   return (header.bpp/8) * header.height * header.width;
}

long TGAType::GetType(void)
{
  return header.type;
}

bool TGAType::IsLoaded()
{
   return isLoaded;
}

int TGAType::LoadFile(char *fileName)
{
  long i;
  char temp[18];
  unsigned char tempByte;
  fstream tgaFile;
  memset(&header, 0, sizeof(header));
  memset(temp, 0, sizeof(temp));

  tgaFile.open(fileName, ios::in | ios::binary);
  if (!tgaFile)
  {
    #ifdef CONSOLE_OUTPUT
    cout << "Can't open " << fileName << endl;
    #endif
    return errorCode = TGA_FILE_ERROR;
  }
  tgaFile.read((char *) &temp, sizeof(temp));

   //extract the parts we need for the header
   memcpy(&header.type, &temp[2], 1);
   memcpy(&header.width, &temp[12], 2);
   memcpy(&header.height, &temp[14], 2);
   memcpy(&header.bpp, &temp[16], 1);

  if (header.bpp != 24  && header.bpp != 8)
  {
    #ifdef CONSOLE_OUTPUT
    cout << "Not a valid bit depth: " <<  header.bpp << endl;
    #endif
    return errorCode = TGA_INVALID_BPP;
  }

  // type 2 is uncompressed true-color
  // type 3 is uncompress black and white
  if (header.type != 2 && header.type != 3)
  {
    #ifdef CONSOLE_OUTPUT
    cout << "Not a valid image type: "<< temp[2] <<endl;
    #endif
    return errorCode = TGA_INVALID_TYPE;
  }

  // *******allocate the buffer*********
  buffer = new unsigned char[GetSize()];
  if (!buffer) return errorCode = TGA_MEMORY_ERROR;

   memset(buffer, 0, GetSize()); //clear the buffer

   tgaFile.read((char *)buffer, GetSize());
   tgaFile.close();

  #ifdef CONSOLE_OUTPUT
   cout << "TGA Filename: " << fileName << endl;
   cout << "width: " << header.width << "  height: " << header.height
      << "  bpp:  " << (short)header.bpp<< endl;
   cout << "THE SIZE:  " << GetSize() << endl;
  #endif

  if (header.bpp == 24)
  {
    // Change from TGA's blue-green-red to red-green-blue
    for (i = 0; i < GetSize(); i += (header.bpp/8))
    {
      tempByte = buffer[i];
      buffer[i] = buffer[i+2];
      buffer[i+2] = tempByte;
    }
  }

   isLoaded = true;
   return 0;
}
