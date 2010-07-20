/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include <iostream>
#include <cstdlib>
#include "LSMToMegaCapture.h"

int main(int argc, char** argv)
{
  if (argc != 4)
    {
    std::cerr << "lsmtomegacapture requires 3 argument:" << std::endl;
    std::cerr << "1-lsm filename" << std::endl;
    std::cerr << "2-Output directory" << std::endl;
    std::cerr << "3-png or tiff (png by default)" << std::endl;
    return EXIT_FAILURE;
    }

  std::string        type(argv[3]);
  GoFigure::FileType filetype = GoFigure::PNG;

  if (type.compare("tiff") == 0)
    {
    filetype = GoFigure::TIFF;
    }

  LSMToMegaCapture converter;
  converter.SetFileName(argv[1]);
  converter.SetPlaque(0);
  converter.SetRow(0);
  converter.SetColumn(0);
  converter.SetXTile(0);
  converter.SetYTile(0);
  converter.SetZTile(0);
  converter.SetOutputFileType(filetype);
  converter.Export(argv[2]);

  return EXIT_SUCCESS;
}
