/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 1869 $  // Revision of last commit
  Date: $Date: 2010-07-29 11:24:36 -0400 (Thu, 29 Jul 2010) $  // Date of last commit
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

#ifndef __QGoColorComboBox_h
#define __QGoColorComboBox_h

#include <QComboBox>
#include <QtGui>
#include "QGoGUILibConfigure.h"

class QGOGUILIB_EXPORT QGoColorComboBox : public QComboBox
  {
  Q_OBJECT
public:
  explicit QGoColorComboBox(std::string iTextToAddANewOne,
      QWidget *parent = 0,std::string iTextToDelete = "");
  virtual ~QGoColorComboBox();

  typedef std::pair<std::string, QColor> ItemColorComboboxData;

signals:
  void AddANewOneActivated();
  void ItemSelected(ItemColorComboboxData);
  void DeleteActivated();
  
public slots:
  /** \brief add an item with color at the end of the list befor the "add new..."
  if they have already been added to the list and select it if 
  selectetheaddeditem is set to true. */
  void AddItemWithColor(ItemColorComboboxData,bool SelectTheAddedItem = true);

  void setItemsWithColorFromList(std::list< ItemColorComboboxData > iDataFromList);
  //void ListToUpdateWithItemDeleted(std::list< ItemColorComboboxData > iDataFromList);
  
protected:
  ItemColorComboboxData GetTheItemColorComboBoxData(int iIndex);

  virtual     void SetActivatedItem();


protected slots:
  virtual void ActionWhenNewOneRequested() = 0;

private slots:
  void emitActivatedItem(int iIndexActivatedItem);

private:
  std::string m_TextToAddANewOne;
  std::string m_TextToDelete;
  int         m_NumberOfItemsAfterList;

  };

#endif

