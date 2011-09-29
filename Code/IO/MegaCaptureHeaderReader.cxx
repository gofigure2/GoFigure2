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

#include "MegaCaptureHeaderReader.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

/*****************************************************************************/
MegaCaptureHeaderReader::MegaCaptureHeaderReader(const std::string & iFileName) :
  m_FileName(iFileName)
{
  m_TimeInterval = 0.;
  m_VoxelSizeX = 0.;
  m_VoxelSizeY = 0.;
  m_VoxelSizeZ = 0.;
  m_DimensionX = 0;
  m_DimensionY = 0;
  m_NumberOfChannels = 0;
  m_ChannelDepth = 0;
  m_ChannelColor.resize(0);
}
/*****************************************************************************/

/*****************************************************************************/
MegaCaptureHeaderReader::
~MegaCaptureHeaderReader()
{
}
/*****************************************************************************/

/*****************************************************************************/
void MegaCaptureHeaderReader::SetFileName(const std::string & iFileName)
{
  if ( iFileName.empty() )
    {
    std::cerr << "empty filename" << std::endl;
    return;
    }
  else
    {
    m_FileName = iFileName;
    }
}
/*****************************************************************************/

/*****************************************************************************/
void MegaCaptureHeaderReader::Read()
{
  std::string   line;
  std::ifstream ifs(m_FileName.c_str(), std::ifstream::in);

  if ( ifs.is_open() )
    {
    /**
      * \todo Nicolas- shoudln't we read it too
      */
    // Skip the 5 first lines, i.e.
    // MegaCapture
    // <ImageSessionData>
    // Version 3.0
    // ExperimentTitle
    // ExperimentDescription
    int lineNumber(0);
    for ( int i = 0; i < 5; i++ )
      {
      getline(ifs, line);
      ++lineNumber;
      }

    std::string word;
    ifs >> word >> m_TimeInterval;
    CheckKeyWord(word, "TimeInterval", lineNumber);

    /**
      * \todo Nicolas- shoudln't we read Objective too
      */
    getline(ifs, line);   // to get '\n'
    getline(ifs, line);   // Objective Plan-Apochromat 20x/0.8 M27
    ++lineNumber;

    ifs >> word >> m_VoxelSizeX;
    CheckKeyWord(word, "VoxelSizeX", lineNumber);
    ifs >> word >> m_VoxelSizeY;
    CheckKeyWord(word, "VoxelSizeY", lineNumber);
    ifs >> word >> m_VoxelSizeZ;
    CheckKeyWord(word, "VoxelSizeZ", lineNumber);
    ifs >> word >> m_DimensionX;
    CheckKeyWord(word, "DimensionX", lineNumber);
    ifs >> word >> m_DimensionY;
    CheckKeyWord(word, "DimensionY", lineNumber);

    /**
      * \todo Nicolas- shoudln't we read it too
      */
    // DimensionPL 1
    // DimensionCO 2
    // DimensionRO 2
    // DimensionZT 1
    // DimensionYT 2
    // DimensionXT 2
    // DimensionTM 2
    // DimensionZS 4

    getline(ifs, line); // to get '\n'
    for ( int i = 0; i < 8; i++ )
      {
      getline(ifs, line);
      ++lineNumber;
      }

    ifs >> word  >> m_NumberOfChannels;
    CheckKeyWord(word, "DimensionCH", lineNumber);

    m_ChannelColor.resize(m_NumberOfChannels);
    unsigned long color;
    for ( unsigned int i = 0; i < m_NumberOfChannels; i++ )
      {
      ifs >> word >> color;
      std::ostringstream channelColor1;
      channelColor1 << "ChannelColor" << std::setw(2) << std::setfill('0') << i;
      std::ostringstream channelColor2;
      channelColor2 << "ChannelColor" << i;
      CheckKeyWord(word, channelColor1.str(), lineNumber, channelColor2.str());
      m_ChannelColor[i] = ConvertUnsignedLongColorToRGBIntColor(color);
      }
    ifs >> word >> m_ChannelDepth;
    CheckKeyWord(word, "ChannelDepth", lineNumber);

    /**
      * \todo Nicolas- Reading date from here might not be a good solution
      */
    // FileType  TIF
    // </ImageSessionData>
    // <Image>
    // Filename
    //
    // D:/megacapture/image-PL00-CO00-RO00-ZT00-YT00-XT00-TM0000-ch00-zs0000.png
    for ( int i = 0; i < 5; i++ )
      {
      getline(ifs, line);
      }
    std::string date, hours;
    ifs >> word >> date >> hours;
    m_CreationDate = date;
    m_CreationDate += " ";
    m_CreationDate += hours;

    //std::cout << "** " << m_CreationDate << std::endl;
    }
  else
    {
    std::cerr << "Unable to open file" << std::endl;
    }
}

std::vector< int > MegaCaptureHeaderReader::ConvertUnsignedLongColorToRGBIntColor(const unsigned long & iColor)
{
  std::vector< int > oRGB(3);
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
/*****************************************************************************/

/*****************************************************************************/
bool
MegaCaptureHeaderReader::
CheckKeyWord(std::string iWord, std::string iCompare, int& iLineNumber,
             std::string iExtraKeyWord)
{
  ++iLineNumber;
  if( (iWord.compare(iCompare) != 0) && (iWord.compare(iExtraKeyWord) != 0) )
    {
    std::cerr << ">> ERROR: *" << iCompare << "* keyword should on the line *"
              << iLineNumber << "* of your .meg file"<< std::endl;
    throw std::string("Corrupted header file");

    return false;
    }
  return true;
}
