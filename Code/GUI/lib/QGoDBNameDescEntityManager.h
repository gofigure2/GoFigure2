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
#include "QGoNameDescriptionInputDialog.h"

/**
\class QGoDBNameDescEntityManager
\brief Abstract class : the QGoDBNameDescEntityManager manages the interactions
between the user and the database (add a new one, delete)for a DBTable
containing a name and a description.Has some Qt widgets and manages queries for
this particular DBTable (i.e get list of existing ones).
\ingroup DB GUI
*/
class QGoDBNameDescEntityManager:
  public QWidget
{
  Q_OBJECT
public:
  /**
  \param[in] iParent QWidget parent
  \param[in] iEntityName Name of the DBTable
  \param[in] iImgSessionID ID of the current imagingsession
  */
  explicit QGoDBNameDescEntityManager (QWidget *iParent = 0,
                                       std::string iEntityName = "", int iImgSessionID = 0);

  ~QGoDBNameDescEntityManager();

  typedef std::vector< std::pair< std::string, std::string > >
  NamesDescrContainerType;

  /**
  \brief execute the dialog asking the user to enter a name and a
  description, validates the name, set the m_DatabaseConnector,
  save the entity in the DB and return the name of the new entity
  \param[in] iDatabaseConnector connection to the database
  \return Name of the new entity, empty if the user canceled the adding
  */
  std::string AddAnEntity(vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief return the list of all the existing entities stored
  in the database
  \param[in] iDatabaseConnector connection to the database
  \return a vector of all the names associated with their description
  */
  NamesDescrContainerType GetListExistingEntities(
    vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief show the list of the existing entities so the user can
  choose the ones he wants to delete, then delete them from the
  database
  \param[in] iDatabaseConnector connection to the database
  \return true if the user chooses to delete an entity,
  false if he canceled the deleting
  */
  virtual bool DeleteEntity(vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief return the name of the new entity added
  \return name of the new entity
  */
  std::string GetNameNewEntity();

  /**
  \brief get the ID of the entity based on the name
  \param[in] iDatabaseConnector connection to the database
  \param[in] iName name of the entity
  \return ID of the entity
  */
  int GetTheEntityID(std::string iName, vtkMySQLDatabase *iDatabaseConnector);

protected slots:

  /**
  \brief Pure Virtual method : check that the name doesn't already exists in the
  database, if so, make the m_NameDescDialog asks the user to
  choose another one, if no, close the m_NameDescDialog and
  call SaveNewEntityInDB()
  \param[in] iName name entered by the user
  \param[in] iDescription description entered by the user
  */
  virtual void ValidateName(
    std::string iName, std::string iDescription) = 0;

  /**
  \brief Delete in the database the entities with the names contained in
  the vector
  \param[in] iVectorNamesEntitiesToDelete vector of the names for the
  entities to be deleted in the database
  */
  void DeleteEntitiesFromList(
    std::vector< std::string > iVectorNamesEntitiesToDelete);

protected:
  int                            m_ImgSessionID;
  std::string                    m_EntityName;
  QGoNameDescriptionInputDialog *m_NameDescDialog;
  vtkMySQLDatabase *             m_DatabaseConnector;
  std::string                    m_NameNewEntity;

  /**
  \brief Pure Virtual method : save the new entity in the database, the
  m_DatabaseConnectorForNewEntity needs to be set before
  calling this method. Check that the entity doesn't
  already exists in the database, if so, give the user
  the name of the existing entity
  */
  virtual void SaveNewEntityInDB() = 0;

  /**
  \brief return the names of all the entities stored in the database
  \param[in] iDatabaseConnector connection to the database
  \return a vector of the names
  */
  std::vector< std::string > GetNameExistingEntities(vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief check if the entity already exists in the database,
  if yes, give the name of the existing entity to the user, if
  no, record the name iName as the name of the new entity
  \param[in] iNewEntity contains data to check if it already exists
  \return bool return true if it already exists, false if not
  \tparam T this method takes only children of GoDBNameDescRow as type
  */
  template< typename T >
  bool CheckEntityAlreadyExists(T iNewEntity)
  {
    std::string Name = iNewEntity.GetMapValue("Name");

    if ( iNewEntity.DoesThisEntityAlreadyExistsAndReturnName(
           this->m_DatabaseConnector, Name) != -1 )
      {
      QMessageBox msgBox;
      msgBox.setText(
        tr("This %1 already exists, its name is: ' %2 ' ")
        .arg( this->m_EntityName.c_str() )
        .arg( Name.c_str() ) );
      msgBox.exec();
      return true;
      }
    else
      {
      this->m_NameNewEntity = Name;
      return false;
      }
  }

  /**
  \brief check if the name already exists for this entity in the database, if
  yes, let the user know, if not, call SaveNewEntityInDB
  \tparam T this method takes only children of GoDBNameDescRow as type
  \param[in] iName name for the new entity entered by the user
  \param[in] iDescription description for the new entity entered by the user
  \param[in,out] ioNewEntity fields name and description modified by the method
  */
  template< typename T >
  void ValidateNameTemplate(T & ioNewEntity, std::string iName, std::string iDescription)
  {
    ioNewEntity.SetField("Name", iName);
    ioNewEntity.SetField("Description", iDescription);
    if ( ioNewEntity.DoesThisNameAlreadyExists(
           this->m_DatabaseConnector) != -1 )
      {
      this->m_NameDescDialog->NameAlreadyExists();
      }
    else
      {
      this->m_NameDescDialog->accept();
      this->SaveNewEntityInDB();
      }
  }
};
#endif
