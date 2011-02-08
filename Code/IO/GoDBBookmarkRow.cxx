/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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
#include "GoDBBookmarkRow.h"
#include "SelectQueryDatabaseHelper.h"
#include "GoDBRecordSetHelper.h"

GoDBBookmarkRow::GoDBBookmarkRow()
{
  this->m_TableName = "bookmark";
  this->m_TableIDName = "BookmarkID";
  this->InitializeMap();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBBookmarkRow::InitializeMap()
{
  this->m_MapRow["BookmarkID"] = ConvertToString< int >(0);
  this->m_MapRow["Name"] = "";
  this->m_MapRow["ImagingSessionID"] = ConvertToString< int >(0);
  this->m_MapRow["CoordID"] = ConvertToString< int >(0);
  this->m_MapRow["CreationDate"] = "";
  this->m_MapRow["Description"] = "";
  std::string NoDescription = "None";
  this->SetField("Description", NoDescription);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBBookmarkRow::DoesThisNameAlreadyExists(
  vtkMySQLDatabase *DatabaseConnector)
{
  std::vector< FieldWithValue > Conditions;
  this->AddConditions("ImagingSessionID", Conditions);
  this->AddConditions("Name", Conditions);
  return FindOneID(DatabaseConnector, "bookmark", "BookmarkID", Conditions);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBBookmarkRow::DoesThisEntityAlreadyExists(
  vtkMySQLDatabase *iDatabaseConnector)
{
  std::vector< FieldWithValue > Conditions;
  this->AddConditions("ImagingSessionID", Conditions);
  this->AddConditions("CoordID", Conditions);
  return FindOneID(iDatabaseConnector, "bookmark", "BookmarkID", Conditions);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBBookmarkRow::SaveInDB(vtkMySQLDatabase *iDatabaseConnector)
{
  return this->SaveInDBTemplate< GoDBBookmarkRow >(iDatabaseConnector, *this);
}
