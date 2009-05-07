#ifndef __GoDBExperimentRow_h__
#define __GoDBExperimentRow_h__

class GoDBExperimentRow
{
public:
  int    experimentID;
  char*  name;
  char*  description;
  char*  timeInterval;
  int    tileHeight;
  int    tileWidth;
  double pixelDepth;
  double pixelHeight;
  double pixelWidth;
  int    colorDepth;
  int    nTimePoints;
  int    nYTiles;
  int    nXTiles;
  int    nSlices;
  int    nRows;
  int    nColumns;
  char*  filePattern;

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
};

std::string PrintValues()
{
  std::stringstream myString;
  myString << experimentID << ", ";
  myString << name         << ", ";
  myString << description  << ", ";
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
  myString << filePattern;
  return myString.str();
};

};

#endif
