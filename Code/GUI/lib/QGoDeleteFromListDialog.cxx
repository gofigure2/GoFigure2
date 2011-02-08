/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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
#include "QGoDeleteFromListDialog.h"
#include <QStringListModel>
#include <QStringList>
#include <QTreeWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QModelIndex>
#include <QPainter>
#include "ConvertToStringHelper.h"

QGoDeleteFromListDialog::QGoDeleteFromListDialog
  (std::vector< std::string > iVectorEntities,
  QWidget *iParent,
  std::string iEntityName
  ):QDialog(iParent)
{
  this->SetUpUi(iEntityName);
  this->SetItemsFromTheVector(iVectorEntities);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoDeleteFromListDialog::QGoDeleteFromListDialog(
  QGoDeleteFromListDialog::ListOfItemColorComboboxData  iDataListWithColor,
  QWidget *iParent,
  std::string iEntityName
  ):QDialog(iParent)
{
  this->SetUpUi(iEntityName);
  this->SetItemsInTheListWithColor(iDataListWithColor);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoDeleteFromListDialog::~QGoDeleteFromListDialog()
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoDeleteFromListDialog::SetItemsFromTheVector(
  std::vector< std::string > iVectorItems)
{
  for ( size_t i = 0; i < iVectorItems.size(); ++i )
    {
    QListWidgetItem *item =
      new QListWidgetItem(iVectorItems[i].c_str(), this->m_ListWidget);
    (void)item;
    }
}

//--------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDeleteFromListDialog::SetItemsInTheListWithColor(
  std::list< ItemColorComboboxData > iDataList)
{
  std::list< ItemColorComboboxData >::iterator iter = iDataList.begin();
  QPixmap                                      pix(12, 12);
  QPainter                                     painter(&pix);
  QIcon                                        Icon;
  while ( iter != iDataList.end() )
    {
    if ( iter->second.isValid() )
      {
      painter.setPen(Qt::gray);
      painter.setBrush( QBrush(iter->second) );
      painter.drawRect(0, 0, 12, 12);
      }
    Icon.addPixmap(pix);
    QListWidgetItem *item =
      new QListWidgetItem(Icon, iter->first.c_str(), this->m_ListWidget);
    (void)item;
    ++iter;
    }
}

//--------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDeleteFromListDialog::SelectionValidation()
{
  QList< QListWidgetItem * > ListEntitiesToDeleteSelected =
    this->m_ListWidget->selectedItems();
  if ( !ListEntitiesToDeleteSelected.empty() )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Are you sure you want to delete these %1s ?")
      .arg( this->m_EntityName.c_str() ) );
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int r = msgBox.exec();
    if ( r == 16384 )
      {
      DeleteSelection(ListEntitiesToDeleteSelected);
      this->accept();
      }
    else
      {
      msgBox.close();
      }
    }
  else
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1.")
      .arg( this->m_EntityName.c_str() ) );
    msgBox.exec();
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoDeleteFromListDialog::DeleteSelection(
  QList< QListWidgetItem * > iListEntitiesToDelete)
{
  std::vector< std::string > VectorNamesToDelete;
  for ( int i = 0; i < iListEntitiesToDelete.size(); i++ )
    {
    VectorNamesToDelete.push_back( iListEntitiesToDelete.at(i)->text().toStdString() );
    }
  emit ListEntitiesToDelete(VectorNamesToDelete);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoDeleteFromListDialog::SetUpUi(std::string iEntityName)
{
  this->m_EntityName = iEntityName;
  this->m_ListWidget = new QListWidget(this);
  this->m_ListWidget->setSelectionMode(QAbstractItemView::MultiSelection);

  QDialogButtonBox *ButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                     | QDialogButtonBox::Cancel);

  QVBoxLayout *vlayout = new QVBoxLayout(this);
  vlayout->addWidget(this->m_ListWidget);
  vlayout->addWidget(ButtonBox);
  this->setWindowTitle( tr("Delete a %1").arg( this->m_EntityName.c_str() ) );
  this->setLayout(vlayout);

  QObject::connect( ButtonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
  QObject::connect( ButtonBox, SIGNAL( rejected() ), this, SIGNAL( CancelRequested() ) );
  QObject::connect( ButtonBox, SIGNAL( accepted() ), this, SLOT( SelectionValidation() ) );
}
