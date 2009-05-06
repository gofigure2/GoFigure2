#ifndef __GoDBFigureRow_h__
#define __GoDBFigureRow_h__

#include "vtkPolyData.h"

class GoDBFigureRow
{
public:
  int figureID;
  int imageID;
  int meshID;
  vtkPolyData* points;
  int cellTypeID;
  int flavorID;
  int meanRed;
  int meanGreen;
  int meanBlue;
  int area;
  int perimeter;
  int AP;
  int score;
  int xCenter;
  int yCenter;
  int experimentID;
  int RCoord;
  int CCoord;
  int TCoord;
  int YCoord;
  int XCoord;
  int ZCoord;

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
};

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
  experimentID = 0;
  RCoord = 0;
  CCoord = 0;
  TCoord = 0;
  YCoord = 0;
  XCoord = 0;
  ZCoord = 0;
} 
  
};

#endif
