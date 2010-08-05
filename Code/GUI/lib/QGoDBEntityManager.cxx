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
std::string QGoDBEntityManager::AddAnEntity(
  vtkMySQLDatabase* iDatabaseConnector)
{
 this->m_NameDescDialog = new QNameDescriptionInputDialog(
    this, this->m_EntityName.c_str());

  this->m_DatabaseConnectorForNewEntity = iDatabaseConnector;
  QObject::connect (this->m_NameDescDialog, SIGNAL(NewNameDescription(std::string,
                    std::string)),
                    this, SLOT(ValidateName(std::string,std::string)));
  bool ok = this->m_NameDescDialog->exec();
  if (!ok)
    {
    this->m_NameNewEntity.clear();
    }
  return this->m_NameNewEntity;
}


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
bool QGoDBEntityManager::DeleteEntity(
  vtkMySQLDatabase* iDatabaseConnector)
{
  QGoDeleteDBEntityDialog* Dialog = new QGoDeleteDBEntityDialog(
    this, this->m_EntityName, this->m_ImgSessionID, iDatabaseConnector);
  Dialog->show();
  return Dialog->exec();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoDBEntityManager::GetNameNewEntity()
{
  return this->m_NameNewEntity;
}
