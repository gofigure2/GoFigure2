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
#ifndef __QGoDeleteDBEntityDialog_h
#define __QGoDeleteDBEntityDialog_h

#include <QDialog>
#include <QStringList>
#include <QListWidget>
#include <QListWidgetItem>
#include "vtkMySQLDatabase.h"

/**
\class QGoDeleteDBEntityDialog
\brief this class displays the corresponding entities
stored in the database.
*/
class QGoDeleteDBEntityDialog : public QDialog
{
  Q_OBJECT

public:
  explicit QGoDeleteDBEntityDialog(QWidget* iParent = 0,
    std::string iEntityName = "", int iImgSessionID = 0,
    vtkMySQLDatabase* iDatabaseConnector = 0);
  virtual ~QGoDeleteDBEntityDialog();

protected:
  std::string       m_EntityName;
  int               m_ImgSessionID;
  QListWidget*      m_ListWidget;
  /** \brief Get the list of the existing entities names from
  the database*/
  QStringList GetListExistingEntities(
    vtkMySQLDatabase* iDatabaseConnector);
  vtkMySQLDatabase* m_DatabaseConnector;
  /** \brief delete the corresponding items selected in the
  database*/
  void DeleteSelection(
    QList<QListWidgetItem*> iListEntitiesToDelete );

  /** \brief get the names of the entities from the database
  and create the corresponding QListWidgetItems*/
  void SetItemsInTheList(vtkMySQLDatabase* iDatabaseConnector);

protected slots:
  /** \brief ask the user to select at least one item if 
  nothying has been selected and ask the user confirmation 
  if selection is not null*/
  void SelectionValidation();
signals:
  void ListEntitiesChanged();
};

#endif

