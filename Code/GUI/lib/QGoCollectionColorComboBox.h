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

#ifndef __QGoCollectionColorComboBox_h
#define __QGoCollectionColorComboBox_h

#include "QGoColorComboBox.h"

/**
\class QGoCollectionColorComboBox
\brief this class inherits from QGoColorComboBox, has only the add item option and
update the text according to the iCollectionName. It handles the printing of the
collection IDs
*/
class QGOGUILIB_EXPORT QGoCollectionColorComboBox : public QGoColorComboBox
  {
  Q_OBJECT
public:
  explicit QGoCollectionColorComboBox(QWidget *parent = 0);
  virtual ~QGoCollectionColorComboBox();

  void SetTextToAdd(std::string iCollectionName);

signals:
  void NewCollectionToCreate();

public slots:
  /**
  \brief clear the items already in the combobox,displayed the one in the iDataFromList,
  the items to add/delete and adjust the text to add an item according to the iCollectionName.
  \param[in] iDataFromList contains the names and QColor of the items to be 
  displayed in the combobox
  \param[in] iCollectionName name to use in the add an item
  \overload from the mother class method
  */
  virtual void SetItemsFromList(std::list<ItemColorComboboxData> iDataFromList,
    std::string iCollectionName);

  /**
  \brief
  \param[in] iCollectionName name of the collection to add in the text to add a new
  collection
  \overload from mother class method
  */
  void InitializeTheList(std::list<ItemColorComboboxData> iDataFromList,
    std::string iCollectionName);
 
protected slots:
  //mother class method
  virtual void ActionWhenNewOneRequested();

  };

#endif