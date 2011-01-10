/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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
#include "vtkMetaImageReader.h"
#include "vtkImageData.h"

#include <fstream>
#include <sstream>

int main(int argc, char **argv)
{
  if ( argc != 6 )
    {
    std::cerr << "1-mha filename" << std::endl;
    std::cerr << "2-megacapture header (*.meg)" << std::endl;
    std::cerr << "3-Time Interval (in sec)" << std::endl;
    std::cerr << "4-Number of time points" << std::endl;
    std::cerr << "5-Number of channels" << std::endl;
    return EXIT_FAILURE;
    }

  vtkMetaImageReader *reader = vtkMetaImageReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkImageData *image = reader->GetOutput();

  double spacing[3];
  image->GetSpacing(spacing);

  int dim[3];
  image->GetDimensions(dim);

  int extent[6];
  image->GetExtent(extent);

  double iTimeInterval = atof(argv[3]);
  int    iNumberOfTimePoints = atoi(argv[4]);
  int    iNumberOfChannels = atoi(argv[5]);

  std::ofstream file(argv[2]);
  file << "MegaCapture" << std::endl;
  file << "<ImageSessionData>" << std::endl;
  file << "Version 3.0" << std::endl;
  file << "ExperimentTitle " << std::endl;
  file << "ExperimentDescription " << std::endl;
  file << "TimeInterval " << iTimeInterval << std::endl;
  file << "Objective " << std::endl;
  file << "VoxelSizeX " << spacing[0] << std::endl;
  file << "VoxelSizeY " << spacing[1] << std::endl;
  file << "VoxelSizeZ " << spacing[2] << std::endl;
  file << "DimensionX " << dim[0] << std::endl;
  file << "DimensionY " << dim[1] << std::endl;
  file << "DimensionPL 1" << std::endl;
  file << "DimensionCO 1" << std::endl;
  file << "DimensionRO 1" << std::endl;
  file << "DimensionZT 1" << std::endl;
  file << "DimensionYT 1" << std::endl;
  file << "DimensionXT 1" << std::endl;
  file << "DimensionTM " << iNumberOfTimePoints << std::endl;
  file << "DimensionZS " << dim[2] << std::endl;

  if ( iNumberOfChannels == 1 )
    {
    file << "DimensionCH 1" << std::endl;
    int blue = 255;
    file << "ChannelColor00 " << blue << std::endl;
    }
  else
    {
    file << "DimensionCH 3" << std::endl;
    int red = 255;
    int green = 0;
    int blue = 0;
    file << "ChannelColor00 " << red * 256 * 256 + green * 256 + blue << std::endl;
    red = 0;
    green = 255;
    blue = 0;
    file << "ChannelColor01 " << red * 256 * 256 + green * 256 + blue << std::endl;
    red = 0;
    green = 0;
    blue = 255;
    file << "ChannelColor02 " << red * 256 * 256 + green * 256 + blue << std::endl;
    }

  file << "ChannelDepth 8" << std::endl;
  file << "FileType PNG" << std::endl;
  file << "</ImageSessionData>" << std::endl;

  file.close();

  reader->Delete();

  return EXIT_SUCCESS;
}
