#include "TrackTextFileImport.h"

#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"

#include "MegaCaptureHeaderReader.h"
#include "vtkPolyDataMySQLTextWriter.h"

#include "vtkMySQLDatabase.h"
#include "QueryDataBaseHelper.h"

#include "GoDBCoordinateRow.h"
#include "GoDBMeshRow.h"

/**
 * \brief Constructor
 */
TrackTextFileImport::
TrackTextFileImport( const std::string& iServerName, const std::string& iLogin,
  const std::string& iPassword, const std::string& iDBName,
  const unsigned int& iImagingSessionId ) : m_ImagingSessionId( iImagingSessionId )
{
  m_DBConnector =
    OpenDatabaseConnection( iServerName, iLogin, iPassword, iDBName );
}

/**
 * \brief Destructor
 */
TrackTextFileImport::
~TrackTextFileImport()
{
  m_DBConnector->Delete();
}

/**
 * \brief Set the directory which contains the TrackText files
 * \param[in]  iDir Name of the directory which contains the TrackText files
 */
void
TrackTextFileImport::
SetDirectory( const std::string& iDir )
{
  m_Directory = iDir;
}

/**
 * \brief Set the name of the TrackText files
 * \param[in]  iDir Name of the TrackText files
 */
void
TrackTextFileImport::
SetFileName( const std::string& iFileName )
{
  m_FileName = iFileName;
}

/**
 * \brief Read the TrackTex file
 */
void
TrackTextFileImport::
Read()
{
  std::string filename0 = m_Directory;
  filename0 += m_FileName;

  std::string line;
  std::ifstream ifs( filename0.c_str(), std::ifstream::in );
  if( ifs.is_open() )
    {
    //<ImagingSession>
    getline( ifs, line );

    //HeaderFile CaltechZebrafishSubsetData.meg
    std::string word;
    ifs >> word >> m_MegaCaptureHeaderFile;

    std::string filename1 = m_Directory;
    filename1 += m_MegaCaptureHeaderFile;

    // Read megacapture header
    MegaCaptureHeaderReader header_reader;
    header_reader.SetFileName( filename1 );
    header_reader.Read();

    m_NumberOfChannels = header_reader.m_NumberOfChannels;

    double spacing[3];
    spacing[0] = header_reader.m_VoxelSizeX;
    spacing[1] = header_reader.m_VoxelSizeY;
    spacing[2] = header_reader.m_VoxelSizeZ;

    getline( ifs, line );
    //</ImagingSession>
    getline( ifs, line );

    // NumberOfTracks
    ifs >> word >> m_NumberOfTracks;
    getline( ifs, line );

    for( unsigned int j = 0; j < m_NumberOfTracks; j++ )
      {
      InternalTrackStructure track(m_NumberOfChannels);
      //<Track>
      getline( ifs, line );

      // LineageID
      ifs >> word >> track.m_LineageID;
      getline( ifs, line );

      //<ListOfMeshes>
      getline( ifs, line );

      // NumberOfMeshes 1622
      ifs >> word >> m_NumberOfMeshes;
      getline( ifs, line );

      unsigned int ch;

      for( unsigned int i = 0; i < m_NumberOfMeshes; i++ )
        {
        //InternalMeshStructure mesh( m_NumberOfChannels );

        // <mesh>Write
        getline( ifs, line );

        // TrackId 1685
        //ifs >> word >> mesh.m_TrackId;
        ifs >> word >> track.m_TrackId;

        // TCoord 2
        //ifs >> word >> mesh.m_TCoord;
        ifs >> word >> track.m_TCoord;

        getline( ifs, line );
        // Centroid 89.6544 2.1618 29.8110
        // useless information
        getline( ifs, line );

        // Volume 531.28
        //ifs >> word >> mesh.m_Volume;
        ifs >> word >> track.m_Volume;

        std::string filename;
        std::string filename2 = m_Directory;

        //Filename 2_1.vtk
        ifs >> word >> filename;
        getline( ifs, line );

        filename2 += filename;

        // Read filename
        vtkPolyDataReader* reader = vtkPolyDataReader::New();
        reader->SetFileName( filename2.c_str() );
        reader->Update();

        //vtkPolyData* vtk_mesh = reader->GetOutput();
        //
        //double bounds[6];
        //vtk_mesh->GetBounds( bounds );
        //
        //mesh.m_XMin = static_cast< unsigned int >( bounds[0] / spacing[0] );
        //mesh.m_XMax = static_cast< unsigned int >( bounds[1] / spacing[0] );
        //mesh.m_YMin = static_cast< unsigned int >( bounds[2] / spacing[1] );
        //mesh.m_YMax = static_cast< unsigned int >( bounds[3] / spacing[1] );
        //mesh.m_ZMin = static_cast< unsigned int >( bounds[4] / spacing[2] );
        //mesh.m_ZMax = static_cast< unsigned int >( bounds[5] / spacing[2] );
        //
        //mesh.m_Points = vtk_mesh;

        vtkPolyData* vtk_track = reader->GetOutput();

        double bounds[6];
        vtk_track->GetBounds( bounds );

        track.m_XMin = static_cast< unsigned int >( bounds[0] / spacing[0] );
        track.m_XMax = static_cast< unsigned int >( bounds[1] / spacing[0] );
        track.m_YMin = static_cast< unsigned int >( bounds[2] / spacing[1] );
        track.m_YMax = static_cast< unsigned int >( bounds[3] / spacing[1] );
        track.m_ZMin = static_cast< unsigned int >( bounds[4] / spacing[2] );
        track.m_ZMax = static_cast< unsigned int >( bounds[5] / spacing[2] );

        track.m_Points = vtk_track;

        for( ch = 0; ch < m_NumberOfChannels; ch++ )
          {
          // <intensity>
          getline( ifs, line );

          // Channel 0
          getline( ifs, line );

          // AverageValue 119.68
          //ifs >> word >> mesh.m_AverageIntensity[ch];
          ifs >> word >> track.m_AverageIntensity[ch];

          //m_ListOfMeshes.push_back( mesh );
          m_ListOfTracks.push_back( track );

          getline( ifs, line );
          // </intensity>
          getline( ifs, line );
          }
        // </mesh>
        getline( ifs, line );

        //SaveMeshInDataBase( mesh );
        SaveTrackInDataBase( track );
        reader->Delete();
        }
      //</ListOfMeshes>
      getline( ifs, line );
      //</Track>
      getline( ifs, line );
      }
    }
}

