/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
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
  this->m_MapRow["BookmarkID"] = ConvertToString<int>(0);
  this->m_MapRow["Name"] = "";
  this->m_MapRow["ImagingSessionID"] = ConvertToString<int>(0);
  this->m_MapRow["CoordID"] = ConvertToString<int>(0);
  this->m_MapRow["CreationDate"] = "";
  this->m_MapRow["Description"] = "";
  std::string NoDescription = "None";
  this->SetField("Description", NoDescription);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBBookmarkRow::DoesThisNameAlreadyExists(
  vtkMySQLDatabase* DatabaseConnector)
{
  return FindOneID(DatabaseConnector, "bookmark", "BookmarkID",
                     "ImagingSessionID", this->GetMapValue("ImagingSessionID"), "Name",
                     this->GetMapValue("Name"));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*bool GoDBBookmarkRow::DoesThisBookmarkNameAlreadyExistsInTheDB(
    vtkMySQLDatabase* DatabaseConnector)
{
  int ID = FindOneID(DatabaseConnector, "bookmark", "BookmarkID",
                     "ImagingSessionID", this->GetMapValue("ImagingSessionID"), "Name",
                     this->GetMapValue("Name"));
  if (ID == -1)
    {
    return false;
    }
  return true;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*int GoDBBookmarkRow::DoesThisBookmarkAlreadyExists(
  vtkMySQLDatabase* DatabaseConnector)
{
  int ID = FindOneID(DatabaseConnector, "bookmark", "BookmarkID",
                     "ImagingSessionID", this->GetMapValue("ImagingSessionID"), "CoordID",
                     this->GetMapValue("CoordID"));
  return ID;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBBookmarkRow::DoesThisEntityAlreadyExists(
  vtkMySQLDatabase* iDatabaseConnector)
{
  int ID = FindOneID(iDatabaseConnector, "bookmark", "BookmarkID",
                     "ImagingSessionID", this->GetMapValue("ImagingSessionID"), "CoordID",
                     this->GetMapValue("CoordID"));
  return ID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*int GoDBBookmarkRow::DoesThisBookmarkAlreadyExists(
  vtkMySQLDatabase* DatabaseConnector, std::string& ioName)
{
  int ID = FindOneID(DatabaseConnector, "bookmark", "BookmarkID",
                     "ImagingSessionID", this->GetMapValue("ImagingSessionID"), "CoordID",
                     this->GetMapValue("CoordID"));
  if (ID == -1)
    {
    return ID;
    }
  ioName = ReturnOnlyOneValue(DatabaseConnector, "bookmark", "Name",
                              "BookmarkID", ConvertToString<int>(ID));
  return ID;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*int GoDBBookmarkRow::DoesThisEntityAlreadyExists(
    vtkMySQLDatabase* iDatabaseConnector, std::string& ioName)
{
  /*int ID = FindOneID(iDatabaseConnector, "bookmark", "BookmarkID",
                     "ImagingSessionID", this->GetMapValue("ImagingSessionID"), "CoordID",
                     this->GetMapValue("CoordID"));
  int ID = this->DoesThisEntityAlreadyExists(iDatabaseConnector);
  if (ID == -1)
    {
    return ID;
    }
  ioName = ReturnOnlyOneValue(iDatabaseConnector, "bookmark", "Name",
                              "BookmarkID", ConvertToString<int>(ID));
  return ID;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBBookmarkRow::SaveInDB(vtkMySQLDatabase* iDatabaseConnector)
{
  //int BookmarkID = this->DoesThisBookmarkAlreadyExists(DatabaseConnector);
 /* int BookmarkID = this->DoesThisEntityAlreadyExists(DatabaseConnector);
  if (BookmarkID == -1)
    {
    BookmarkID = AddOnlyOneNewObjectInTable<GoDBBookmarkRow>(DatabaseConnector,
                                                             "bookmark", *this, "BookmarkID");
    }
  return BookmarkID;*/
  return this->SaveInDBTemplate<GoDBBookmarkRow>(iDatabaseConnector,*this);
}
