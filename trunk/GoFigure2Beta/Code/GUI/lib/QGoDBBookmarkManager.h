/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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

#ifndef __QGoDBBookmarkManager_h
#define __QGoDBBookmarkManager_h

#include <QDialog>
#include <QWidget>
#include <QTextEdit>
#include <QMenu>
#include "vtkMySQLDatabase.h"
#include "QNameDescriptionInputDialog.h"
#include "GoDBCoordinateRow.h"

class QGoDBBookmarkManager:
  public QWidget
{
  Q_OBJECT

  public:
    explicit QGoDBBookmarkManager (QWidget* iParent = 0,
      int iImgSessionID = 0);
    
    ~QGoDBBookmarkManager();

    typedef std::vector<std::pair<std::string,std::string> > 
      NamesDescrContainerType;
    /** \brief execute the dialog asking the user to enter a name and a
    description, validates the name, set the m_DatabaseConnectorForNewBkmrk
    and save the bookmark in the DB*/
    void AddABookmark(int iCoordID, vtkMySQLDatabase* iDatabaseConnector);
    /** \brief return the list of existing bookmarks for the imagingsession
    stored in the database*/
    NamesDescrContainerType GetListExistingBookmarks(
      vtkMySQLDatabase* iDatabaseConnector);
    /** \brief return the coordinate for the bookmark with the name iName*/
    GoDBCoordinateRow GetCoordinatesForBookmark(
      vtkMySQLDatabase* iDatabaseConnector,std::string iName); 

    void DeleteBookmark(vtkMySQLDatabase* iDatabaseConnector);

  protected:
    int                          m_ImgSessionID;
    int                          m_CoordIDForNewBookmark;
    QNameDescriptionInputDialog* m_NameDescDialog;
    vtkMySQLDatabase*            m_DatabaseConnectorForNewBkmrk;
    
     bool DoesThisBookmarkNameAlreadyExistsInTheDB(
      vtkMySQLDatabase* DatabaseConnector,std::string iName);

  protected slots:
    /** \brief save the new bookmark in the database, the 
    m_DatabaseConnectorForNewBkmrk needs to be set before
    calling this method. Check that the bookmark doesn't 
    already exits in the database, if so, give the user
    the name of the existing bookmark*/
    void SaveNewBookmarkInDB();

    int GetCoordIDForBookmark(vtkMySQLDatabase* iDatabaseConnector,
      std::string iName);

    /** \brief check that the name doesn't already exists in the 
    database, if so, make the m_NameDescDialog asks the user to
    choose another one, if no, close the m_NameDescDialog and 
    call SaveNewBookmarkInDB()*/
    void ValidateName(std::string iName);

signals:
    void ListBookmarksChanged();

};
#endif