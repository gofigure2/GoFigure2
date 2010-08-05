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

#ifndef __QGoDBNameDescEntityManager_h
#define __QGoDBNameDescEntityManager_h

#include <QWidget>
#include <QMessageBox>
#include "vtkMySQLDatabase.h"
#include "QNameDescriptionInputDialog.h"

/**
\class QGoDBEntityManager
\brief the QGoDBNameDescEntityManager manages the interactions between the user and the database
for all the DBtables containing a name and a description.
*/
class QGoDBNameDescEntityManager :
  public QWidget
  {
  Q_OBJECT

public:
  explicit QGoDBNameDescEntityManager (QWidget* iParent = 0,
                               std::string iEntityName = "", int iImgSessionID = 0);

  ~QGoDBNameDescEntityManager();

  typedef std::vector<std::pair<std::string, std::string> >
  NamesDescrContainerType;

  /** \brief execute the dialog asking the user to enter a name and a
  description, validates the name, set the m_DatabaseConnectorForNewBkmrk,
  save the entity in the DB and return the name of the new entity*/
  std::string AddAnEntity(vtkMySQLDatabase* iDatabaseConnector);

  /** \brief return the list of all the existing entities stored
  in the database*/
  NamesDescrContainerType GetListExistingEntities(
    vtkMySQLDatabase* iDatabaseConnector);

  /** \brief show the list of the existing entities so the user can
  choose the ones he wants to delete, then delete them from the
  database*/
  bool DeleteEntity(vtkMySQLDatabase* iDatabaseConnector);
  std::string GetNameNewEntity();

protected slots:

  /** \brief check that the name doesn't already exists in the
  database, if so, make the m_NameDescDialog asks the user to
  choose another one, if no, close the m_NameDescDialog and
  call SaveNewEntityInDB()*/
  virtual void ValidateName(std::string,std::string) = 0;

protected:
  int                          m_ImgSessionID;
  std::string                  m_EntityName;
  QNameDescriptionInputDialog* m_NameDescDialog;
  vtkMySQLDatabase*            m_DatabaseConnectorForNewEntity;
  std::string                  m_NameNewEntity;
  
  /** \brief save the new entity in the database, the
  m_DatabaseConnectorForNewEntity needs to be set before
  calling this method. Check that the entity doesn't
  already exits in the database, if so, give the user
  the name of the existing entity*/
  virtual void SaveNewEntityInDB(std::string iName,
    std::string iDescription) = 0;
  
  /** \brief check if the entity already exists in the database,
  if yes, give the name of the existing entity to the user, if
  no, record the name iName as the name of the new entity*/
  template <typename T>
  bool CheckEntityAlreadyExists(T &iNewEntity,std::string iName,
    std::string iDescription)
  {
    iNewEntity.SetField("Description",iDescription);
    if (iNewEntity.DoesThisEntityAlreadyExists(
        this->m_DatabaseConnectorForNewEntity, iName) != -1)
      {
      QMessageBox msgBox;
      msgBox.setText(
        tr("This %1 already exists, its name is: ' %2 ' ")
        .arg(this->m_EntityName.c_str())
        .arg(iName.c_str()));
      msgBox.exec();
      return true;
      }
    else
      {
      this->m_NameNewEntity =iName;
      return false;
      }
  }

  /** \brief check if the name already exists for this entity in the database, if
  yes, let the user know, if not, call SaveNewEntityInDB*/
  template< typename T>
  void ValidateNameTemplate(T &iNewEntity, std::string iName, std::string iDescription)
  {
  iNewEntity.SetField("Name",iName);
  if (iNewEntity.DoesThisNameAlreadyExists(
    this->m_DatabaseConnectorForNewEntity) != -1)
    {
    this->m_NameDescDialog->NameAlreadyExists();
    }
  else
    {
    this->m_NameDescDialog->accept();
    this->SaveNewEntityInDB(iName,iDescription);
    }
  }
  
  };
#endif
