/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 1902 $  // Revision of last commit
  Date: $Date: 2010-08-05 15:24:47 -0400 (Thu, 05 Aug 2010) $  // Date of last commit
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
#include "QGoDBNameDescEntityManager.h"
#include "ConvertToStringHelper.h"
#include "SelectQueryDatabaseHelper.h"
#include "QueryDatabaseHelper.h"
#include "QGoDeleteFromListDialog.h"

QGoDBNameDescEntityManager::QGoDBNameDescEntityManager(QWidget* iParent,
                                       std::string iEntityName, int iImgSessionID) :
  QWidget(iParent)
  {
  this->m_EntityName = iEntityName;
  this->m_ImgSessionID = iImgSessionID;
  this->m_NameNewEntity = "";
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDBNameDescEntityManager::~QGoDBNameDescEntityManager()
  {
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoDBNameDescEntityManager::AddAnEntity(
  vtkMySQLDatabase* iDatabaseConnector)
{
 this->m_NameDescDialog = new QGoNameDescriptionInputDialog(
    this, this->m_EntityName.c_str());

  this->m_DatabaseConnector = iDatabaseConnector;
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
QGoDBNameDescEntityManager::NamesDescrContainerType 
QGoDBNameDescEntityManager::GetListExistingEntities(vtkMySQLDatabase* iDatabaseConnector)
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
bool QGoDBNameDescEntityManager::DeleteEntity(
  vtkMySQLDatabase* iDatabaseConnector)
{
  QGoDeleteFromListDialog* Dialog = new QGoDeleteFromListDialog(
    this->GetNameExistingEntities(iDatabaseConnector),
    this, this->m_EntityName);
  
  this->m_DatabaseConnector = iDatabaseConnector;
  QObject::connect(Dialog,
                   SIGNAL(ListEntitiesToDelete(std::vector<std::string>)),
                   this,
                   SLOT(DeleteEntitiesFromList(std::vector<std::string>))
                   );
  Dialog->show();
  return Dialog->exec();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBNameDescEntityManager::DeleteEntitiesFromList(
    std::vector<std::string> iVectorNamesEntitiesToDelete)
{
  if (this->m_DatabaseConnector != 0)
    {
    DeleteRows(this->m_DatabaseConnector, this->m_EntityName,
               "Name", iVectorNamesEntitiesToDelete);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoDBNameDescEntityManager::GetNameNewEntity()
{
  return this->m_NameNewEntity;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<std::string> QGoDBNameDescEntityManager::
GetNameExistingEntities(vtkMySQLDatabase* iDatabaseConnector)
{
  std::vector<std::string> ResultsQuery;
  if (iDatabaseConnector)
    {
    // if m_ImgSessionID = 0, the entity name is not related to an imagingsession
    if (this->m_ImgSessionID != 0)
      {
      ResultsQuery = ListSpecificValuesForOneColumn(
        iDatabaseConnector, this->m_EntityName, "Name", "ImagingSessionID",
        ConvertToString<int>(this->m_ImgSessionID), "Name");
      }
    else
      {
      ResultsQuery = ListAllValuesForOneColumn(iDatabaseConnector,
                                               "Name", this->m_EntityName);
      }
    }
  return ResultsQuery;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoDBNameDescEntityManager::GetTheEntityID(std::string iName,
                                               vtkMySQLDatabase* iDatabaseConnector)
{
  std::string EntityNameID = this->m_EntityName;
  EntityNameID += "ID";
  return FindOneID(iDatabaseConnector,
                   this->m_EntityName, EntityNameID, "Name", iName);
}