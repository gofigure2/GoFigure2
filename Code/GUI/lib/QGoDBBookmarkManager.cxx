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
#include "GoDBBookmarkRow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QFormLayout>
#include <QTextEdit>
#include "QueryDataBaseHelper.h"
#include "QNameDescriptionInputDialog.h"
#include "SelectQueryDatabaseHelper.h"
#include "QGoDeleteDBEntityDialog.h"

QGoDBBookmarkManager::QGoDBBookmarkManager(QWidget* iParent,
                                           int iImgSessionID) :
  QGoDBEntityManager(iParent,"bookmark",iImgSessionID)
  {
  //this->m_ImgSessionID = iImgSessionID;
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
 /* this->m_NameDescDialog = new QNameDescriptionInputDialog(
    this, "Bookmark");
  this->m_DatabaseConnectorForNewBkmrk = iDatabaseConnector;
  QObject::connect (this->m_NameDescDialog, 
                    SIGNAL(NewNameDescription(std::string, std::string)),
                    this, SLOT(ValidateName(std::string,std::string)));
  this->m_NameDescDialog->exec();*/
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBBookmarkManager::SaveNewEntityInDB(std::string iName, 
                                               std::string iDescription)
{
  //NewBookmark.SetField("Name", this->m_NameDescDialog->GetInputTextForName());
  //NewBookmark.SetField("Description",
                     //  this->m_NameDescDialog->GetInputTextForDescription());
  //this->m_NewBookmark.SetField("Description", iDescription);
  QDateTime   CreationDate = QDateTime::currentDateTime();
  std::string CreationDateStr =
    CreationDate.toString(Qt::ISODate).toStdString();
  this->m_NewBookmark.SetField("CreationDate", CreationDateStr);
  this->m_NewBookmark.SetField<int>("CoordID", this->m_CoordIDForNewBookmark);
  this->m_NewBookmark.SetField<int>("ImagingSessionID", this->m_ImgSessionID);

  /*if (this->m_NewBookmark.DoesThisEntityAlreadyExists(
    this->m_DatabaseConnectorForNewEntity, iName) != -1)
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("This bookmark already exists, its name is: ' %1 ' ").arg(iName.c_str()));
    msgBox.exec();
    }
  else
    {
    this->m_NewBookmark.SaveInDB(this->m_DatabaseConnectorForNewEntity);
    emit ListBookmarksChanged();
    }*/
  if(!this->CheckEntityAlreadyExists<GoDBBookmarkRow>(
    this->m_NewBookmark,iName,iDescription))
    {
    this->m_NewBookmark.SaveInDB(this->m_DatabaseConnectorForNewEntity);
      emit ListBookmarksChanged();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*QGoDBBookmarkManager::NamesDescrContainerType QGoDBBookmarkManager::
GetListExistingBookmarks(vtkMySQLDatabase* iDatabaseConnector)
{
  // return ListSpecificValuesForOneColumn(iDatabaseConnector,"bookmark",
  //   "Name","ImagingSessionID",ConvertToString<int>(this->m_ImgSessionID));
  return ListSpecificValuesForTwoColumns(iDatabaseConnector, "bookmark",
                                         "Name", "Description", "ImagingSessionID",
                                         ConvertToString<int>(this->m_ImgSessionID), "Name");
}*/
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
/*bool QGoDBBookmarkManager::DoesThisBookmarkNameAlreadyExistsInTheDB(
  vtkMySQLDatabase* DatabaseConnector, std::string iName)
{
  int ID = FindOneID(DatabaseConnector, "bookmark", "BookmarkID",
                     "Name", iName);
  if (ID == -1)
    {
    return false;
    }
  return true;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBBookmarkManager::ValidateName(std::string iName, 
                                        std::string iDescription)
{
  this->ValidateNameTemplate<GoDBBookmarkRow>(this->m_NewBookmark,
    iName,iDescription);
  /*this->m_NewBookmark.SetField("Name",iName);
  if (this->m_NewBookmark.DoesThisNameAlreadyExists(
    this->m_DatabaseConnectorForNewEntity) != -1)
       // this->m_NameDescDialog->GetInputTextForName()))
       //iName))
    {
    this->m_NameDescDialog->NameAlreadyExists();
    }
  else
    {
    this->m_NameDescDialog->accept();
    this->SaveNewEntityInDB(iName,iDescription);
    }*/
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBBookmarkManager::DeleteBookmark(
  vtkMySQLDatabase* iDatabaseConnector)
{
 /* QGoDeleteDBEntityDialog* Dialog = new QGoDeleteDBEntityDialog(
    this, "bookmark", this->m_ImgSessionID, iDatabaseConnector);
  QObject::connect(Dialog, SIGNAL(ListEntitiesChanged()),
                   this, SIGNAL(ListBookmarksChanged()));
  Dialog->show();
  Dialog->exec();*/
  bool ok = QGoDBEntityManager::DeleteEntity(iDatabaseConnector);
  if (ok)
    {
    emit ListBookmarksChanged();//to update the menu for bookmarks;
    }

}
