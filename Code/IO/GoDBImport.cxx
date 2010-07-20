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

#include "GoDBImport.h"
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
GoDBImport::GoDBImport(std::string iServerName, std::string iLogin,
                       std::string iPassword, int iImagingSessionID, std::string iFilename)
  {
  this->m_ServerName = iServerName;
  this->m_Login = iLogin;
  this->m_Password = iPassword;
  this->m_ImagingSessionID = iImagingSessionID;
  this->m_InFile.open(iFilename.c_str(), std::ifstream::in);
  }

//--------------------------------------------------------------------------
GoDBImport::~GoDBImport()
  {
  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::ImportContours()
{
  this->OpenDBConnection();
  std::map<int, int> MapColorIDs;
  std::map<int, int> MapCellTypeIDs;
  std::map<int, int> MapSubCellTypeIDs;
  std::map<int, int> MapCoordIDs;
  std::string        LineContent;

  LineContent = this->SaveNoTracesEntities(MapColorIDs, MapCellTypeIDs,
                                           MapSubCellTypeIDs, MapCoordIDs);

  this->SaveTracesEntities(MapColorIDs, MapCoordIDs, LineContent, MapCellTypeIDs,
                           MapSubCellTypeIDs);
  this->FillContourInfoForVisu(this->m_NewContourIDs);
  this->CloseDBConnection();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::ImportMeshes()
{
  this->OpenDBConnection();
  this->m_NewContourIDs.clear();
  std::map<int, int> MapColorIDs;
  std::map<int, int> MapCellTypeIDs;
  std::map<int, int> MapSubCellTypeIDs;
  std::map<int, int> MapCoordIDs;
  std::string        LineContent;

  LineContent = this->SaveNoTracesEntities(MapColorIDs, MapCellTypeIDs,
                                           MapSubCellTypeIDs, MapCoordIDs);
  this->SaveTracesEntities(MapColorIDs, MapCoordIDs, LineContent, MapCellTypeIDs,
                           MapSubCellTypeIDs, true);
  this->FillMeshInfoForVisu(this->m_NewMeshIDs);
  this->CloseDBConnection();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string GoDBImport::SaveNoTracesEntities(std::map<int, int>& ioMapColorIDs,
                                             std::map<int, int>& ioMapCellTypeIDs,
                                             std::map<int, int>& ioMapSubCellTypeIDs,
                                             std::map<int, int>& ioMapCoordIDs)
{
  std::string LineContent;
  getline(this->m_InFile, LineContent);
  while (!this->IsLineForNumberOfEntities(LineContent))
    {
    getline (this->m_InFile, LineContent);
    }
  while (this->FindFieldName(LineContent) != "NumberOflineage")
    {
    int EntitiesNumber = atoi(this->GetValueForTheLine(LineContent).c_str());
    getline(this->m_InFile, LineContent);
    //if there is nothing to be saved in the database for this group, just go
    //to the next line in the file:
    if (EntitiesNumber != 0)
      {
      if (this->GetValueForTheLine(LineContent) != "NoValueOnTheLine")
        {
        std::cout << "There was supposed to be only the name of the entity to save,the entity will not be saved";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        return LineContent;
        }
      std::string NameEntity = this->FindFieldName(LineContent);
      if (NameEntity == "color")
        {
        LineContent = this->SaveImportedEntitiesInDatabase<GoDBColorRow>(
          EntitiesNumber, ioMapColorIDs);
        }
      if (NameEntity == "celltype")
        {
        LineContent = this->SaveImportedEntitiesInDatabase<GoDBCellTypeRow>(
          EntitiesNumber, ioMapCellTypeIDs);
        }
      if (NameEntity == "subcellulartype")
        {
        LineContent = this->SaveImportedEntitiesInDatabase<GoDBSubCellTypeRow>(
          EntitiesNumber, ioMapSubCellTypeIDs);
        }
      if (NameEntity == "coordinate")
        {
        LineContent = this->SaveImportedEntitiesInDatabase<GoDBCoordinateRow>(
          EntitiesNumber, ioMapCoordIDs);
        }
      if (NameEntity != "color" && NameEntity != "celltype" &&
          NameEntity != "subcellulartype" && NameEntity != "coordinate")
        {
        std::cout << "The name of the entity doesn't correspond to any of the no traces entity";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        }
      }
    }
  return LineContent;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::SaveTracesEntities(std::map<int, int> iMapColorIDs,
                                    std::map<int, int> iMapCoordIDs, std::string iLineContent,
                                    std::map<int, int> iMapCellTypeIDs, std::map<int, int> iMapSubCellTypeIDs,
                                    bool SaveIntensities)
{
  std::map<int, int> MapContourIDs;
  std::map<int, int> MapMeshIDs;
  std::map<int, int> MapTrackIDs;
  std::map<int, int> MapLineageIDs;

  std::string LineContent = iLineContent;

  this->SaveTraces<GoDBLineageRow>(iMapColorIDs, iMapCoordIDs,
                                   MapLineageIDs, LineContent, this->m_NewLineageIDs, MapLineageIDs);
  this->SaveTraces<GoDBTrackRow>(iMapColorIDs, iMapCoordIDs, MapLineageIDs,
                                 LineContent, this->m_NewTracksIDs, MapTrackIDs);
  this->SaveTraces<GoDBMeshRow>(iMapColorIDs, iMapCoordIDs, MapTrackIDs,
                                LineContent, this->m_NewMeshIDs, MapMeshIDs, iMapCellTypeIDs,
                                iMapSubCellTypeIDs);
  if (SaveIntensities)
    {
    this->SaveIntensityForMesh(LineContent, MapMeshIDs, iMapColorIDs);
    }
  this->SaveTraces<GoDBContourRow>(iMapColorIDs, iMapCoordIDs, MapMeshIDs,
                                   LineContent, this->m_NewContourIDs, MapContourIDs);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::FillContourInfoForVisu(
  std::vector<int> iListContourIDs)
{
  for (unsigned int i = 0; i < iListContourIDs.size(); i++)
    {
    this->m_NewContourInfoForVisu.push_back(GetTraceInfoFromDB(
                                              this->m_DatabaseConnector, "contour",
                                              "mesh", iListContourIDs.at(i)));
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::FillMeshInfoForVisu(std::vector<int> iListMeshIDs)
{
  for (unsigned int i = 0; i < iListMeshIDs.size(); i++)
    {
    this->m_NewMeshInfoForVisu.push_back(GetTraceInfoFromDB(
                                           this->m_DatabaseConnector, "mesh",
                                           "track", iListMeshIDs.at(i)));
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string GoDBImport::FindFieldName(std::string iLine)
{
  size_t      BegName = iLine.find("<", 0) + 1;
  size_t      EndName = iLine.find(">", 0);
  size_t      NameLength = EndName - BegName;
  std::string oName = iLine.substr(BegName, NameLength);
  if (oName.find("/", 0) == 0)
    {
    oName = oName.substr(1);
    }
  return oName;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string GoDBImport::GetValueForTheLine(std::string iLine)
{
  size_t BegValue = iLine.find(">", 0) + 1;
  size_t EndValue = iLine.find("<", BegValue);
  if (EndValue != iLine.npos)
    {
    size_t ValueLength = EndValue - BegValue;
    return iLine.substr(BegValue, ValueLength);
    }
  return "NoValueOnTheLine";
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool GoDBImport::IsLineForNumberOfEntities(std::string iLine)
{
  size_t BegValue = iLine.find("NumberOf", 0);
  if (BegValue != iLine.npos)
    {
    return true;
    }
  return false;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::OpenDBConnection()
{
  this->m_DatabaseConnector = OpenDatabaseConnection(m_ServerName,
                                                     m_Login, m_Password, "gofiguredatabase");
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::CloseDBConnection()
{
  CloseDatabaseConnection(m_DatabaseConnector);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBImport::SaveIntensityForMesh(std::string iLineContent,
                                      std::map<int, int> iMapMeshIDs, std::map<int, int> iMapColorIDs)
{
  std::map<int, int> MapChannelIDs;
  while (this->FindFieldName(iLineContent) != "NumberOfchannel")
    {
    getline (this->m_InFile, iLineContent);
    }

  int EntitiesNumber = atoi(this->GetValueForTheLine(iLineContent).c_str());
  getline(this->m_InFile, iLineContent);
  if (EntitiesNumber != 0)
    {
    while (this->FindFieldName(iLineContent) != "NumberOfintensity")
      {
      if (this->GetValueForTheLine(iLineContent) != "NoValueOnTheLine")
        {
        std::cout << "There was supposed to be only the name of the entity to save,the entity will not be saved";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        }
      std::string NameEntity = this->FindFieldName(iLineContent);
      if (NameEntity != "channel")
        {
        std::cout << "The name of the entity should be channel but is actually different";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        }
      else
        {
        GoDBChannelRow NewChannel;
        iLineContent = this->GetValuesFromInfile<GoDBChannelRow>(NewChannel);
        this->ReplaceTheFieldWithNewIDs<GoDBChannelRow>(
          iMapColorIDs, "ColorID", NewChannel);
        int OldID = atoi(NewChannel.GetMapValue("ChannelID").c_str());
        NewChannel.SetField("ChannelID", "0");
        NewChannel.SetField("ImagingSessionID", this->m_ImagingSessionID);
        MapChannelIDs[OldID] = NewChannel.SaveInDB(this->m_DatabaseConnector);
        }
      }
    }
  EntitiesNumber = atoi(this->GetValueForTheLine(iLineContent).c_str());
  getline(this->m_InFile, iLineContent);
  if (EntitiesNumber != 0)
    {
    while (this->FindFieldName(iLineContent) != "ExportTraces")
      {
      if (this->GetValueForTheLine(iLineContent) != "NoValueOnTheLine")
        {
        std::cout << "There was supposed to be only the name of the entity to save,the entity will not be saved";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        }
      std::string NameEntity = this->FindFieldName(iLineContent);
      if (NameEntity != "intensity")
        {
        std::cout << "The name of the entity should be channel but is actually different";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        }
      else
        {
        GoDBIntensityRow NewIntensity;
        iLineContent = this->GetValuesFromInfile<GoDBIntensityRow>(NewIntensity);
        this->ReplaceTheFieldWithNewIDs<GoDBIntensityRow>(
          iMapMeshIDs, "MeshID", NewIntensity);
        this->ReplaceTheFieldWithNewIDs<GoDBIntensityRow>(
          MapChannelIDs, "ChannelID", NewIntensity);
        NewIntensity.SetField("IntensityID", "0");
        NewIntensity.SaveInDB(this->m_DatabaseConnector);
        }
      }
    }
}
