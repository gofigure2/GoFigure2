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
#include "QGoColorComboBox.h"
#include <iostream>
#include <sstream>
#include <QStringListModel>
#include <QHeaderView>
#include <QSettings>
#include <QApplication>
#include <QClipboard>
#include <QToolButton>

QGoColorComboBox::QGoColorComboBox(std::string iTextToAddANewOne,QWidget *parent)
:QComboBox(parent)
{
  m_TextToAddANewOne = iTextToAddANewOne;
  QObject::connect(this, SIGNAL(activated(int)), SLOT(emitActivatedItem(int )));
  QObject::connect(this, SIGNAL(AddANewOneActivated()), 
     this,SLOT(ActionWhenNewOneRequested()));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoColorComboBox::~QGoColorComboBox()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoColorComboBox::setItemsWithColorFromList(
  std::list<ItemColorComboboxData> iDataFromList)
{
  this->clear();
  if (!iDataFromList.empty())
    {
    std::list<ItemColorComboboxData >::iterator iter =
      iDataFromList.begin();
    while (iter != iDataFromList.end())
      {
      this->AddItemWithColor(iter,false);//we don't want the signal emitted for each added item,
      //only the currentIndex one at the end of the added list.
      iter++;
      }
    this->addItem(this->m_TextToAddANewOne.c_str());
    this->show();
    emit ItemSelected(this->GetTheItemColorComboBoxData(this->currentIndex()));
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoColorComboBox::AddItemWithColor(ItemColorComboboxData iNewItemData,
   bool SelectTheAddedItem)
{
  QPixmap  pix(12, 12);
  QPainter painter(&pix);
  if (iNewItemData.second.isValid())
    {
    painter.setPen(Qt::gray);
    painter.setBrush(QBrush(iNewItemData.second));
    painter.drawRect(0, 0, 12, 12);
    }
  QIcon Icon;
  Icon.addPixmap(pix);
  //we want to insert the item before the "more":
  //if the index is 0 or negative,the new item is prepended to the list of existing items:
  int Index = this->count() -1;
  this->insertItem(Index,Icon,iNewItemData.first,iNewItemData.second);
  if (SelectTheAddedItem)
    {
    this->setCurrentIndex(Index);
    this->emitActivatedItem(Index);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoColorComboBox::emitActivatedItem(int iIndexActivatedItem)
{
  int numberitem = this->count();
  if(iIndexActivatedItem == this->count()-1)
  {
  emit AddANewOneActivated();
  }
  else
  {
  ItemColorComboboxData ItemActivated = 
    this->GetTheItemColorComboBoxData(iIndexActivatedItem);
  emit ItemSelected(ItemActivated);
  this->m_LastActivated = ItemActivated.first.c_str();
  }
  //to check:
  //this->setCurrentIndex(iIndexActivatedItem);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoColorComboBox::ItemColorComboboxData 
QGoColorComboBox::GetTheItemColorComboBoxData(int iIndex)
{
  ItemColorComboboxData Item;
  QVariant variant = this->itemData(iIndex);
 // ItemActivated.second = this->itemIcon(iIndexActivatedItem).pixmap();
  Item.second = variant.value<QColor>();
  Item.first = this->itemText(iIndex).toStdString();
  //for test purpose:
  int red = Item.second.red();
  int green = Item.second.green();
  int blue = Item.second.blue();
  int alpha = Item.second.alpha();

  return Item;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoColorComboBox::ListToUpdateWithItemDeleted(
  std::list< ItemColorComboboxData > iDataFromList)
{
  this->setItemsWithColorFromList(iDataFromList);
  int Index = this->findText(this->m_LastActivated);
  this->setCurrentIndex(Index);
}
