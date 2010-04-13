/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Rev: 1150 $  // Revision of last commit
  Date: $Date: 2010-03-30 15:00:47 -0400 (Tue, 30 Mar 2010) $  // Date of last commit
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

#ifndef __QGoDBEntityManager_h
#define __QGoDBEntityManager_h

#include <QDialog>
#include <QWidget>
#include <QTextEdit>
#include <QMenu>
#include "vtkMySQLDatabase.h"
#include "QNameDescriptionInputDialog.h"
#include "GoDBCoordinateRow.h"

class QGoDBEntityManager:
  public QWidget
{
  Q_OBJECT

  public:
    explicit QGoDBEntityManager (QWidget* iParent = 0,
      std::string iEntityName = "", int iImgSessionID = 0);
    
    ~QGoDBEntityManager();

    typedef std::vector<std::pair<std::string,std::string> > 
      NamesDescrContainerType;
    /** \brief execute the dialog asking the user to enter a name and a
    description, validates the name, set the m_DatabaseConnectorForNewBkmrk
    and save the entity in the DB*/
    void AddAnEntity(vtkMySQLDatabase* iDatabaseConnector);

    /** \brief return the list of all the existing entities stored 
    in the database*/
    NamesDescrContainerType GetListExistingEntities(
      vtkMySQLDatabase* iDatabaseConnector);
   /** \brief show the list of the existing entities so the user can
   choose the ones he wants to delete, then delete them from the 
   database*/
    void DeleteEntity(vtkMySQLDatabase* iDatabaseConnector);
    std::string GetNameNewEntity();

  protected:
    int                          m_ImgSessionID;
    std::string                  m_EntityName;
    int                          m_CoordIDForNewEntity;
    QNameDescriptionInputDialog* m_NameDescDialog;
    vtkMySQLDatabase*            m_DatabaseConnectorForNewEntity;
    std::string                  m_NameNewEntity;

  protected slots:
    /** \brief save the new bookmark in the database, the 
    m_DatabaseConnectorForNewBkmrk needs to be set before
    calling this method. Check that the bookmark doesn't 
    already exits in the database, if so, give the user
    the name of the existing bookmark*/
    virtual void SaveNewEntityInDB()= 0;

    /** \brief check that the name doesn't already exists in the 
    database, if so, make the m_NameDescDialog asks the user to
    choose another one, if no, close the m_NameDescDialog and 
    call SaveNewBookmarkInDB()*/
    virtual void ValidateName()= 0;

signals:
    void ListEntitiesChanged();

};
#endif