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
#ifndef __GoDBExperimentRow_h
#define __GoDBExperimentRow_h

#include <string>

class GoDBExperimentRow
{
public:
  int           experimentID;
  std::string   name;
  std::string   description;
  int           timeInterval;
  int           tileHeight;
  int           tileWidth;
  double        pixelDepth;
  double        pixelHeight;
  double        pixelWidth;
  int           colorDepth;
  int           nTimePoints;
  int           nYTiles;
  int           nXTiles;
  int           nSlices;
  int           nRows;
  int           nColumns;
  std::string   filePattern;

  GoDBExperimentRow()
    {
    experimentID = 0;
    name         = "";
    description  = "";
    timeInterval = 0;
    tileHeight   = 1024;
    tileWidth    = 1024;
    pixelDepth   = 0;
    pixelHeight  = 0;
    pixelWidth   = 0;
    colorDepth   = 8;
    nTimePoints  = -1;
    nYTiles      = -1;
    nXTiles      = -1;
    nSlices      = -1;
    nRows        = -1;
    nColumns     = -1;
    filePattern  = "";
    }

  std::string PrintValues()
    {
    std::stringstream myString;

    myString << experimentID << ", ";
    myString << "\"" << name         << "\"" << ", ";
    myString << "\"" << description  << "\"" << ", ";
    myString << timeInterval << ", ";
    myString << tileHeight   << ", ";
    myString << tileWidth    << ", ";
    myString << pixelDepth   << ", ";
    myString << pixelHeight  << ", ";
    myString << pixelWidth   << ", ";
    myString << colorDepth   << ", ";
    myString << nTimePoints  << ", ";
    myString << nYTiles      << ", ";
    myString << nXTiles      << ", ";
    myString << nSlices      << ", ";
    myString << nRows        << ", ";
    myString << nColumns     << ", ";
    myString << "\"" << filePattern << "\"";

    return myString.str();
    }

  bool SetFieldValueAsString( unsigned int FieldID, std::string ValueAsString )
    {
    switch( FieldID )
      {
      case 0:
        {
        this->experimentID = atoi( ValueAsString.c_str() );
        break;
        }
      case 1:
        {
        this->name = ValueAsString;
        break;
        }
      case 2:
        {
        this->description = ValueAsString;
        break;
        }
      case 3:
        {
        this->timeInterval = atoi( ValueAsString.c_str() );
        break;
        }
      case 4:
        {
        this->tileHeight = atoi( ValueAsString.c_str() );
        break;
        }
      case 5:
        {
        this->tileWidth= atoi( ValueAsString.c_str() );
        break;
        }
      case 6:
        {
        this->pixelDepth = atof( ValueAsString.c_str() );
        break;
        }
      case 7:
        {
        this->pixelHeight = atof( ValueAsString.c_str() );
        break;
        }
      case 8:
        {
        this->pixelWidth = atof( ValueAsString.c_str() );
        break;
        }
      case 9:
        {
        this->colorDepth= atoi( ValueAsString.c_str() );
        break;
        }
      case 10:
        {
        this->nTimePoints= atoi( ValueAsString.c_str() );
        break;
        }
      case 11:
        {
        this->nYTiles= atoi( ValueAsString.c_str() );
        break;
        }
      case 12:
        {
        this->nXTiles= atoi( ValueAsString.c_str() );
        break;
        }
      case 13:
        {
        this->nSlices= atoi( ValueAsString.c_str() );
        break;
        }
      case 14:
        {
        this->nRows= atoi( ValueAsString.c_str() );
        break;
        }
      case 15:
        {
        this->nColumns= atoi( ValueAsString.c_str() );
        break;
        }
      case 16:
        {
        this->filePattern= ValueAsString;
        break;
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
