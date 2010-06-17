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
#include "GoDBIntensityRow.h"
#include "GoDBRecordSetHelper.h"

GoDBIntensityRow::GoDBIntensityRow()
{
  this->InitializeMap();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBIntensityRow::~GoDBIntensityRow()
{}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBIntensityRow::InitializeMap()
{
  this->m_TableName = "intensity";
  this->m_TableIDName = "IntensityID";
  this->m_MapRow["IntensityID"] = ConvertToString<int>(0);
  this->m_MapRow["Value"] = ConvertToString<int>(0);
  this->m_MapRow["MeshID"] = ConvertToString<int>(0);
  this->m_MapRow["ChannelID"] = ConvertToString<int>(0);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBIntensityRow::SaveInDB(vtkMySQLDatabase* DatabaseConnector)
{
  int IntensityID = this->DoesThisIntensityAlreadyExists(DatabaseConnector);
  if (IntensityID == -1)
    {
    IntensityID = AddOnlyOneNewObjectInTable<GoDBIntensityRow>( 
      DatabaseConnector,"intensity", this, "IntensityID");
    }
  return IntensityID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBIntensityRow::DoesThisIntensityAlreadyExists(
  vtkMySQLDatabase* DatabaseConnector)
{
  std::string ChannelID = this->GetMapValue("ChannelID");
  std::string MeshID = this->GetMapValue("MeshID");
  /*return FindOneID(DatabaseConnector,"color", "ColorID","Red",Red,"Green",Green,
  "Blue",Blue,"Alpha",Alpha,"Name",Name);*/
  return FindOneID(DatabaseConnector,"intensity", "IntensityID","ChannelID", 
    ChannelID,"MeshID",MeshID);
}
