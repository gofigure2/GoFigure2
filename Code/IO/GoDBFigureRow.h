#ifndef __GoDBFigureRow_h__
#define __GoDBFigureRow_h__

#include "vtkPolyData.h"

class GoDBFigureRow
{
  const int figureID;
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
}

#endif
