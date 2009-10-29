#include "MegaCaptureHeaderReader.h"

#include <iostream>
#include <fstream>

MegaCaptureHeaderReader::
MegaCaptureHeaderReader( std::string iFileName ) :
  m_FileName( iFileName )
{}

MegaCaptureHeaderReader::
~MegaCaptureHeaderReader()
{
}

void MegaCaptureHeaderReader::
SetFileName( const std::string& iFileName )
{
  if( iFileName.empty() )
    {
    std::cerr <<"empty filename" <<std::endl;
    return;
    }
  else
    {
    m_FileName = iFileName;
    }
}


void MegaCaptureHeaderReader::
Read()
{
  std::string line;
  std::ifstream ifs( m_FileName.c_str(), std::ifstream::in );
  if( ifs.is_open() )
    {
    // Skip the 5 first lines, i.e.
    // MegaCapture
    // <ImageSessionData>
    // Version 3.0
    // ExperimentTitle
    // ExperimentDescription
    for( int i = 0; i < 5; i++ )
      {
      getline( ifs, line );
//       std::cout <<i <<" * "<<line <<std::endl;
      }

    std::string word;
    ifs >> word >> m_TimeInterval; //>>endofline;
//     std::cout <<word <<" "<<m_TimeInterval <<std::endl;
    getline( ifs, line ); // to get '\n'
    getline( ifs, line ); // Objective Plan-Apochromat 20x/0.8 M27

    ifs >> word >> m_VoxelSizeX;
    ifs >> word >> m_VoxelSizeY;
    ifs >> word >> m_VoxelSizeZ;
    ifs >> word >> m_DimensionX;
    ifs >> word >> m_DimensionY;

  // DimensionPL 1
  // DimensionCO 2
  // DimensionRO 2
  // DimensionZT 1
  // DimensionYT 2
  // DimensionXT 2
  // DimensionTM 2
  // DimensionZS 4

    for( int i = 0; i < 9; i++ )
      {
      getline( ifs, line );
      }

    ifs >> word  >> m_NumberOfChannels;

    m_ChannelColor.resize( m_NumberOfChannels );
    unsigned long color;
    for( unsigned int i = 0; i < m_NumberOfChannels; i++ )
      {
      ifs >>word >>color;
      m_ChannelColor[i] = ConvertUnsignedLongColorToRGBIntColor( color );
      }
    ifs >>word >>m_ChannelDepth;

  // FileType  TIF
  // </ImageSessionData>
  // <Image>
  // Filename  D:/megacapture/image-PL00-CO00-RO00-ZT00-YT00-XT00-TM0000-ch00-zs0000.png
    for( int i = 0; i < 5; i++ )
      {
      getline( ifs, line );
      }
    std::string date;
    ifs >>word >>date;
    std::cout <<"** " <<date <<std::endl;
    }
  else
    {
    std::cerr <<"Unable to open file" <<std::endl;
    return;
    }
}

std::vector< int > MegaCaptureHeaderReader::
ConvertUnsignedLongColorToRGBIntColor( const unsigned long& iColor )
{
  std::vector< int > oRGB( 3 );
  oRGB[0] = static_cast< int >( ( iColor / ( 256 * 256 ) ) % 256 );
  oRGB[1] = static_cast< int >( ( iColor / 256 ) % 256 );
  oRGB[2] = static_cast< int >( iColor % 256 );
  return oRGB;
}

// std::string MegaCaptureHeaderReader::
// ConvertDayStringFormat( std::string iDate )
// {
//   std::string oDate;
//   oDate = iDate.substr( );
// }
