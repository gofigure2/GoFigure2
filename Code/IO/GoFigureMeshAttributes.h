#ifndef __GoFigureMeshAttributes_h
#define __GoFigureMeshAttributes_h

#include <map>
#include <string>

struct QGOIO_EXPORT GoFigureMeshAttributes
{
  std::map< std::string, int >    m_TotalIntensityMap;
  std::map< std::string, double > m_MeanIntensityMap;
  double                          m_Volume;
  double                          m_Area;
  int                             m_Size;
};
#endif
