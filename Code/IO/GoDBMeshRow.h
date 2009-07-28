/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Rev: 323 $  // Revision of last commit
  Date: $Date: 2009-06-18 15:53:05 -0400 (Thu, 18 Jun 2009) $  // Date of last commit
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
#ifndef __GoDBMeshRow_h
#define __GoDBMeshRow_h

#include <string>
#include "ConvertToStringHelper.h"

class GoDBMeshRow
{
public:
  int           meshID;
  int           flavorMeshID;
  int           cellTypeID;
  int           width;
  int           height;
  int           depth;
  int           xCenter;
  int           yCenter;
  int           zCenter;
  int           trackID;
  int           meanBlue;
  int           meanGreen;
  int           meanRed;
  int           score;
  int           surfaceArea;
  int           volume;
  int           experimentID;
  int           RCoord;
  int           CCoord;
  int           TCoord;
  int           YCoord;
  int           XCoord;
  int           ZCoordMin;
  int           ZCoordMax;
  std::string   points;

  std::string PrintValues()
    {
    std::stringstream myString;
    myString << meshID       << ", ";
    myString << flavorMeshID << ", ";
    myString << cellTypeID   << ", ";
    myString << width        << ", ";
    myString << height       << ", ";
    myString << depth        << ", ";
    myString << xCenter      << ", ";
    myString << yCenter      << ", ";
    myString << zCenter      << ", ";
    myString << trackID      << ", ";
    myString << meanBlue     << ", ";
    myString << meanGreen    << ", ";
    myString << meanRed      << ", ";
    myString << score        << ", ";
    myString << surfaceArea  << ", ";
    myString << volume       << ", ";
    myString << experimentID << ", ";
    myString << RCoord       << ", ";
    myString << CCoord       << ", ";
    myString << TCoord       << ", ";
    myString << YCoord       << ", ";
    myString << XCoord       << ", ";
    myString << ZCoordMin    << ", ";
    myString << ZCoordMax    << ", ";
    myString <<"\"" <<points <<"\"";

    return myString.str();
    }

  GoDBMeshRow()
    {
    meshID        = 0;
    flavorMeshID  = 0;
    cellTypeID    = 0;
    width         = 0;
    height        = 0;
    depth         = 0;
    xCenter       = 0;
    yCenter       = 0;
    zCenter       = 0;
    trackID       = 0;
    meanBlue      = 0;
    meanGreen     = 0;
    meanRed       = 0;
    score         = 0;
    surfaceArea   = 0;
    volume        = 0;
    experimentID  = -1;
    RCoord        = -1;
    CCoord        = -1;
    TCoord        = -1;
    YCoord        = -1;
    XCoord        = -1;
    ZCoordMin     = -1;
    ZCoordMax     = -1;
    points        = "\"\"";
    }

  bool SetFieldValueAsString( unsigned int FieldID, std::string ValueAsString )
    {
    switch( FieldID )
      {
      case 0:
        {
        this->meshID = atoi( ValueAsString.c_str() );
        break;
        }
      case 1:
        {
        this->flavorMeshID = atoi( ValueAsString.c_str() );
        break;
        }
      case 2:
        {
        this->cellTypeID = atoi( ValueAsString.c_str() );
        break;
        }
      case 3:
        {
        this->width = atoi( ValueAsString.c_str() );
        break;
        }
      case 4:
        {
        this->height = atoi( ValueAsString.c_str() );
        break;
        }
      case 5:
        {
        this->depth = atoi( ValueAsString.c_str() );
        break;
        }
      case 6:
        {
        this->xCenter = atoi( ValueAsString.c_str() );
        break;
        }
      case 7:
        {
        this->yCenter = atoi( ValueAsString.c_str() );
        break;
        }
      case 8:
        {
        this->zCenter = atoi( ValueAsString.c_str() );
        break;
        }
      case 9:
        {
        this->trackID = atoi( ValueAsString.c_str() );
        break;
        }
      case 10:
        {
        this->meanBlue = atoi( ValueAsString.c_str() );
        break;
        }
      case 11:
        {
        this->meanGreen = atoi( ValueAsString.c_str() );
        break;
        }
      case 12:
        {
        this->meanRed = atoi( ValueAsString.c_str() );
        break;
        }
      case 13:
        {
        this->score = atoi( ValueAsString.c_str() );
        break;
        }
      case 14:
        {
        this->surfaceArea = atoi( ValueAsString.c_str() );
        break;
        }
      case 15:
        {
        this->volume = atoi( ValueAsString.c_str() );
        break;
        }
      case 16:
        {
        this->experimentID = atoi( ValueAsString.c_str() );
        break;
        }
      case 17:
        {
        this->RCoord = atoi( ValueAsString.c_str() );
        break;
        }
      case 18:
        {
        this->CCoord = atoi( ValueAsString.c_str() );
        break;
        }
      case 19:
        {
        this->TCoord = atoi( ValueAsString.c_str() );
        break;
        }
      case 20:
        {
        this->YCoord = atoi( ValueAsString.c_str() );
        break;
        }
      case 21:
        {
        this->XCoord = atoi( ValueAsString.c_str() );
        break;
        }
      case 22:
        {
        this->ZCoordMin = atoi( ValueAsString.c_str() );
        }
      case 23:
        {
        this->ZCoordMax = atoi( ValueAsString.c_str() );
        }
      case 24:
        {
        this->points = ValueAsString;
        }
      default:
        {
        return false;
        }
      }
    return true;
    }

  std::map<std::string,std::string> LinkColumnNamesAndValues ()
    {
    std::map<std::string,std::string> Map;
    Map["meshID"] = ConvertToString<int>(meshID);
    Map["flavorMeshID"] = ConvertToString<int>(flavorMeshID);
    Map["cellTypeID"] = ConvertToString<int>(cellTypeID);
    Map["width"] = ConvertToString<int>(width);
    Map["height"] = ConvertToString<int>(height);
    Map["depth"] = ConvertToString<int>(depth);
    Map["xCenter"] = ConvertToString<int>(xCenter);
    Map["yCenter"] = ConvertToString<int>(yCenter);
    Map["zCenter"] = ConvertToString<int>(zCenter);
    Map["trackID"] = ConvertToString<int>(trackID);
    Map["meanBlue"] = ConvertToString<int>(meanBlue);
    Map["meanGreen"] = ConvertToString<int>(meanGreen);
    Map["meanRed"] = ConvertToString<int>(meanRed);
    Map["score"] = ConvertToString<int>(score);
    Map["surfaceArea"] = ConvertToString<int>(surfaceArea);
    Map["volume"] = ConvertToString<int>(volume);
    Map["experimentID"] = ConvertToString<int>(experimentID);
    Map["RCoord"] = ConvertToString<int>(RCoord);
    Map["CCoord"] = ConvertToString<int>(CCoord);
    Map["TCoord"] = ConvertToString<int>(TCoord);
    Map["YCoord"] = ConvertToString<int>(YCoord);
    Map["XCoord"] = ConvertToString<int>(XCoord);
    Map["ZCoordMin"] = ConvertToString<int>(ZCoordMin);
    Map["ZCoordMax"] = ConvertToString<int>(ZCoordMax);
    Map["points"] = points;

    return Map;
    }
};

#endif
