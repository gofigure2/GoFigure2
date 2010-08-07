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
#ifndef __QGoDeleteFromListDialog_h
#define __QGoDeleteFromListDialog_h

#include <QDialog>
#include <QStringList>
#include <QListWidget>
#include <QListWidgetItem>
//#include "vtkMySQLDatabase.h"

#include "QGoGUILibConfigure.h"

/**
\class QGoDeleteFromListDialog
\brief this class displays a list of entities and return the list of the 
ones selected by the user. class based on Qt
*/
class QGOGUILIB_EXPORT QGoDeleteDBEntityDialog : public QDialog
  {
  Q_OBJECT

public:

  typedef std::pair<std::string,QColor> ItemColorComboboxData;
  explicit QGoDeleteDBEntityDialog(QWidget* iParent = 0,
                                   std::string iEntityName = "", int iImgSessionID = 0,
                                   vtkMySQLDatabase* iDatabaseConnector = 0);
  explicit QGoDeleteDBEntityDialog(std::list< ItemColorComboboxData > iDataListWithColor,
                                   QWidget* iParent = 0,
                                   std::string iEntityName = "", 
                                   int iImgSessionID = 0,
                                   vtkMySQLDatabase* iDatabaseConnector = 0);
  virtual ~QGoDeleteDBEntityDialog();

protected:
  std::string  m_EntityName;
  int          m_ImgSessionID;
  QListWidget* m_ListWidget;

  void SetUpUi(std::string iEntityName, int iImgSessionID,
                                      vtkMySQLDatabase* iDatabaseConnector);
  /** \brief Get the list of the existing entities names from
  the database*/
  QStringList GetListExistingEntities(
    vtkMySQLDatabase* iDatabaseConnector);
  vtkMySQLDatabase* m_DatabaseConnector;
  /** \brief delete the corresponding items selected in the
  database*/
  void DeleteSelection(
    QList<QListWidgetItem*> iListEntitiesToDelete);

  /** \brief get the names of the entities from the database
  and create the corresponding QListWidgetItems*/
  void SetItemsInTheListFromDB(vtkMySQLDatabase* iDatabaseConnector);

  void SetItemsInTheListWithColor(std::list<ItemColorComboboxData> iDataList);

protected slots:
  /** \brief ask the user to select at least one item if
  nothying has been selected and ask the user confirmation
  if selection is not null*/
  void SelectionValidation();
signals:
  void ListEntitiesChanged();
  void CancelRequested();
  };

#endif