/**
 * \brief Save the meshes informations into the database
 */
void
TrackTextFileImport::
SaveMeshInDataBase( const InternalMeshStructure& iMesh )
{
  GoDBCoordinateRow coord_min;
  coord_min.SetField< unsigned int >( "XCoord", iMesh.m_XMin );
  coord_min.SetField< unsigned int >( "YCoord", iMesh.m_YMin );
  coord_min.SetField< unsigned int >( "ZCoord", iMesh.m_ZMin );
  coord_min.SetField< unsigned int >( "TCoord", iMesh.m_TCoord );

  GoDBCoordinateRow coord_max;
  coord_max.SetField< unsigned int >( "XCoord", iMesh.m_XMax );
  coord_max.SetField< unsigned int >( "YCoord", iMesh.m_YMax );
  coord_max.SetField< unsigned int >( "ZCoord", iMesh.m_ZMax );
  coord_max.SetField< unsigned int >( "TCoord", iMesh.m_TCoord );

  GoDBMeshRow mesh_row( m_DBConnector, coord_min, coord_max,
    m_ImagingSessionId, iMesh.m_Points );
  mesh_row.SetColor( 255, 255, 0, 255,"KishoreColor", m_DBConnector );
  int mesh_id = mesh_row.SaveInDB( m_DBConnector );
  std::cout <<mesh_id <<std::endl;
}

/**
 * \brief Save the tracks informations into the database
 */
void
TrackTextFileImport::
SaveTrackInDataBase( const InternalTrackStructure& iTrack )
{
  GoDBCoordinateRow coord_min;
  coord_min.SetField< unsigned int >( "XCoord", iTrack.m_XMin );
  coord_min.SetField< unsigned int >( "YCoord", iTrack.m_YMin );
  coord_min.SetField< unsigned int >( "ZCoord", iTrack.m_ZMin );
  coord_min.SetField< unsigned int >( "TCoord", iTrack.m_TCoord );

  GoDBCoordinateRow coord_max;
  coord_max.SetField< unsigned int >( "XCoord", iTrack.m_YMax );
  coord_min.SetField< unsigned int >( "YCoord", iTrack.m_YMax );
  coord_max.SetField< unsigned int >( "ZCoord", iTrack.m_ZMax );
  coord_max.SetField< unsigned int >( "TCoord", iTrack.m_TCoord );

  GoDBMeshRow track_row( m_DBConnector, coord_min, coord_max,
    m_ImagingSessionId, iTrack.m_Points );
  track_row.SetColor( 255, 255, 0, 255,"KishoreColor", m_DBConnector );
  int track_id = track_row.SaveInDB( m_DBConnector );
  std::cout <<track_id <<std::endl;
}
