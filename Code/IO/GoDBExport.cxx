/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
  Date: $Date$  // Date of last commit
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

#include "GoDBExport.h"
#include "SelectQueryDatabaseHelper.h"
#include "QueryDataBaseHelper.h"
#include "ConvertToStringHelper.h"
#include "GoDBColorRow.h"
#include "GoDBCellTypeRow.h"
#include "GoDBSubCellTypeRow.h"
#include "GoDBCoordinateRow.h"
#include "GoDBContourRow.h"
#include "GoDBMeshRow.h"
#include "GoDBTrackRow.h"
#include "GoDBLineageRow.h"
#include "GoDBChannelRow.h"
#include "GoDBIntensityRow.h"

//--------------------------------------------------------------------------
GoDBExport::GoDBExport(std::string iServerName, std::string iLogin,
                       std::string iPassword, int iImagingSessionID, std::string iFilename)
{
  this->m_ServerName = iServerName;
  this->m_Login = iLogin;
  this->m_Password = iPassword;
  this->m_ImagingSessionID = iImagingSessionID;
  this->m_outfile.open (iFilename.c_str(), std::ios::out);
}

//--------------------------------------------------------------------------
GoDBExport::~GoDBExport()
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::ExportContours()
{
  this->WriteGeneraleInfo();
  this->OpenDBConnection();
  this->WriteOnTheOutputFile( "imagingsession", this->GetImagingSessionInfoFromDB() );
  this->UpdateAllVectorTracesIDsToExportContours();
  this->WriteTheColorsInfoFromDatabase();
  this->WriteCellTypeAndSubCellTypeInfoFromDatabase();
  this->WriteCoordinatesInfoFromDatabase();
  this->WriteLineagesInfoFromDatabase();
  this->WriteTracksInfoFromDatabase();
  this->WriteMeshesInfoFromDatabase();
  this->WriteContoursInfoFromDatabase();
  this->CloseDBConnection();
  this->m_outfile << this->GetNameWithSlashBrackets(this->m_NameDocXml);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::ExportMeshes()
{
  this->WriteGeneraleInfo();
  this->OpenDBConnection();
  this->WriteOnTheOutputFile( "imagingsession",
                              this->GetImagingSessionInfoFromDB() );
  this->UpdateAllVectorTracesIDsToExportMeshes();
  this->WriteTheColorsInfoFromDatabase();
  this->WriteCellTypeAndSubCellTypeInfoFromDatabase();
  this->WriteCoordinatesInfoFromDatabase();
  this->WriteLineagesInfoFromDatabase();
  this->WriteTracksInfoFromDatabase();
  this->WriteMeshesInfoFromDatabase();
  this->WriteChannelsInfoFromDatabase();
  this->WriteIntensityInfoFromDatabase();
  this->CloseDBConnection();
  this->m_outfile << this->GetNameWithSlashBrackets(this->m_NameDocXml);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteGeneraleInfo()
{
  this->m_NameDocXml = "ExportTraces";
  int VersionNumber = 1;
  this->m_outfile << "<?xml version=\"1.0\" ?>" << std::endl;
  this->m_outfile << "<";
  this->m_outfile << this->m_NameDocXml;
  this->m_outfile << " version=\"";
  this->m_outfile << VersionNumber;
  this->m_outfile << "\">" << std::endl;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< std::pair< std::string, std::string > >
GoDBExport::GetImagingSessionInfoFromDB()
{
  std::vector< std::pair< std::string, std::string > > infoImgSession;
  infoImgSession.push_back( this->GetOneInfoFromDBForImgSession("Name") );
  infoImgSession.push_back( this->GetOneInfoFromDBForImgSession("CreationDate") );
  infoImgSession.push_back( this->GetOneInfoFromDBForImgSession("MicroscopeName") );

  return infoImgSession;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::pair< std::string, std::string > GoDBExport::GetOneInfoFromDBForImgSession(
  std::string iNameInfo)
{
  std::pair< std::string, std::string > OneInfo;
  OneInfo.first = iNameInfo;
  OneInfo.second = ListSpecificValuesForOneColumn(
    this->m_DatabaseConnector, "imagingsession", iNameInfo, "ImagingSessionID",
    ConvertToString< int >(this->m_ImagingSessionID) ).at(0);
  return OneInfo;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::UpdateVectorContourIDsForExportContours()
{
  this->m_VectorContourIDs = ListSpecificValuesForOneColumn(
    this->m_DatabaseConnector, "contour", "contourID", "imagingsessionID",
    ConvertToString< int >(this->m_ImagingSessionID) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::UpdateVectorContourIDsForExportMeshes()
{
  this->m_VectorContourIDs.clear();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::UpdateVectorMeshIDsForExportContours()
{
  this->m_VectorMeshIDs = ListSpecificValuesForOneColumn(
    this->m_DatabaseConnector, "contour", "meshID", "contourID",
    this->m_VectorContourIDs, true, true);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::UpdateVectorMeshIDsForExportMeshes()
{
  this->m_VectorMeshIDs = FindSeveralIDs(this->m_DatabaseConnector,
                                         "mesh", "meshID", "ImagingSessionID",
                                         ConvertToString< int >(this->m_ImagingSessionID), "Points", "0");
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::UpdateVectorChannelIDsForExportMeshes()
{
  this->m_VectorChannelIDs = ListSpecificValuesForOneColumn(
    this->m_DatabaseConnector, "channel", "ChannelID",
    "ImagingSessionID", ConvertToString< int >(this->m_ImagingSessionID) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::UpdateVectorTrackIDsToExportInfo()
{
  if ( !this->m_VectorMeshIDs.empty() )
    {
    this->m_VectorTrackIDs = ListSpecificValuesForOneColumn(
      this->m_DatabaseConnector, "mesh", "trackID", "meshID",
      this->m_VectorMeshIDs, true, true);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::UpdateVectorLineageIDsToExportInfo()
{
  if ( !this->m_VectorTrackIDs.empty() )
    {
    this->m_VectorLineageIDs = ListSpecificValuesForOneColumn(
      this->m_DatabaseConnector, "track", "lineageID", "trackID",
      this->m_VectorTrackIDs, true, true);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::UpdateAllVectorTracesIDsToExportContours()
{
  this->UpdateVectorContourIDsForExportContours();
  this->UpdateVectorMeshIDsForExportContours();
  this->UpdateVectorTrackIDsToExportInfo();
  this->UpdateVectorLineageIDsToExportInfo();
  //no need for channel info when exporting contours at this time:
  this->m_VectorChannelIDs.clear();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::UpdateAllVectorTracesIDsToExportMeshes()
{
  this->UpdateVectorContourIDsForExportMeshes();
  this->UpdateVectorMeshIDsForExportMeshes();
  this->UpdateVectorTrackIDsToExportInfo();
  this->UpdateVectorLineageIDsToExportInfo();
  this->UpdateVectorChannelIDsForExportMeshes();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteTheColorsInfoFromDatabase()
{
  std::vector< std::string >                TablesNames;
  std::vector< std::string >                FieldNames;
  std::vector< std::vector< std::string > > VectorTracesIDs;
  this->GetVectorsTableNamesTracesIDsAndFields(
    TablesNames, VectorTracesIDs, FieldNames, true);
  std::vector< std::string > ColumnNames(1);
  ColumnNames[0] = "ColorID";
  std::vector< std::string > ListColorIDs = GetSameFieldsFromSeveralTables(
    this->m_DatabaseConnector, ColumnNames, TablesNames, FieldNames, VectorTracesIDs);
  this->WriteTableInfoFromDB< GoDBColorRow >(ListColorIDs);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteCellTypeAndSubCellTypeInfoFromDatabase()
{
  std::vector< std::string > ListCellTypeIDs = ListSpecificValuesForOneColumn(
    this->m_DatabaseConnector, "mesh", "CellTypeID", "meshID",
    this->m_VectorMeshIDs, true, true);
  this->WriteTableInfoFromDB< GoDBCellTypeRow >(ListCellTypeIDs);
  std::vector< std::string > ListSubCellTypeIDs =
    ListSpecificValuesForOneColumn(this->m_DatabaseConnector,
                                   "mesh", "SubCellularID", "meshID", this->m_VectorMeshIDs, true, true);
  this->WriteTableInfoFromDB< GoDBSubCellTypeRow >(ListSubCellTypeIDs);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteCoordinatesInfoFromDatabase()
{
  std::vector< std::string >                TablesNames;
  std::vector< std::string >                FieldNames;
  std::vector< std::vector< std::string > > VectorTracesIDs;
  this->GetVectorsTableNamesTracesIDsAndFields(TablesNames,
                                               VectorTracesIDs, FieldNames);
  std::vector< std::string > ColumnNames(2);
  ColumnNames[0] = "CoordIDMax";
  ColumnNames[1] = "CoordIDMin";
  std::vector< std::string > ListCoordIDs = GetSameFieldsFromSeveralTables(
    this->m_DatabaseConnector, ColumnNames, TablesNames, FieldNames, VectorTracesIDs);

  this->WriteTableInfoFromDB< GoDBCoordinateRow >(ListCoordIDs);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteLineagesInfoFromDatabase()
{
  this->WriteTableInfoFromDB< GoDBLineageRow >(this->m_VectorLineageIDs);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteTracksInfoFromDatabase()
{
  this->WriteTableInfoFromDB< GoDBTrackRow >(this->m_VectorTrackIDs);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteMeshesInfoFromDatabase()
{
  this->WriteTableInfoFromDB< GoDBMeshRow >(this->m_VectorMeshIDs);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteChannelsInfoFromDatabase()
{
  this->WriteTableInfoFromDB< GoDBChannelRow >(this->m_VectorChannelIDs);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteIntensityInfoFromDatabase()
{
  std::vector< std::string > VectorIntensityIDs =
    GetSpecificValueFromOneTableWithConditionsOnTwoColumns(
      this->m_DatabaseConnector, "IntensityID", "intensity",
      "meshID", this->m_VectorMeshIDs, "ChannelID", this->m_VectorChannelIDs);
  this->WriteTableInfoFromDB< GoDBIntensityRow >(VectorIntensityIDs);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteContoursInfoFromDatabase()
{
  this->WriteTableInfoFromDB< GoDBContourRow >(m_VectorContourIDs);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteOnTheOutputFile(std::string iNameOfEntity,
                                      std::vector< std::pair< std::string, std::string > > iInfoToWrite)
{
  this->AddTabulation();
  this->m_outfile << GetNameWithBrackets(iNameOfEntity) << std::endl;
  std::vector< std::pair< std::string, std::string > >::iterator iter =
    iInfoToWrite.begin();
  while ( iter != iInfoToWrite.end() )
    {
    this->AddTabulation();
    this->AddTabulation();
    this->m_outfile << this->GetNameWithBrackets(iter->first);
    this->m_outfile << iter->second;
    this->m_outfile << this->GetNameWithSlashBrackets(iter->first) << std::endl;
    iter++;
    }
  this->AddTabulation();
  this->m_outfile << GetNameWithSlashBrackets(iNameOfEntity) << std::endl;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::WriteNumberOfEntities(std::string iNameOfEntity, size_t iNumber)
{
  this->AddTabulation();
  std::string NameToWrite = "NumberOf";
  NameToWrite += iNameOfEntity;
  this->m_outfile << this->GetNameWithBrackets(NameToWrite);
  this->m_outfile << iNumber;
  this->m_outfile << this->GetNameWithSlashBrackets(NameToWrite) << std::endl;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string GoDBExport::GetNameWithBrackets(std::string iName)
{
  std::stringstream NameWithBrackets;

  NameWithBrackets << "<";
  NameWithBrackets << iName;
  NameWithBrackets << ">";
  return NameWithBrackets.str();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string GoDBExport::GetNameWithSlashBrackets(std::string iName)
{
  std::stringstream NameWithBrackets;

  NameWithBrackets << "</";
  NameWithBrackets << iName;
  NameWithBrackets << ">";
  return NameWithBrackets.str();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::AddTabulation()
{
  this->m_outfile << "  ";
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::OpenDBConnection()
{
  this->m_DatabaseConnector = OpenDatabaseConnection(m_ServerName,
                                                     m_Login, m_Password, "gofiguredatabase");
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::CloseDBConnection()
{
  CloseDatabaseConnection(m_DatabaseConnector);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBExport::GetVectorsTableNamesTracesIDsAndFields(
  std::vector< std::string > & ioVectorTableNames,
  std::vector< std::vector< std::string > > & ioVectorTracesIDs,
  std::vector< std::string > & ioVectorFields, bool IncludeChannelIDs)
{
  if ( !this->m_VectorContourIDs.empty() )
    {
    ioVectorTableNames.push_back("contour");
    ioVectorFields.push_back("contourID");
    ioVectorTracesIDs.push_back(this->m_VectorContourIDs);
    }
  if ( !this->m_VectorMeshIDs.empty() )
    {
    ioVectorTableNames.push_back("mesh");
    ioVectorFields.push_back("meshID");
    ioVectorTracesIDs.push_back(this->m_VectorMeshIDs);
    }
  if ( !this->m_VectorTrackIDs.empty() )
    {
    ioVectorTableNames.push_back("track");
    ioVectorFields.push_back("trackID");
    ioVectorTracesIDs.push_back(this->m_VectorTrackIDs);
    }
  if ( !this->m_VectorLineageIDs.empty() )
    {
    ioVectorTableNames.push_back("lineage");
    ioVectorFields.push_back("lineageID");
    ioVectorTracesIDs.push_back(this->m_VectorLineageIDs);
    }
  if ( IncludeChannelIDs )
    {
    if ( !this->m_VectorChannelIDs.empty() )
      {
      ioVectorTableNames.push_back("channel");
      ioVectorFields.push_back("ChannelID");
      ioVectorTracesIDs.push_back(this->m_VectorChannelIDs);
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
