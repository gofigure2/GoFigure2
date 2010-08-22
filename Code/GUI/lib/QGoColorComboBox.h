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

#include "QGoGUILibConfigure.h"
#include "QGoComboBox.h"

/**
\class QGoColorCombobox
\brief inherits from QGoCombobox, display not ony the names but also the QColor of
the items as an Icon in the combobox
\ingroup GUI
*/
class QGOGUILIB_EXPORT QGoColorComboBox : public QGoComboBox
  {
  Q_OBJECT
public:
  explicit QGoColorComboBox(std::string iTextToAddANewOne,
      QWidget *iparent = 0,std::string iTextToDelete = "");

  virtual ~QGoColorComboBox();

  typedef std::pair<std::string, QColor> ItemColorComboboxData;
  
  /**
  \brief call the method setItemsWithColorFromList and send a signal with the current index.
  \param[in] iDataFromList contains the names and the QColor of the items to be displayed
  in the combobox
  */
  void InitializeTheListWithColor(
    std::list<ItemColorComboboxData> iDataFromList);

  /**
  \brief add an item with color at the end of the list befor the "add new..."
  if they have already been added to the list and select it if 
  selectetheaddeditem is set to true. 
  \param[in] iNewItemData name and QColor of the new item to be added
  \param[in] SelectTheAddedItem if true, the new added item will be the selectedone
  in the combobox
  */
  void AddItemWithColor(ItemColorComboboxData iNewItemData,
    bool SelectTheAddedItem = true);
  
  /**
  \brief clear the items already in the combobox,displayed the one in the iDataFromList
  and the items to add/delete
  \param[in] iDataFromList contains the names and QColor of the items to be 
  displayed in the combobox
  \overload from the mother class method
  */
  void SetItemsFromListWithColor(
    std::list< ItemColorComboboxData > iDataFromList);
  
signals:
  void ItemSelected(ItemColorComboboxData);

protected:
  /**
  \brief get the name and the QColor of the item located at the index iIndex
  \param[in] iIndex index for which the name and QColor are wanted
  \return ItemColorComboboxData contains the name and QColor of the item 
  located at index iIndex
  */
  ItemColorComboboxData GetTheItemColorComboBoxData(int iIndex);


protected slots:

  /**
  \brief slot linked to the signal AddANewOneActivated()
  */
  virtual void ActionWhenNewOneRequested() = 0;

  //mother class method
  virtual void EmitActivatedItem(int iIndexActivatedItem);

  };

#endif

