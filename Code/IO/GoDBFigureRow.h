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
    points   = 0;
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

};

#endif
