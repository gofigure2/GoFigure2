/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 525 $  // Revision of last commit
  Date: $Date: 2009-08-05 16:08:25 -0400 (Wed, 05 Aug 2009) $  // Date of last commit
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
#include "QGoDeleteDBEntityDialog.h"
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
#include "SelectQueryDatabaseHelper.h"
#include "QueryDataBaseHelper.h"
#include "ConvertToStringHelper.h"


QGoDeleteDBEntityDialog::QGoDeleteDBEntityDialog( QWidget* iParent,
  std::string iEntityName,int iImgSessionID, 
  vtkMySQLDatabase* iDatabaseConnector):QDialog(iParent)
{
  this->m_EntityName = iEntityName;
  this->m_ImgSessionID = iImgSessionID;
  this->m_DatabaseConnector = iDatabaseConnector;
  //QStringListModel* SourceModel = new QStringListModel();
  //SourceModel->setStringList(this->GetListExistingEntities(iDatabaseConnector));  
  //this->m_ListView = new QListView;
  //this->m_ListView->setModel(SourceModel);
  //this->m_ListView->setSelectionMode(QAbstractItemView::MultiSelection);
  this->m_ListWidget = new QListWidget;
  this->SetItemsInTheList(iDatabaseConnector);
  this->m_ListWidget->setSelectionMode(QAbstractItemView::MultiSelection);

  QDialogButtonBox* ButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
  QObject::connect(ButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(ButtonBox,SIGNAL(accepted()), this, SLOT(AskUserConfirmation()));
  QVBoxLayout* layout = new QVBoxLayout(this);
  //layout->addWidget(this->m_ListView);
  layout->addWidget(this->m_ListWidget);
  layout->addWidget(ButtonBox);
  this->setWindowTitle(tr("Delete a %1").arg(this->m_EntityName.c_str()));
  this->setLayout(layout);

 // QTreeWidget* TreeWidget = new QTreeWidget;
  //TreeWidget->setColumnCount(2);
  //TreeWidget->setModel(model);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoDeleteDBEntityDialog::~QGoDeleteDBEntityDialog()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QStringList QGoDeleteDBEntityDialog::GetListExistingEntities(
  vtkMySQLDatabase* iDatabaseConnector)
{
  std::vector<std::string> ResultsQuery = ListSpecificValuesForOneColumn(
    iDatabaseConnector,this->m_EntityName,"Name","ImagingSessionID",
    ConvertToString<int>(this->m_ImgSessionID),"Name");

  QStringList ListEntities;
  for (int i=0; i< ResultsQuery.size();i++)
    {
    ListEntities.append(ResultsQuery[i].c_str());
    }
  return ListEntities;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoDeleteDBEntityDialog::SetItemsInTheList(
  vtkMySQLDatabase* iDatabaseConnector)
{
 QStringList ListNamesEntities = this->GetListExistingEntities(
   iDatabaseConnector);
 for (int i=0; i<ListNamesEntities.size();i++)
   {
   QListWidgetItem* item = new QListWidgetItem(
     ListNamesEntities.at(i),this->m_ListWidget);
   }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoDeleteDBEntityDialog::AskUserConfirmation()
{
    QMessageBox msgBox;

    msgBox.setText(
      tr("Are you sure you want to delete these %1s ?")
      .arg(this->m_EntityName.c_str()));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int r = msgBox.exec(); 
    if(r == 16384)
      {
      DeleteSelection();
      this->accept();
      }
    else
      {
      msgBox.close();
      }

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoDeleteDBEntityDialog::DeleteSelection()
{
  /*QModelIndexList indexes = this->m_ListView->selectedIndexes();
 // int i = 0;
 // QModelIndex Index = ListIndexes.at(i);
//this->m_ListView->selectedItem().text();
//QListViewItem* ItemSelected = this->m_ListView->selectedItem();
  QModelIndex index;
  std::list<std::string> ListNameEntityToDelete;

     foreach(index, indexes) 
       {
       QString text = QString("(%1,%2)").arg(index.row()).arg(index.column());
         //model->setData(index, text);
         //this->m_SourceModel->setData(index,text);
       ListNameEntityToDelete.push_back(index.data().toString().toStdString());
       }*/
  QList<QListWidgetItem*> ListEntitiesToDelete = 
    this->m_ListWidget->selectedItems();
  std::vector<std::string> VectorNamesToDelete;
  for (int i=0; i<ListEntitiesToDelete.size();i++)
    {
    VectorNamesToDelete.push_back(ListEntitiesToDelete.at(i)->text().toStdString());
    }  
  DeleteRows(this->m_DatabaseConnector,this->m_EntityName, 
  "Name", VectorNamesToDelete);
}