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

#ifndef __QGoComboBox_h
#define __QGoComboBox_h

#include <QComboBox>
#include <QtGui>
#include "QGoGUILibConfigure.h"

/**
\class QGoComboBox
\brief inherits from Qt QCombobox but add a the end of the list of items, 1 or 2 items:
the first one to add new items and the second one to delete them
*/
class QGOGUILIB_EXPORT QGoComboBox : public QComboBox
  {
  Q_OBJECT
public:
  /**
  \brief if the string iTextToDelete is empty, there will be only the add
  a new item at the end of the list
  */
  explicit QGoComboBox(std::string iTextToAddANewOne,
      QWidget *parent = 0,std::string iTextToDelete = "");
  virtual ~QGoComboBox();

signals:
  void AddANewOneActivated();
  void ItemSelected(std::string);
  void DeleteActivated();
  
public slots:
  /**
  \brief call the method SetItemsFromList and send a signal with the current index.
  \param[in] iListItems contains the names of the items to be displayed in the combobox
  */
  virtual void InitializeTheList(QStringList iListItems);
  /**
  \brief clear the items already in the combobox,displayed the one in the QStringList and
  the items to add/delete
  \param[in] iListItems contains the names of the items to be displayed in the combobox
  */
  virtual void SetItemsFromList(QStringList iDataFromList);

  /**
  \brief set the activated item corresponding to the iTemText (no need to emit the signal
  ItemSelected)
  \parma[in] iTemText name of the item to be set to activated in the combobox
  */
  void SetCurrentItem(std::string iItemText);
  
protected:
  std::string m_TextToAddANewOne;
  std::string m_TextToDelete;
  int         m_NumberOfItemsAfterList;

  /**
  \brief Add the "Add a new one..." and "Delete..." text items at the end of the items list
  */
  void AddItemsEndOfList();

protected slots:
  /**
  \brief check which item has been clicked and emit the corresponding signal: addanewone,
  deleteactivated or itemselected
  \parma[in] iIndexActivatedItem index of the clicked item
  */
  void CheckUserAction(int iIndexActivatedItem);

  /**
  \brief call the signal to send the index of the activated item.
  \param[in] iIndexActivatedItem index of the activated item
  */
  virtual void EmitActivatedItem(int iIndexActivatedItem);
  
  };

#endif

