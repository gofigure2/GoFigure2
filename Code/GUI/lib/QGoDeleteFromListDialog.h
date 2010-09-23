/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 1910 $  // Revision of last commit
  Date: $Date: 2010-08-06 18:43:09 -0400 (Fri, 06 Aug 2010) $  // Date of last commit
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
class QGOGUILIB_EXPORT QGoDeleteFromListDialog : public QDialog
  {
  Q_OBJECT

public:

  typedef std::pair<std::string,QColor> ItemColorComboboxData;
  
  explicit QGoDeleteFromListDialog(std::vector<std::string> iVectorEntities,
                                   QWidget* iParent = 0,     
                                   std::string iEntityName = "");

  explicit QGoDeleteFromListDialog(std::list< ItemColorComboboxData > iDataListWithColor,
                                   QWidget* iParent = 0,
                                   std::string iEntityName = "");
                                   
  virtual ~QGoDeleteFromListDialog();

protected:
  std::string  m_EntityName;
  QListWidget* m_ListWidget;

  /**
  \brief set the layout with all the objects, the connections and
  the entity name 
  \param[in] iEntityName name of the entity to be deleted
  */
  void SetUpUi(std::string iEntityName);
                                    
  /** 
  \brief emit a signal which sends vector with the names of the entities
  the user selected to be deleted
  \param[in] iListEntitiesToDelete list of the items selected by the user
  */
  void DeleteSelection(
    QList<QListWidgetItem*> iListEntitiesToDelete);

  /** 
  \brief create the corresponding QListWidgetItems
  \param[in] iVectorItems vector with the names of the entities to be 
  displayed
  */  
  void SetItemsFromTheVector(std::vector<std::string> iVectorItems);
  
  /** 
  \brief create the corresponding QListWidgetItems with a QColor
  \param[in] iDataList list with the names of the entities and their color 
  to be displayed
  */ 
  void SetItemsInTheListWithColor(std::list<ItemColorComboboxData> iDataList);

protected slots:
  /** 
  \brief ask the user to select at least one item if
  nothying has been selected and ask the user confirmation
  if the selection is not null
  */
  void SelectionValidation();

signals:
  
  void CancelRequested();
  void ListEntitiesToDelete(std::vector<std::string>);
  };

#endif