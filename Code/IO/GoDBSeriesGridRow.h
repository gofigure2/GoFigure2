#ifndef _GoDBSeriesGridRow_h__
#define _GoDBSeriesGridRow_h__

class GoDBSeriesGridRow
{
public:

int   imageID;
int   experimentID;
int   RCoord;
int   CCoord;
int   TCoord;
int   YCoord;
int   XCoord;
int   ZCoord;
char* filename;

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
};

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
};

};

#endif

