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
#ifndef __GoDBSeriesGridRow_h
#define __GoDBSeriesGridRow_h

#include <string>

class GoDBSeriesGridRow
{
public:

  int         imageID;
  int         experimentID;
  int         RCoord;
  int         CCoord;
  int         TCoord;
  int         YCoord;
  int         XCoord;
  int         ZCoord;
  std::string filename;

  std::string PrintValues()
    {
    std::stringstream myString;

    myString << imageID << ", ";
    myString << experimentID << ", ";
    myString << RCoord << ", ";
    myString << CCoord << ", ";
    myString << TCoord << ", ";
    myString << YCoord << ", ";
    myString << XCoord << ", ";
    myString << ZCoord << ", ";
    myString << "\"" << filename << "\"";

    return myString.str();
    }

  GoDBSeriesGridRow()
    {
    imageID = 0;
    experimentID = -1;
    RCoord = -1;
    CCoord = -1;
    TCoord = -1;
    YCoord = -1;
    XCoord = -1;
    ZCoord = -1;
    filename = "";
    }

    bool SetFieldValueAsString( unsigned int FieldID, std::string ValueAsString )
    {
      switch( FieldID )
        {
        case 0:
          {
          this->imageID = atoi( ValueAsString.c_str() );
          break;
          }
        case 1:
          {
          this->experimentID = atoi( ValueAsString.c_str() );
          break;
          }
        case 2:
          {
          this->RCoord = atoi( ValueAsString.c_str() );
          break;
          }
        case 3:
          {
          this->CCoord = atoi( ValueAsString.c_str() );
          break;
          }
        case 4:
          {
          this->TCoord = atoi( ValueAsString.c_str() );
          break;
          }
        case 5:
          {
          this->YCoord = atoi( ValueAsString.c_str() );
          break;
          }
        case 6:
          {
          this->XCoord = atoi( ValueAsString.c_str() );
          break;
          }
        case 7:
          {
          this->ZCoord = atoi( ValueAsString.c_str() );
          break;
          }
        case 8:
          {
          this->filename = ValueAsString;
          }
        default:
          {
          return false;
          }
        }
      return true;
    }

};

#endif
