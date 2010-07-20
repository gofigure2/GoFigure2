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
#include "MegaCaptureHeaderReader.h"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv)
{
  if (argc != 2)
    {
    std::cerr << "Requires 1 argument: " << std::endl;
    std::cerr << "1-FileName (.meg)" << std::endl;
    return EXIT_FAILURE;
    }

  MegaCaptureHeaderReader reader(argv[1]);
  reader.Read();

  std::cout << "TimeInterval * " << reader.m_TimeInterval << std::endl;
  std::cout << "VoxelSizeX * " << reader.m_VoxelSizeX << std::endl;
  std::cout << "VoxelSizeY * " << reader.m_VoxelSizeY << std::endl;
  std::cout << "VoxelSizeZ * " << reader.m_VoxelSizeZ << std::endl;
  std::cout << "DimensionX * " << reader.m_DimensionX << std::endl;
  std::cout << "DimensionY * " << reader.m_DimensionY << std::endl;
  std::cout << "ChannelDepth * " << reader.m_ChannelDepth << std::endl;
  std::cout << "Number Of Channels * " << reader.m_NumberOfChannels << std::endl;

  for (unsigned int i = 0; i < reader.m_NumberOfChannels; i++)
    {
    std::cout << "Channel Color " << i  << " * [ "
              << reader.m_ChannelColor[i][0]
              << ", " << reader.m_ChannelColor[i][1]
              << ", " << reader.m_ChannelColor[i][2] << " ]" << std::endl;
    }

  return EXIT_SUCCESS;
}
