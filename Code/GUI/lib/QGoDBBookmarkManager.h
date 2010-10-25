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

#ifndef __QGoDBBookmarkManager_h
#define __QGoDBBookmarkManager_h

#include "GoDBCoordinateRow.h"
#include "GoDBBookmarkRow.h"

#include "QGoGUILibConfigure.h"
#include "QGoDBNameDescEntityManager.h"

/**
\class QGoDBBookmarkManager
\brief the QGoDBBookmarkManager manages the interactions between the user and the database
for the Bookmark DBTable.
\ingroup DB GUI
*/
class QGOGUILIB_EXPORT QGoDBBookmarkManager:
  public QGoDBNameDescEntityManager
{
  Q_OBJECT
public:
  explicit QGoDBBookmarkManager(QWidget *iParent = 0,
                                int iImgSessionID = 0);

  ~QGoDBBookmarkManager();

  /**
  \brief execute the dialog asking the user to enter a name and a
  description, validates the name, set the m_DatabaseConnectorForNewBkmrk
  and save the bookmark in the DB
  \param[in] iCoordID ID for the coordinate to be saved in the bookmark table
  */
  void AddABookmark(int iCoordID, vtkMySQLDatabase *iDatabaseConnector);

  /**
  \brief return the coordinate for the bookmark with the name iName
  \param[in] iName name of the bookmark for which we want to get the coordinate
  \param[out] GoDBCoordinateRow containing the data for the coordinate which name is iName
  */
  GoDBCoordinateRow GetCoordinatesForBookmark(
    vtkMySQLDatabase *iDatabaseConnector, std::string iName);

  /** \brief delete the bookmarks from the database from a list the user
  selects and send a signal to tell that the list has changed*/
  void DeleteBookmark(vtkMySQLDatabase *iDatabaseConnector);

protected:
  GoDBBookmarkRow m_NewBookmark;
  int             m_CoordIDForNewBookmark;
protected slots:
  //mother class method
  void SaveNewEntityInDB();

  /**
  \brief get the coordid for the bookmark with the name
  iName
  \param[in] iName Name of the bookmark
  \param[out] int ID for the coordinate in the bookmark DBTable
  */
  int GetCoordIDForBookmark(vtkMySQLDatabase *iDatabaseConnector,
                            std::string iName);

  //mother class method
  void ValidateName(std::string iName, std::string iDescription);

signals:
  void ListBookmarksChanged();
};
#endif
