/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "MeshTextFileImport.h"

#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"

#include "MegaCaptureHeaderReader.h"
#include "vtkPolyDataMySQLTextWriter.h"

#include "vtkMySQLDatabase.h"
#include "QueryDataBaseHelper.h"

#include "GoDBCoordinateRow.h"
#include "GoDBMeshRow.h"

MeshTextFileImport::
MeshTextFileImport( const std::string& iServerName, const std::string& iLogin,
  const std::string& iPassword, const std::string& iDBName,
  const unsigned int& iImagingSessionId ) : m_ImagingSessionId( iImagingSessionId )
{
  m_DBConnector =
    OpenDatabaseConnection( iServerName, iLogin, iPassword, iDBName );
}

MeshTextFileImport::
~MeshTextFileImport()
{
  m_DBConnector->Delete();
}

void MeshTextFileImport::
SetDirectory( const std::string& iDir )
{
  m_Directory = iDir;
}

void MeshTextFileImport::
SetFileName( const std::string& iFileName )
{
  m_FileName = iFileName;
}

void MeshTextFileImport::
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

    // NumberOfMeshes 1622
    ifs >> word >> m_NumberOfMeshes;
    getline( ifs, line );

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

      getline( ifs, line );
      // Centroid 89.6544 2.1618 29.8110
      // useless information
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

      mesh.m_Points = vtk_mesh;

      for( ch = 0; ch < m_NumberOfChannels; ch++ )
        {
        // <intensity>
        getline( ifs, line );

        // Channel 0
        getline( ifs, line );

        // AverageValue 119.68
        ifs >> word >> mesh.m_AverageIntensity[ch];

        m_ListOfMeshes.push_back( mesh );

        getline( ifs, line );
        // </intensity>
        getline( ifs, line );
        }
      // </mesh>
      getline( ifs, line );

      SaveMeshInDataBase( mesh );

      reader->Delete();
      }
    }
}

void
MeshTextFileImport::
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

  GoDBMeshRow mesh_row( m_DBConnector,iMesh.m_Points, coord_min, 
    coord_max,m_ImagingSessionId);
  mesh_row.SetColor( 255, 255, 0, 255,"KishoreMeshColor", m_DBConnector );
  int mesh_id = mesh_row.SaveInDB( m_DBConnector );
  std::cout <<mesh_id <<std::endl;
}
