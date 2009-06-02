/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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

class GoFigureFileInfoHelper
{
public:
  std::string    Filename;
  unsigned int   TimePoint;
  unsigned int   ZDepth;
  unsigned int   Channel;
  unsigned int   CTile;
  unsigned int   RTile;
  unsigned int   YOffset;
  unsigned int   XOffset;

  bool operator<( const GoFigureFileInfoHelper& other ) const
    {
    if( this->Channel < other.Channel )
      {
      return true;
      }
    if( this->Channel == other.Channel )
      {
      if( this->TimePoint < other.TimePoint )
        {
        return true;
        }
      if( this->TimePoint == other.TimePoint )
        {
        if( this->ZDepth < other.ZDepth )
          {
          return true;
          }
        }
      }
    return false;
    }

  GoFigureFileInfoHelper()
    {
    Filename  = "";
    TimePoint = 0;
    ZDepth    = 0;
    Channel   = 0;
    CTile     = 0;
    RTile     = 0;
    YOffset   = 0;
    XOffset   = 0;
    }
};

typedef std::vector< GoFigureFileInfoHelper > FileListType;

#endif
