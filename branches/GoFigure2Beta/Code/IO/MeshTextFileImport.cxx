#include "MeshTextFileImport.h"

#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"

#include "MegaCaptureHeaderReader.h"
#include "vtkPolyDataMySQLTextWriter.h"

MeshTextFileImport::
MeshTextFileImport()
{}

MeshTextFileImport::
~MeshTextFileImport()
{}

void MeshTextFileImport::
SetFileName( const std::string& iFileName )
{
  m_FileName = iFileName;
}

void MeshTextFileImport::
Read()
{
  std::string line;
  std::ifstream ifs( m_FileName.c_str(), std::ifstream::in );
  if( ifs.is_open() )
    {
    //<ImagingSession>
    getline( ifs, line );

    //HeaderFile CaltechZebrafishSubsetData.meg
    std::string word;
    ifs >> word >> m_MegaCaptureHeaderFile;

    // Read megacapture header
    MegaCaptureHeaderReader header_reader;
    header_reader.SetFileName( m_MegaCaptureHeaderFile );
    header_reader.Read();

    m_NumberOfChannels = header_reader.m_NumberOfChannels;

    double spacing[3];
    spacing[0] = header_reader.m_VoxelSizeX;
    spacing[1] = header_reader.m_VoxelSizeY;
    spacing[2] = header_reader.m_VoxelSizeZ;

    //</ImagingSession>
    getline( ifs, line );

    // NumberOfMeshes 1622
    ifs >> word >> m_NumberOfMeshes;

    vtkPolyDataMySQLTextWriter* Converter = vtkPolyDataMySQLTextWriter::New();

    unsigned int ch;

    for( unsigned int i = 0; i < m_NumberOfMeshes; i++ )
      {
      InternalMeshStructure mesh( m_NumberOfChannels );

      // <mesh>
      getline( ifs, line );

      // TrackId 1685
      ifs >> word >> mesh.m_TrackId;

      // TCoord 2
      ifs >>word >> mesh.m_TCoord;

      // Centroid 89.6544 2.1618 29.8110
      // useless information
      getline( ifs, line );

      // Volume 531.28
      ifs >> word >> mesh.m_Volume;

      std::string filename;

      //Filename 2_1.vtk
      ifs >> word >> filename;

      // Read filename
      vtkPolyDataReader* reader = vtkPolyDataReader::New();
      reader->SetFileName( filename.c_str() );
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

      mesh.m_Points = Converter->GetMySQLText( vtk_mesh );

      reader->Delete();
      vtk_mesh->Delete();

      for( ch = 0; ch < m_NumberOfChannels; ch++ )
        {
        // <intensity>
        getline( ifs, line );

        // Channel 0
        getline( ifs, line );

        // AverageValue 119.68
        ifs >> word >> mesh.m_AverageIntensity[ch];

        m_ListOfMeshes.push_back( mesh );

        // </intensity>
        getline( ifs, line );
        }
      // </mesh>
      getline( ifs, line );
      }

    Converter->Delete();
    }
}