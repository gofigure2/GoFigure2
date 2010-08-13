/*=========================================================================
  Author: $Author: lydiesouhait $  // Author of last commit
  Version: $Rev: 1873 $  // Revision of last commit
  Date: $Date: 2010-07-29 13:12:06 -0400 (Thu, 29 Jul 2010) $  // Date of last commit
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
#include "QGoCollectionColorComboBox.h"
#include <iostream>

QGoCollectionColorComboBox::QGoCollectionColorComboBox(QWidget *iparent)
:QGoColorComboBox("Add a new mesh...",iparent)
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoCollectionColorComboBox::~QGoCollectionColorComboBox()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoCollectionColorComboBox::ActionWhenNewOneRequested()
{
  emit NewCollectionToCreate();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoCollectionColorComboBox::InitializeTheListWithColor(
  std::list<ItemColorComboboxData> iDataFromList, std::string iCollectionName)
{
  this->SetItemsFromListWithColor(iDataFromList,iCollectionName);
  //if it is the 1rst time for the list to be displayed, there has to be an activated
  //item:
  //by default, the one selected by the combobox is the one to stick to:
  this->EmitActivatedItem(this->currentIndex());
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoCollectionColorComboBox::SetItemsFromListWithColor(
  std::list<ItemColorComboboxData> iDataFromList, std::string iCollectionName)
{
  QString TextForNewOne(tr("Add a new %1 ...").arg(iCollectionName.c_str()));
  this->m_TextToAddANewOne = TextForNewOne.toStdString();
  QGoColorComboBox::SetItemsFromListWithColor(iDataFromList);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
