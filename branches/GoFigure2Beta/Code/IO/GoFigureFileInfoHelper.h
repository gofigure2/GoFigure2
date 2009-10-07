/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Rev: 536 $  // Revision of last commit
  Date: $Date: 2009-08-06 13:33:19 -0400 (Thu, 06 Aug 2009) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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

#ifndef __GoFigureFileInfoHelper_h
#define __GoFigureFileInfoHelper_h

#include <string>

struct GoFigureFileInfoHelper
{
  std::string     m_Filename;
  unsigned int    m_TCoord;
  unsigned int    m_ZCoord;
  unsigned int    m_Channel;
  unsigned int    m_CTile;
  unsigned int    m_RTile;
  unsigned int    m_YCoord;
  unsigned int    m_XCoord;

  GoFigureFileInfoHelper() : m_Filename( "" ), m_TCoord( 0 ),
    m_ZCoord( 0 ), m_Channel( 0 ), m_CTile( 0 ), m_RTile( 0 ),
    m_YCoord( 0 ), m_XCoord( 0 )
    {}

  ~GoFigureFileInfoHelper()
    {}
};

typedef std::vector< GoFigureFileInfoHelper > FileListType;

struct GoFigureFileInfoHelperTimeBasedCompare
{
  bool operator() ( const GoFigureFileInfoHelper& iA, const GoFigureFileInfoHelper& other )
    {
    if( iA.m_Channel < other.m_Channel )
      {
      return true;
      }
    if( iA.m_Channel == other.m_Channel )
      {
        if( iA.m_TCoord < other.m_TCoord )
          {
          return true;
          }
        if( iA.m_TCoord == other.m_TCoord )
          {
          if( iA.m_ZCoord < other.m_ZCoord )
            {
            return true;
            }
          }
      }
    return false;
    }
};


struct GoFigureFileInfoHelperZCoordBasedCompare
{
  bool operator() ( const GoFigureFileInfoHelper& iA, const GoFigureFileInfoHelper& other )
    {
    if( iA.m_Channel < other.m_Channel )
      {
      return true;
      }
    if( iA.m_Channel == other.m_Channel )
      {
        if( iA.m_ZCoord < other.m_ZCoord )
          {
          return true;
          }
        if( iA.m_ZCoord == other.m_ZCoord )
          {
          if( iA.m_TCoord < other.m_TCoord )
            {
            return true;
            }
          }
      }
    return false;
    }
};


#endif
