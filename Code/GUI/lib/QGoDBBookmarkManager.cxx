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

#include "QGoDBBookmarkManager.h"
#include <QDateTime>
#include "SelectQueryDatabaseHelper.h"

QGoDBBookmarkManager::QGoDBBookmarkManager(QWidget* iParent,
                                           int iImgSessionID) :
  QGoDBNameDescEntityManager(iParent,"bookmark",iImgSessionID)
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDBBookmarkManager::~QGoDBBookmarkManager()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBBookmarkManager::AddABookmark(int iCoordID,
                                        vtkMySQLDatabase* iDatabaseConnector)
{
  this->m_CoordIDForNewBookmark = iCoordID;
  this->AddAnEntity(iDatabaseConnector);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBBookmarkManager::SaveNewEntityInDB()
{
  QDateTime   CreationDate = QDateTime::currentDateTime();
  std::string CreationDateStr =
    CreationDate.toString(Qt::ISODate).toStdString();
  this->m_NewBookmark.SetField("CreationDate", CreationDateStr);
  this->m_NewBookmark.SetField<int>("CoordID", this->m_CoordIDForNewBookmark);
  this->m_NewBookmark.SetField<int>("ImagingSessionID", this->m_ImgSessionID);

  if(!this->CheckEntityAlreadyExists<GoDBBookmarkRow>(this->m_NewBookmark))
    {
    this->m_NewBookmark.SaveInDB(this->m_DatabaseConnectorForNewEntity);
      emit ListBookmarksChanged();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBCoordinateRow QGoDBBookmarkManager::GetCoordinatesForBookmark(
  vtkMySQLDatabase* iDatabaseConnector, std::string iName)
{
  GoDBCoordinateRow BookmarkCoord;
  BookmarkCoord.SetValuesForSpecificID(
    this->GetCoordIDForBookmark(iDatabaseConnector, iName), iDatabaseConnector);
  return BookmarkCoord;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoDBBookmarkManager::GetCoordIDForBookmark(
  vtkMySQLDatabase* iDatabaseConnector, std::string iName)
{
  return FindOneID(iDatabaseConnector, "bookmark",
                   "CoordID", "Name", iName);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBBookmarkManager::ValidateName(std::string iName, 
                                        std::string iDescription)
{
  this->ValidateNameTemplate<GoDBBookmarkRow>(this->m_NewBookmark,
    iName,iDescription);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBBookmarkManager::DeleteBookmark(
  vtkMySQLDatabase* iDatabaseConnector)
{
  bool ok = this->DeleteEntity(iDatabaseConnector);
  if (ok)
    {
    emit ListBookmarksChanged();//to update the menu for bookmarks;
    }

}
