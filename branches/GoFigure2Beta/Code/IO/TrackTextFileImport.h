#ifndef __TrackTextFileImport_h
#define __TrackTextFileImport_h

#include <string>
#include <list>
#include <vector>

class vtkPolyData;
class vtkMySQLDatabase;

class TrackTextFileImport
{
  public:
		TrackTextFileImport( const std::string& iServerName, const std::string& iLogin,
      const std::string& iPassword, const std::string& iDBName,
      const unsigned int& iImagingSessionId );

    ~TrackTextFileImport();

    void SetDirectory( const std::string& );
    void SetFileName( const std::string& );
    void Read();

  private:
    unsigned int m_ImagingSessionId;
    unsigned int m_NumberOfChannels;
    unsigned int m_NumberOfMeshes;
    std::string m_MegaCaptureHeaderFile;
    std::string m_Directory;
    std::string m_FileName;

    vtkMySQLDatabase* m_DBConnector;

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
      vtkPolyData* m_Points;
      std::vector< double > m_AverageIntensity;
      };

    std::list< InternalMeshStructure > m_ListOfMeshes;

    void SaveMeshInDataBase( const InternalMeshStructure& iMesh );


};
#endif
