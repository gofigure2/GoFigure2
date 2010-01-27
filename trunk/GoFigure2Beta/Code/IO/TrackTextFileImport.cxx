#include "TrackTextFileImport.h"

#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"

#include "MegaCaptureHeaderReader.h"
#include "vtkPolyDataMySQLTextWriter.h"

#include "vtkMySQLDatabase.h"
#include "QueryDataBaseHelper.h"

#include "GoDBCoordinateRow.h"
#include "GoDBMeshRow.h"
#include "GoDBTrackRow.h"

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
    MegaCaptureHeaderReader header_reader( filename1 );
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
      InternalTrackStructure track;

      track.m_TrackId = j;

      std::string points = "0";
      points += " ";

      //<Track>
      getline( ifs, line );

      // LineageID
      ifs >> word >> track.m_LineageID;
      getline( ifs, line );

      //<ListOfMeshes>
      getline( ifs, line );

      // NumberOfMeshes 1622
      unsigned int numberOfMeshes;
      ifs >> word >> numberOfMeshes;
      getline( ifs, line );

      unsigned int ch;
      std::vector< InternalMeshStructure > listofmeshes;

      for( unsigned int i = 0; i < numberOfMeshes; i++ )
        {
        InternalMeshStructure mesh( m_NumberOfChannels );

        // <mesh>Write
        getline( ifs, line );

/// \todo to be removed in the file (too many duplications)
//         // TrackId 1685
//         ifs >> word >> mesh.m_TrackId;

        // assign j to the mesh TrackID
        mesh.m_TrackId = j;

        // TCoord 2
        ifs >> word >> mesh.m_TCoord;

        // Fill the Track structure
        if( mesh.m_TCoord < track.m_TMin )
          {
          track.m_TMin = mesh.m_TCoord;
          }
        if( mesh.m_TCoord > track.m_TMax )
          {
          track.m_TMax = mesh.m_TCoord;
          }

        getline( ifs, line );
        // Centroid 89.6544 2.1618 29.8110
        std::string centerX;
        std::string centerY;
        std::string centerZ;
        ifs >> word >> centerX >> centerY >> centerZ;

        getline( ifs, line );

        // Volume 531.28
        ifs >> word >> mesh.m_Volume;

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

        vtkPolyData* vtk_mesh = reader->GetOutput();

        double bounds[6];
        vtk_mesh->GetBounds( bounds );

        mesh.m_XMin = static_cast< unsigned int >( bounds[0] / spacing[0] );
        mesh.m_XMax = static_cast< unsigned int >( bounds[1] / spacing[0] );
        mesh.m_YMin = static_cast< unsigned int >( bounds[2] / spacing[1] );
        mesh.m_YMax = static_cast< unsigned int >( bounds[3] / spacing[1] );
        mesh.m_ZMin = static_cast< unsigned int >( bounds[4] / spacing[2] );
        mesh.m_ZMax = static_cast< unsigned int >( bounds[5] / spacing[2] );

        // Fill the Track Structure
        if( mesh.m_XMin < track.m_XMin )
        	{
          track.m_XMin = mesh.m_XMin;
        	}
        if( mesh.m_XMax > track.m_XMax )
          {
          track.m_XMax = mesh.m_XMax;
          }
        if( mesh.m_YMin < track.m_YMin )
          {
          track.m_YMin = mesh.m_YMin;
          }
        if( mesh.m_YMax > track.m_YMax )
          {
          track.m_YMax = mesh.m_YMax;
          }
        if( mesh.m_ZMin < track.m_ZMin )
          {
          track.m_ZMin = mesh.m_ZMin;
          }
        if( mesh.m_ZMax > track.m_ZMax )
          {
          track.m_ZMax = mesh.m_ZMax;
          }

        mesh.m_Points = vtk_mesh;

        // Re-write the points "string"
        std::stringstream streamTCoord;
        std::string       stringTCoord;
        streamTCoord << mesh.m_TCoord;
        stringTCoord = streamTCoord.str();

        points = AddTimePoint(points, centerX, centerY, centerZ, stringTCoord, true);


        for( ch = 0; ch < m_NumberOfChannels; ch++ )
          {
          // <intensity>
          getline( ifs, line );

          // Channel 0
          getline( ifs, line );

          // AverageValue 119.68
          ifs >> word >> mesh.m_AverageIntensity[ch];

          listofmeshes.push_back( mesh );

          getline( ifs, line );
          // </intensity>
          getline( ifs, line );
          }
        // </mesh>
        getline( ifs, line );

        SaveMeshInDataBase( mesh );

        reader->Delete();
        }
      //</ListOfMeshes>
      getline( ifs, line );

      // Remove time points in the points "string"
      track.m_Points = AddTimePoint(points, "", "", "", "", false);
      cout << "Track to be saved: " << track.m_Points << endl;
      SaveTrackInDataBase( track );

      //</Track>
      getline( ifs, line );
      }
    }
}

