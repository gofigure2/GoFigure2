#ifndef __MeshTextFileImport_h
#define __MeshTextFileImport_h

#include <string>
#include <list>
#include <vector>

class MeshTextFileImport
{
  public:
    MeshTextFileImport();
    ~MeshTextFileImport();

    void SetFileName( const std::string& );
    void Read();

  private:
    unsigned int m_NumberOfChannels;
    unsigned int m_NumberOfMeshes;
    std::string m_MegaCaptureHeaderFile;
    std::string m_FileName;

    struct InternalMeshStructure
      {
      InternalMeshStructure( const unsigned int& iNumberOfChannels ) :
        m_AverageIntensity( iNumberOfChannels, 0 ) {}

      unsigned int m_TrackId;
      unsigned int m_TCoord;
      double m_Volume;
      unsigned int m_XMin;
      unsigned int m_XMax;
      unsigned int m_YMin;
      unsigned int m_YMax;
      unsigned int m_ZMin;
      unsigned int m_ZMax;
      std::string m_Points;
      std::vector< double > m_AverageIntensity;
      };

    std::list< InternalMeshStructure > m_ListOfMeshes;


};
#endif
