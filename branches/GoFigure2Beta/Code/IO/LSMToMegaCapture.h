#ifndef __LSMToMegaCapture_h
#define __LSMToMegaCapture_h

#include <vector>
#include <string>

#include "vtkLSMReader.h"

/**
\brief Convert 1 LSM (5D file) into megacapture files
*/
class LSMToMegaCapture
{
  public:
    LSMToMegaCapture( );
    ~LSMToMegaCapture( );

    void SetFileName( const std::string& iFileName );

    void Export( const std::string& iHeaderFileName );

    void SetPlaque( const unsigned int& iPlaque );
    void SetRow( const unsigned int& iRow );
    void SetColumn( const unsigned int& iCol );
    void SetXTile( const unsigned int& iXt );
    void SetYTile( const unsigned int& iYt );
    void SetZTile( const unsigned int& iZt );

  protected:
    std::string m_FileName;
    std::vector< vtkLSMReader* > m_LSMReaders;
    unsigned int m_Plaque;
    unsigned int m_Row;
    unsigned int m_Column;
    unsigned int m_XTile;
    unsigned int m_YTile;
    unsigned int m_ZTile;
    double m_XOverlap;
    double m_YOverlap;
    double m_ZOverlap;
    unsigned int m_NumberOfChannels;
    unsigned int m_NumberOfTimePoints;

};
#endif