/**
 * \brief Save the mesh informations into the database
 * \param[in]  iMesh Name of the mesh structure to be saved
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
  mesh_row.SetColor( 165, 44, 23, 255, "KishoreMeshColor", m_DBConnector );
  int mesh_id = mesh_row.SaveInDB( m_DBConnector );
  std::cout << "Mesh ID: " << mesh_id <<std::endl;
}

/**
 * \brief Save the track informations into the database
 * \param[in]  iTrack Name of the track structure to be saved
 */
void
TrackTextFileImport::
SaveTrackInDataBase( const InternalTrackStructure& iTrack )
{
  GoDBCoordinateRow coord_min;
  coord_min.SetField< unsigned int >( "XCoord", iTrack.m_XMin );
  coord_min.SetField< unsigned int >( "YCoord", iTrack.m_YMin );
  coord_min.SetField< unsigned int >( "ZCoord", iTrack.m_ZMin );
  coord_min.SetField< unsigned int >( "TCoord", iTrack.m_TMin );

  GoDBCoordinateRow coord_max;
  coord_max.SetField< unsigned int >( "XCoord", iTrack.m_YMax );
  coord_min.SetField< unsigned int >( "YCoord", iTrack.m_YMax );
  coord_min.SetField< unsigned int >( "ZCoord", iTrack.m_ZMax );
  coord_max.SetField< unsigned int >( "TCoord", iTrack.m_TMax );

  GoDBTrackRow track_row( m_DBConnector, coord_min, coord_max,
    m_ImagingSessionId, iTrack.m_Points);

  track_row.SetColor( 55, 25, 0, 255,"KishoreTrackColor1", m_DBConnector );
  int track_id = track_row.SaveInDB( m_DBConnector );
  std::cout << "Track ID: " << track_id <<std::endl;
}

/**
 * \brief Add point to the track string the saved in the database
 * \param[in] iTrackList String in which one point will be added
 * \param[in] iX String containing the X position of the new point
 * \param[in] iY String containing the Y position of the new point
 * \param[in] iZ String containing the Z position of the new point
 * \param[in] iT String containing the Time position of the new point
 * \param[in] iTimePoint Bool == "true" if we want the output string to contain time information (iT, which is requiered to add a new point)
 */
std::string
TrackTextFileImport::
AddTimePoint(const std::string& iTrackList, const std::string& iX,const std::string& iY, const std::string& iZ, const std::string& iT, bool iTimePoint)
{
  std::stringstream str( iTrackList );
  std::string numberOfCoordinatesString;
  str >> numberOfCoordinatesString ;

  unsigned int numberOfCoordinates;
  numberOfCoordinates = atoi( numberOfCoordinatesString.c_str() );

  // Decompose the string into a map
  std::map< std::string, std::string > mapOfCoordinates;

  for(unsigned int i=0; i<numberOfCoordinates; i++)
    {
	std::string xOld = "";
	std::string yOld = "";
	std::string zOld = "";
	std::string tOld = "";
	std::string xyzOld = "";
	str >> xOld >> yOld >> zOld;
	xyzOld += xOld;
	xyzOld += " ";
	xyzOld += yOld;
	xyzOld += " ";
	xyzOld += zOld;

	str >> tOld;
	mapOfCoordinates[tOld] = xyzOld;
    }

  // Add the new current value in the map
  if(iTimePoint)
  	{
    std::string xyzNew = "";
    xyzNew += iX;
    xyzNew += " ";
    xyzNew += iY;
    xyzNew += " ";
    xyzNew += iZ;

    mapOfCoordinates[iT] = xyzNew;
  	}
  // Fill string with the new map
  std::map<  std::string, std::string >::iterator it;
  std::string outputString = "";
  numberOfCoordinates = (int)mapOfCoordinates.size();

  std::stringstream numberOfCoordinatesStream;
  numberOfCoordinatesStream << numberOfCoordinates;
  numberOfCoordinatesString = numberOfCoordinatesStream.str();

  outputString += numberOfCoordinatesString;

  for ( it=mapOfCoordinates.begin() ; it != mapOfCoordinates.end(); it++ )
    {
	outputString += " ";
	outputString += (*it).second;
	if(iTimePoint)
	  {
	  outputString += " ";
	  outputString += (*it).first;
	  }
    }

  return outputString;
}
