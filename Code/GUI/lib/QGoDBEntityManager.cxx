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

#include "QGoDBEntityManager.h"
#include "GoDBNameDescRow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QFormLayout>
#include <QTextEdit>
#include "QueryDataBaseHelper.h"
#include "QNameDescriptionInputDialog.h"
#include "SelectQueryDatabaseHelper.h"
#include "QGoDeleteDBEntityDialog.h"

QGoDBEntityManager::QGoDBEntityManager(QWidget* iParent,
                                       std::string iEntityName, int iImgSessionID) :
  QWidget(iParent)
  {
  this->m_EntityName = iEntityName;
  this->m_ImgSessionID = iImgSessionID;
  this->m_NameNewEntity = "";
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDBEntityManager::~QGoDBEntityManager()
  {
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBEntityManager::AddAnEntity(
  vtkMySQLDatabase* iDatabaseConnector)
{
  //this->m_CoordIDForNewEntity = iCoordID;
  this->m_NameDescDialog = new QNameDescriptionInputDialog(
    this, this->m_EntityName.c_str());
  this->m_DatabaseConnectorForNewEntity = iDatabaseConnector;
  QObject::connect (this->m_NameDescDialog, SIGNAL(NameNonEmpty()),
                    this, SLOT(ValidateName()));
  this->m_NameDescDialog->exec();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoDBEntityManager::SaveNewEntityInDB()
{
  GoDBBookmarkRow NewBookmark;
  NewBookmark.SetField("Name",this->m_NameDescDialog->GetInputTextForName());
  NewBookmark.SetField("Description",
    this->m_NameDescDialog->GetInputTextForDescription());
  QDateTime CreationDate = QDateTime::currentDateTime();
  std::string CreationDateStr =
    CreationDate.toString(Qt::ISODate).toStdString();
  NewBookmark.SetField("CreationDate",CreationDateStr);
  NewBookmark.SetField<int>("CoordID",this->m_CoordIDForNewBookmark);
  NewBookmark.SetField<int>("ImagingSessionID",this->m_ImgSessionID);

  std::string BookmarkName = NewBookmark.GetMapValue("Name");
  if (NewBookmark.DoesThisBookmarkAlreadyExists(
    this->m_DatabaseConnectorForNewBkmrk,BookmarkName) != -1)
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("This bookmark already exists, its name is: ' %1 ' ").arg(BookmarkName.c_str()));
    msgBox.exec();
    }
  else
    {
    NewBookmark.SaveInDB(this->m_DatabaseConnectorForNewBkmrk);
    emit ListBookmarksChanged();
    }
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDBEntityManager::NamesDescrContainerType QGoDBEntityManager::
GetListExistingEntities(vtkMySQLDatabase* iDatabaseConnector)
{
  if (this->m_ImgSessionID != 0)
    {
    return ListSpecificValuesForTwoColumns(iDatabaseConnector,
                                           this->m_EntityName, "Name", "Description", "ImagingSessionID",
                                           ConvertToString<int>(this->m_ImgSessionID), "Name");
    }
  else
    {
    return VectorTwoColumnsFromTable(iDatabaseConnector, "Name",
                                     "Description", this->m_EntityName, "Name");
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*int QGoDBBookmarkManager::GetCoordIDForBookmark(
  vtkMySQLDatabase* iDatabaseConnector,std::string iName)
{
  return FindOneID(iDatabaseConnector,"bookmark",
    "CoordID","Name", iName);
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*bool QGoDBEntityManager::DoesThisBookmarkNameAlreadyExistsInTheDB(
  vtkMySQLDatabase* DatabaseConnector,std::string iName)
{
  int ID = FindOneID(DatabaseConnector,"bookmark", "BookmarkID",
    "Name",iName);
  if (ID == -1)
    {
    return false;
    }
  return true;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoDBBookmarkManager::ValidateName(std::string iName)
{
  if (this->DoesThisNameAlreadyExistsInTheDB(
    this->m_DatabaseConnectorForNewBkmrk,iName))
    {
    this->m_NameDescDialog->NameAlreadyExists();
    }
  else
    {
    this->m_NameDescDialog->accept();
    this->SaveNewBookmarkInDB();
    }
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBEntityManager::DeleteEntity(
  vtkMySQLDatabase* iDatabaseConnector)
{
  QGoDeleteDBEntityDialog* Dialog = new QGoDeleteDBEntityDialog(
    this, this->m_EntityName, this->m_ImgSessionID, iDatabaseConnector);
  QObject::connect(Dialog, SIGNAL(ListEntitiesChanged()),
                   this, SIGNAL(ListEntitiesChanged()));
  Dialog->show();
  Dialog->exec();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoDBEntityManager::GetNameNewEntity()
{
  return this->m_NameNewEntity;
}
