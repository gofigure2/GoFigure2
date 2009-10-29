#include "MegaCaptureHeaderReader.h"

#include <cstdlib>
#include <iostream>

int main( int argc, char** argv )
{
  MegaCaptureHeaderReader reader( argv[1] );
  reader.Read();

  std::cout <<"TimeInterval * " <<reader.m_TimeInterval <<std::endl;
  std::cout <<"VoxelSizeX * " <<reader.m_VoxelSizeX <<std::endl;
  std::cout <<"VoxelSizeY * " <<reader.m_VoxelSizeY <<std::endl;
  std::cout <<"VoxelSizeZ * " <<reader.m_VoxelSizeZ <<std::endl;
  std::cout <<"DimensionX * " <<reader.m_DimensionX <<std::endl;
  std::cout <<"DimensionY * " <<reader.m_DimensionY <<std::endl;
  std::cout <<"ChannelDepth * " <<reader.m_ChannelDepth <<std::endl;
  std::cout <<"Number Of Channels * " <<reader.m_NumberOfChannels <<std::endl;

  for( unsigned int i = 0; i < reader.m_NumberOfChannels; i++ )
    {
    std::cout <<"Channel Color " <<i  <<" * [ " <<reader.m_ChannelColor[i][0]
      <<", " <<reader.m_ChannelColor[i][1]
      <<", " <<reader.m_ChannelColor[i][2] <<" ]"<<std::endl;
    }

  return EXIT_SUCCESS;
}
