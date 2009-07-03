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
#ifndef __GoDBFigureRow_h
#define __GoDBFigureRow_h

#include <string>
#include <map>
#include <iostream>
#include <sstream>

class GoDBFigureRow
{
public:
  int           figureID;
  int           imageID;
  int           meshID;
  std::string   points;
  int           cellTypeID;
  int           flavorID;
  int           meanRed;
  int           meanGreen;
  int           meanBlue;
  int           area;
  int           perimeter;
  int           AP;
  int           score;
  int           xCenter;
  int           yCenter;
  int           experimentID;
  int           RCoord;
  int           CCoord;
  int           TCoord;
  int           YCoord;
  int           XCoord;
  int           ZCoord;



  std::string PrintValues()
    {
    std::stringstream myString;
    myString << figureID     << ", ";
    myString << imageID      << ", ";
    myString << meshID       << ", ";
    myString << points       << ", ";
    myString << cellTypeID   << ", ";
    myString << flavorID     << ", ";
    myString << meanRed      << ", ";
    myString << meanGreen    << ", ";
    myString << meanBlue     << ", ";
    myString << area         << ", ";
    myString << perimeter    << ", ";
    myString << AP           << ", ";
    myString << score        << ", ";
    myString << xCenter      << ", ";
    myString << yCenter      << ", ";
    myString << experimentID << ", ";
    myString << RCoord       << ", ";
    myString << CCoord       << ", ";
    myString << TCoord       << ", ";
    myString << YCoord       << ", ";
    myString << XCoord       << ", ";
    myString << ZCoord;

    return myString.str();
    }

  GoDBFigureRow()
    {
    figureID = 0;
    imageID  = 0;
    meshID   = 0;
    points   = "\"\"";
    cellTypeID = 0;
    flavorID   = 0;
    meanRed    = 0;
    meanGreen  = 0;
    meanBlue   = 0;
    area       = 0;
    perimeter  = 0;
    AP         = 0;
    score      = 0;
    xCenter    = 0;
    yCenter    = 0;
    experimentID = -1;
    RCoord = -1;
    CCoord = -1;
    TCoord = -1;
    YCoord = -1;
    XCoord = -1;
    ZCoord = -1;
    }

  bool SetFieldValueAsString( unsigned int FieldID, std::string ValueAsString )
    {
    switch( FieldID )
      {
      case 0:
        {
        this->figureID = atoi( ValueAsString.c_str() );
        break;
        }
      case 1:
        {
        this->imageID = atoi( ValueAsString.c_str() );
        break;
        }
      case 2:
        {
        this->meshID = atoi( ValueAsString.c_str() );
        break;
        }
      case 3:
        {
        this->points = ValueAsString;
        break;
        }
      case 4:
        {
        this->cellTypeID = atoi( ValueAsString.c_str() );
        break;
        }
      case 5:
        {
        this->flavorID = atoi( ValueAsString.c_str() );
        break;
        }
      case 6:
        {
        this->meanRed = atoi( ValueAsString.c_str() );
        break;
        }
      case 7:
        {
        this->meanGreen = atoi( ValueAsString.c_str() );
        break;
        }
      case 8:
        {
        this->meanBlue = atoi( ValueAsString.c_str() );
        break;
        }
      case 9:
        {
        this->area = atoi( ValueAsString.c_str() );
        break;
        }
      case 10:
        {
        this->perimeter = atoi( ValueAsString.c_str() );
        break;
        }
      case 11:
        {
        this->AP = atoi( ValueAsString.c_str() );
        break;
        }
      case 12:
        {
        this->score = atoi( ValueAsString.c_str() );
        break;
        }
      case 13:
        {
        this->xCenter = atoi( ValueAsString.c_str() );
        break;
        }
      case 14:
        {
        this->yCenter = atoi( ValueAsString.c_str() );
        break;
        }
      case 15:
        {
        this->experimentID = atoi( ValueAsString.c_str() );
        break;
        }
      case 16:
        {
        this->RCoord = atoi( ValueAsString.c_str() );
        break;
        }
      case 17:
        {
        this->CCoord = atoi( ValueAsString.c_str() );
        break;
        }
      case 18:
        {
        this->TCoord = atoi( ValueAsString.c_str() );
        break;
        }
      case 19:
        {
        this->YCoord = atoi( ValueAsString.c_str() );
        break;
        }
      case 20:
        {
        this->XCoord = atoi( ValueAsString.c_str() );
        break;
        }
      case 21:
        {
        this->ZCoord = atoi( ValueAsString.c_str() );
        break;
        }
      default:
        {
        return false;
        }
      }
    return true;
    }

 std::string Convert(int ToConvert)
 {
  std::stringstream st;
  st << ToConvert;
  std::string mystring = st.str();
  return mystring;
 }

 std::map<std::string,std::string> LinkColumnNamesAndValues ()
 {
   std::map<std::string,std::string> Map;
   Map["figureID"] = Convert(figureID);
   Map["imageID"] = Convert(imageID);
   Map["meshID"] = Convert(meshID);
   Map["points"] = points;
   Map["cellTypeID"] = Convert(cellTypeID);
   Map["flavorID"] = Convert(flavorID);
   Map["meanRed"] = Convert(meanRed);
   Map["meanGreen"] = Convert(meanGreen);
   Map["meanBlue"] = Convert(meanBlue);
   Map["area"] = Convert(area);
   Map["perimeter"] = Convert(perimeter);
   Map["AP"] = Convert(AP);
   Map["score"] = Convert(score);
   Map["xCenter"] = Convert(xCenter);
   Map["yCenter"] = Convert(yCenter);
   Map["experimentID"] = Convert(experimentID);
   Map["RCoord"] = Convert(RCoord);
   Map["CCoord"] = Convert(CCoord);
   Map["TCoord"] = Convert(TCoord);
   Map["XCoord"] = Convert(XCoord);
   Map["YCoord"] = Convert(YCoord);
   Map["ZCoord"] = Convert(ZCoord); 

   return Map;
 }
 };

#endif
