/*=========================================================================
  Author: $Author: lydiesouhait$  // Author of last commit
  Version: $Rev: 1972 $  // Revision of last commit
  Date: $Date: 2010-08-16 12:23:05 -0400 (Mon, 16 Aug 2010) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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
#include "GoDBTWContainerForMesh.h"

GoDBTWContainerForMesh::GoDBTWContainerForMesh(int iImgSessionID):
  GoDBTWContainerForContourMesh("mesh", "track", iImgSessionID),
  m_MeshAttributes(NULL)
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBTWContainerForMesh::~GoDBTWContainerForMesh()
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForMesh::SetSpecificColumnsInfoForMesh()
{
  //Get the info for the total intensities per channel:
  GoDBTraceInfoForTableWidget temp;

  std::pair< GoDBTraceInfoForTableWidget,
             std::vector< std::string > > PairTemp;
  size_t                                  NumberOfChannels = this->m_ChannelsInfo.size();

  if ( this->m_ChannelsInfo.empty() )
    {
    std::cout << "No info for the channels" << std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return;
    }
  for ( size_t i = 0; i < NumberOfChannels; i++ )
    {
    std::string InfoName = "TotalIntensityForChannelID";
    InfoName += this->m_ChannelsInfo.at(i).at(1);
    temp.InfoName = InfoName;
    temp.ColumnNameDatabase = "Value";
    std::string ColumnNameTableWidget = "T.I.";
    ColumnNameTableWidget += this->m_ChannelsInfo.at(i).at(0);
    temp.ColumnNameTableWidget = ColumnNameTableWidget;
    temp.TableNameDatabase = "intensity";
    temp.TableForeignKeyDatabase = "MeshID";
    temp.TableKeyDatabase = "MeshID";
    m_ColumnsInfos.push_back(temp);
    PairTemp.first = temp;
    m_RowContainer.push_back(PairTemp);
    temp.Clear();
    }
  //Get the info for the Volume:
  temp.InfoName = "Volume";
  temp.ColumnNameTableWidget = "Volume";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the Surface Area:
  temp.InfoName = "SurfaceArea";
  temp.ColumnNameTableWidget = "SurfaceArea";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//std::vector<std::vector<std::string> > GoDBTWContainerForMesh::
//GetChannelsInfo()
//{
//  return this->m_ChannelsInfo;
//}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForMesh::FillRowContainerForMeshValues(
  vtkMySQLDatabase *iDatabaseConnector, std::vector< std::string > iVectMeshIDs)
{
  std::vector< std::vector< std::string > > ValuesToFill;
  std::vector< std::string >                SelectFields;
  this->GetValuesForIntensities(iDatabaseConnector, iVectMeshIDs, ValuesToFill,
                                SelectFields);
  //if the mesh was just created from the visu (so there is only one):
  if ( this->m_MeshAttributes != 0 && iVectMeshIDs.size() == 1 )
    {
    this->GetValuesForSurfaceVolume(ValuesToFill,
                                    SelectFields);
    }

  this->FillRowContainer(
    ValuesToFill, SelectFields, "ColumnNameTableWidget");
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForMesh::GetValuesForIntensities(
  vtkMySQLDatabase *iDatabaseConnector,
  std::vector< std::string > iVectMeshIDs,
  std::vector< std::vector< std::string > > & ioValuesToFill,
  std::vector< std::string >               & ioSelectFields)
{
  if ( this->m_ChannelsInfo.empty() )
    {
    this->SetChannelsInfo(iDatabaseConnector);
    }
  //fill the columns names to be filled in the row container:
  for ( unsigned int i = 0; i < this->m_ChannelsInfo.size(); i++ )
    {
    std::string NameTableWidgetColumn = "T.I.";
    NameTableWidgetColumn += this->m_ChannelsInfo.at(i).at(0);
    ioSelectFields.push_back(NameTableWidgetColumn);
    }

  if ( iVectMeshIDs.empty() )
    {
    return;
    }
  //get the needed data from the database:
  std::vector< std::string > SelectedFields(3);
  SelectedFields.at(0) = "mesh.meshid";
  SelectedFields.at(1) = "left(points,10) as points";
  SelectedFields.at(2) = "value";
  std::vector< std::string > ResultQuery;

  if ( iVectMeshIDs.size() == 1 )
    {
    this->GetIntensityValuesForOneMesh(iVectMeshIDs.front(),
                                       ioValuesToFill, iDatabaseConnector);
    }
  else
    {
    ResultQuery = GetAllSelectedValuesFromTwoTables(
      iDatabaseConnector, "mesh", "intensity", SelectedFields,
      "mesh.meshid = intensity.meshid", "imagingsessionid",
      ConvertToString< int >(this->m_ImgSessionID) );
    this->GetValuesToFillForIntensityFromQueryResults(
      ResultQuery, iVectMeshIDs, ioValuesToFill);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForMesh::GetIntensityValuesForOneMesh(std::string iMeshID,
                                                          std::vector< std::vector< std::string > > & ioValuesToFill,
                                                          vtkMySQLDatabase *iDatabaseConnector)
{
  std::vector< std::string > temp;
  for ( unsigned int i = 0; i < this->m_ChannelsInfo.size(); i++ )
    {
    std::string ChannelID = this->m_ChannelsInfo.at(i).at(1);
    int         ValueIntensity = FindOneID(iDatabaseConnector, "intensity",
                                           "Value", "MeshID", iMeshID, "ChannelID", ChannelID);
    if ( ValueIntensity == -1 )
      {
      temp.push_back("");
      }
    else
      {
      temp.push_back( ConvertToString< int >(ValueIntensity) );
      }
    }
  ioValuesToFill.push_back(temp);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForMesh::GetValuesToFillForIntensityFromQueryResults
  (std::vector< std::string > iResultQuery, std::vector< std::string > iVectMeshIDs,
  std::vector< std::vector< std::string > > & ioValuesToFill)
{
  std::vector< std::string >::iterator iterResult = iResultQuery.begin();

  //iResultQuery has meshid, points, value for channel0, meshid,points,
  //value for channel1....
  std::vector< std::string >::iterator iterMeshID = iVectMeshIDs.begin();

  size_t i;

  while( ( iterMeshID != iVectMeshIDs.end() ) &&
        ( iterResult != iResultQuery.end() ) )
    {
    ++iterResult;
    std::vector< std::string > temp;
    std::string Points = *iterResult;
    std::string IntensityValue;
    if ( ( Points == "0" ) || ( Points == "" ) ) //if the mesh has no points, he
                                                 // has no intensity
      {
      IntensityValue = "";

      for ( i = 0; i < this->m_ChannelsInfo.size(); i++ )
        {
        temp.push_back(IntensityValue);
        }
      iterResult = iterResult + 2;
      }
    else
      {
      ++iterResult;

      for ( i = 0; i < this->m_ChannelsInfo.size() - 1; i++ )
        {
        if ( iterResult != iResultQuery.end() )
          {
          IntensityValue = *iterResult;
          temp.push_back(IntensityValue);
          }
        iterResult = iterResult + 3;
        }
      IntensityValue = *iterResult;
      temp.push_back(IntensityValue);
      ++iterResult;
      }
    ioValuesToFill.push_back(temp);
    ++iterMeshID;
    }

  bool iterMeshID_at_end = ( iterMeshID == iVectMeshIDs.end() );
  bool iterResult_at_end = ( iterResult == iResultQuery.end() );

  if( iterMeshID_at_end && !iterResult_at_end )
    {
    std::cout << "iterMeshID_at_end AND !iterResult_at_end" <<std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return;
    }
  if( !iterMeshID_at_end && iterResult_at_end )
    {
    std::cout << "!iterMeshID_at_end AND iterResult_at_end" <<std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return;
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForMesh::FillRowContainerForMeshValues(
  vtkMySQLDatabase *iDatabaseConnector, int iMeshID)
{
  std::vector< std::string > MeshIDs;
  MeshIDs.push_back( ConvertToString< int >(iMeshID) );
  this->FillRowContainerForMeshValues(iDatabaseConnector, MeshIDs);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForMesh::GetValuesForSurfaceVolume(
  std::vector< std::vector< std::string > > & ioValuesToFill,
  std::vector< std::string >               & ioSelectFields)
{
  if ( this->m_MeshAttributes != 0 )
    {
    if ( ioValuesToFill.size() != 1 )
      {
      std::cout << "more than 1 mesh for volume and surface values" << std::endl;
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      return;
      }
    else
      {
      std::vector< std::string > temp = ioValuesToFill.at(0);
      ioSelectFields.push_back("Volume");
      temp.push_back( ConvertToString< double >(this->m_MeshAttributes->m_Volume) );
      ioSelectFields.push_back("SurfaceArea");
      temp.push_back( ConvertToString< double >(this->m_MeshAttributes->m_Area) );
      ioValuesToFill.clear();
      ioValuesToFill.push_back(temp);
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBTableWidgetContainer::TWContainerType
GoDBTWContainerForMesh::GetContainerLoadedWithAllFromDB(
  vtkMySQLDatabase *iDatabaseConnector)
{
  GoDBTableWidgetContainer::GetContainerLoadedWithAllFromDB(iDatabaseConnector);
  std::vector< std::string > VectMeshIDs = ListSpecificValuesForOneColumn(
    iDatabaseConnector, "mesh", "MeshID", "ImagingSessionID",
    ConvertToString< unsigned int >(this->m_ImgSessionID) );

  this->FillRowContainerForMeshValues(iDatabaseConnector, VectMeshIDs);

  //what about surfacearea + volume ? FillRowContainerForComputedvalues to check
  return this->m_RowContainer;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForMesh::SetChannelsInfo(
  vtkMySQLDatabase *iDatabaseConnector)
{
  if ( this->m_ChannelsInfo.empty() )
    {
    std::vector< std::string > SelectFields;
    SelectFields.push_back("Name");
    SelectFields.push_back("channel.ChannelID");
    std::vector< std::string > JoinTablesOnTraceTable;
    JoinTablesOnTraceTable.push_back("channel");
    JoinTablesOnTraceTable.push_back("image.ChannelID = channel.ChannelID");

    this->m_ChannelsInfo = GetValuesFromSeveralTables(
      iDatabaseConnector, "image", SelectFields, "ImagingSessionID",
      ConvertToString< unsigned int >(this->m_ImgSessionID), JoinTablesOnTraceTable, true);
    this->SetSpecificColumnsInfoForMesh();
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBTableWidgetContainer::TWContainerType
GoDBTWContainerForMesh::GetContainerForOneSpecificTrace(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
{
  GoDBTableWidgetContainer::GetContainerForOneSpecificTrace(iDatabaseConnector,
                                                            iTraceID);
  this->FillRowContainerForMeshValues(iDatabaseConnector, iTraceID);
  this->m_MeshAttributes = 0;
  return this->m_RowContainer;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTWContainerForMesh::SetMeshAttributes(
  GoFigureMeshAttributes *iMeshAttributes)
{
  this->m_MeshAttributes = iMeshAttributes;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
