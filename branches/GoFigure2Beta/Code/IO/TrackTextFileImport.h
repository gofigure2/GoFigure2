#ifndef __TrackTextFileImport_h
#define __TrackTextFileImport_h

#include <string>
#include <list>
#include <vector>

#include "vtkType.h"

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

    /**
     * \brief Read the TrackTex file
     */
    void Read();

  private:
    unsigned int m_ImagingSessionId;
    unsigned int m_NumberOfChannels;
    unsigned int m_NumberOfTracks;
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

    struct InternalTrackStructure
      {
      InternalTrackStructure() :
          m_XMin(VTK_UNSIGNED_INT_MAX),m_XMax(VTK_UNSIGNED_INT_MIN),
      		m_YMin(VTK_UNSIGNED_INT_MAX), m_YMax(VTK_UNSIGNED_INT_MIN),
      		m_ZMin(VTK_UNSIGNED_INT_MAX), m_ZMax(VTK_UNSIGNED_INT_MIN),
      		m_TMin(VTK_UNSIGNED_INT_MAX), m_TMax(VTK_UNSIGNED_INT_MIN){}

      unsigned int m_LineageID;
      unsigned int m_TrackId;
      unsigned int m_XMin;
      unsigned int m_XMax;
      unsigned int m_YMin;
      unsigned int m_YMax;
      unsigned int m_ZMin;
      unsigned int m_ZMax;
      unsigned int m_TMin;
      unsigned int m_TMax;
      vtkPolyData* m_Points;
      };

    std::list< InternalMeshStructure > m_ListOfMeshes;
    std::list< InternalTrackStructure > m_ListOfTracks;

    void SaveMeshInDataBase( const InternalMeshStructure& iMesh );
    void SaveTrackInDataBase( const InternalTrackStructure& iTrack );


};
#endif
