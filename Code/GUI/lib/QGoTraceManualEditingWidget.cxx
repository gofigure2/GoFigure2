/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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

#include "QGoTraceManualEditingWidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>

QGoTraceManualEditingWidget::
QGoTraceManualEditingWidget(QWidget* iParent) :
  QWidget(iParent)
  {
  this->setupUi(this);
  SetSelectedColorComboBox();
  SetTraceCollectionColorComboBox();
  SetCellTypeComboBox();
  SetSubCellTypeComboBox();
  UpdateTraceAndCollection("contour", "mesh");
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTraceManualEditingWidget::
~QGoTraceManualEditingWidget()
  {
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*int
QGoTraceManualEditingWidget::
GetCurrentCollectionID()
{
  std::string temp = this->m_CollectionColorComboBox->GetCurrentColorData().first;
  if (temp.empty())
    {
    return -1;
    }
  else
    {
    return atoi(temp.c_str());
    }
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceManualEditingWidget::
SetListCollectionID(
  std::list<ItemColorComboboxData> iListExistingID)
{
  //this->ColorIDCollectionComboBox->clear();
  this->m_CollectionColorComboBox->setItemsWithColorFromList(
    iListExistingID,CollectionName->text().toStdString());
  /*std::list<std::pair<std::string, QColor> >::iterator iter =
    iListExistingID.begin();
  while (iter != iListExistingID.end())
    {
    QColor  color = iter->second;
    QString name = iter->first.c_str();
    this->ColorIDCollectionComboBox->addColor(color, name);
    iter++;
    }
  QString Text(tr("Add a new %1...").arg(CollectionName->text()));
  this->ColorIDCollectionComboBox->setColorDialogEnabled(true, Text.toStdString());
  /** \todo when using lineages, remove the following*/
  if (CollectionName->text() == "lineage") //at that time we don't show lineages
    {
    this->m_CollectionColorComboBox->hide();
    }
  else
    {
    this->m_CollectionColorComboBox->show();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetListColors(
  std::list<ItemColorComboboxData> iListColors)
{
  this->m_SelectedColorComboBox->setItemsWithColorFromList(iListColors);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceManualEditingWidget::
SetSelectedColorComboBox()
{
  this->m_SelectedColorComboBox = new QGoSelectedColorComboBox;
 // ColorComboBox->setCreationCollection(false);

  //this->VLayoutColor->addLayout(HLayoutForTrace);
  this->VLayoutColor->addWidget(ColorLbl);
  this->VLayoutColor->addWidget(this->m_SelectedColorComboBox);

  /*QObject::connect(this->m_SelectedColorComboBox,
    SIGNAL(ItemSelected(ItemColorComboboxData)),
    this,SLOT(CheckUserActionColor(ItemColorComboboxData)));*/

  /*QObject::connect(this->m_SelectedColorComboBox,
    SIGNAL(NewColorToBeSaved(ItemColorComboboxData)),
    this,SIGNAL(NewColorToBeSaved(ItemColorComboboxData)));*/
  QObject::connect(this->m_SelectedColorComboBox,
    SIGNAL(ItemSelected(ItemColorComboboxData)),
    this,SIGNAL(NewSelectedColorActivated(ItemColorComboboxData)));

  QObject::connect(this->m_SelectedColorComboBox,
    SIGNAL(AddNewColorActivated()),
    this,SIGNAL(AddNewColor()));

  QObject::connect(this->m_SelectedColorComboBox,
    SIGNAL(DeleteActivated()),
    this,SIGNAL(DeleteColor()));

  
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceManualEditingWidget::
SetTraceCollectionColorComboBox()
{
  TraceName = new QLabel(tr("contour"));
  QFont ifont;
  ifont.setCapitalization(QFont::AllUppercase);
  ifont.setBold(true);
  TraceName->setFont(ifont);
  QHBoxLayout* HLayoutForTrace = new QHBoxLayout;
  HLayoutForTrace->addWidget(this->TraceLbl);
  HLayoutForTrace->addWidget(TraceName);
  QLabel* CollectionLbl = new QLabel(tr("Current Collection:"));
  ifont.setCapitalization(QFont::Capitalize);
  ifont.setPointSize(8);
  ifont.setUnderline(true);
  ifont.setWeight(50);
  ifont.setBold(false);
  CollectionLbl->setFont(ifont);
  this->m_CollectionColorComboBox = new QGoCollectionColorComboBox;
  CollectionName = new QLabel (tr("mesh"));
  ifont.setUnderline(false);
  ifont.setCapitalization(QFont::AllUppercase);
  ifont.setBold(true);
  CollectionName->setFont(ifont);
  /// \todo make the names change with the interaction
  QHBoxLayout* HLayoutForCollection = new QHBoxLayout;
  HLayoutForCollection->addWidget(CollectionName);
  HLayoutForCollection->addWidget(this->m_CollectionColorComboBox);
  this->VLayoutTraceCollection->addLayout(HLayoutForTrace);
  this->VLayoutTraceCollection->addWidget(CollectionLbl);
  this->VLayoutTraceCollection->addLayout(HLayoutForCollection);

  QObject::connect(this->m_CollectionColorComboBox,
    SIGNAL(ItemSelected(ItemColorComboboxData)),
    this,SIGNAL(NewCollectionActivated(ItemColorComboboxData)));

 /* QObject::connect(this->m_CollectionColorComboBox,
    SIGNAL(NewColorToBeSaved(ItemColorComboboxData)),
    this,SIGNAL(NewColorToBeSaved(ItemColorComboboxData)));*/

  QObject::connect(this->m_CollectionColorComboBox,
    SIGNAL(NewCollectionToCreate()),
    this,SIGNAL(NewCollectionToBeCreated()));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceManualEditingWidget::
SetEnableTraceCollectionColorBoxes(bool Enable)
{
  this->m_CollectionColorComboBox->setEnabled(Enable);
  this->m_SelectedColorComboBox->setEnabled(Enable);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceManualEditingWidget::
SetCellTypeComboBox()
{
  this->m_ChoseCellType = new QComboBox(this);
  QHBoxLayout* HLayoutForCellType = new QHBoxLayout;
  HLayoutForCellType->addWidget(m_ChoseCellType);
  this->VLayoutCellType->addLayout(HLayoutForCellType);
  QObject::connect(this->m_ChoseCellType, SIGNAL(activated(QString)),
                   SLOT(CheckUserAction(QString)));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceManualEditingWidget::
SetListCellTypes(QStringList iListCellTypes)
{
  this->m_ChoseCellType->clear();
  this->m_ChoseCellType->addItems(iListCellTypes);
  this->m_ChoseCellType->addItem(tr("Add a celltype..."));
  this->m_ChoseCellType->addItem(tr("Delete a celltype..."));
  
  //emit NewCellTypeActivated(this->m_ChoseCellType->currentText());
  //to check::
  //emit ListCellTypesReady();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceManualEditingWidget::
CheckUserAction(QString iCellTypeText)
{
  if (iCellTypeText == "Add a celltype...")
    {
    emit AddANewCellType();
    }
  else
    {
    if (iCellTypeText == "Delete a celltype...")
      {
      emit DeleteCellType();
      }
    else
      {
      emit NewCellTypeActivated(iCellTypeText.toStdString());
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void
QGoTraceManualEditingWidget::
CheckUserActionColor(ItemColorComboboxData iColorData)
{
  if (iColorData.first == "Add a color...")
    {
    emit AddANewColor();
    }
  else
    {
    if (iColorData.first == "Delete a color...")
      {
      emit DeleteColor();
      }
    else
      {
      emit NewSelectedColorActivated(iColorData);
      }
    }
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceManualEditingWidget::
SetSubCellTypeComboBox()
{
  this->m_ChoseSubCellType = new QComboBox(this);
  this->VLayoutSubCellType->addWidget(LabelSubCellType);
  this->VLayoutSubCellType->addWidget(m_ChoseSubCellType);
  //to check::
  //emit NewSubCellTypeActivated(this->m_ChoseSubCellType->currentText());
  QObject::connect(this->m_ChoseSubCellType, SIGNAL(activated(QString)),
                   SLOT(CheckUserActionSubCell(QString)));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceManualEditingWidget::
SetListSubCellTypes(QStringList iListSubCellTypes)
{
  this->m_ChoseSubCellType->clear();
  this->m_ChoseSubCellType->addItems(iListSubCellTypes);
  this->m_ChoseSubCellType->addItem(tr("Add a subcelltype..."));
  this->m_ChoseSubCellType->addItem(tr("Delete a subcelltype..."));
  //emit ListSubCellTypesReady();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceManualEditingWidget::
CheckUserActionSubCell(QString iCellTypeText)
{
  if (iCellTypeText == "Add a subcelltype...")
    {
    emit AddANewSubCellType();
    }
  else
  {
  if (iCellTypeText == "Delete a subcelltype...")
    {
    emit DeleteSubCellType();
    }
  else
    {
    emit NewSubCellTypeActivated(iCellTypeText.toStdString());
    }
  }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*std::string QGoTraceManualEditingWidget::GetCurrentCellType()
{
  return this->m_ChoseCellType->currentText().toStdString();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoTraceManualEditingWidget::GetCurrentSubCellType()
{
  return this->m_ChoseSubCellType->currentText().toStdString();
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetCurrentCellType(
  std::string iCellTypeText)
{
  int index = this->m_ChoseCellType->findText(iCellTypeText.c_str());
  if (index == -1)
    {
    this->m_ChoseCellType->setCurrentIndex(0);
    }
  else
    {
    this->m_ChoseCellType->setCurrentIndex(index);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetCurrentSubCellType(
  std::string iSubCellTypeText)
{
  int index = this->m_ChoseSubCellType->findText(iSubCellTypeText.c_str());
  if (index == -1)
    {
    this->m_ChoseSubCellType->setCurrentIndex(0);
    }
  else
    {
    this->m_ChoseSubCellType->setCurrentIndex(index);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetCurrentColor(std::string iColorText)
{
  int index = this->m_SelectedColorComboBox->findText(iColorText.c_str());
  if (index == -1)
    {
    this->m_SelectedColorComboBox->setCurrentIndex(0);
    }
  else
    {
    this->m_SelectedColorComboBox->setCurrentIndex(index);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetCurrentCollectionID(std::string iID)
{
  int index = this->m_CollectionColorComboBox->findText(iID.c_str());
  if (index == -1)
    {
    this->m_CollectionColorComboBox->setCurrentIndex(0);
    }
  else
    {
    this->m_CollectionColorComboBox->setCurrentIndex(index);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::UpdateTraceAndCollection(
  std::string iTrace, std::string iCollection)
{
  this->TraceName->setText(iTrace.c_str());
  this->CollectionName->setText(iCollection.c_str());
  if (iTrace == "contour" || iTrace == "mesh")
    {
    this->m_ChoseCellType->show();
    this->LabelCellType->show();
    this->m_ChoseSubCellType->show();
    this->LabelSubCellType->show();
    }
  else
    {
    this->m_ChoseCellType->hide();
    this->LabelCellType->hide();
    this->m_ChoseSubCellType->hide();
    this->LabelSubCellType->hide();
    }
  this->show();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoTraceManualEditingWidget::GetTraceName()
{
  return this->TraceName->text().toStdString();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::AddANewCollectionID(
  ItemColorComboboxData iNewCollectionID)
{
  this->m_CollectionColorComboBox->AddItemWithColor(iNewCollectionID,true);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoTraceManualEditingWidget::UpdateCollectionComboboxWithDeletedItem(
  std::list<ItemColorComboboxData> iListExistingID)
{
  
  this->m_CollectionColorComboBox->ListToUpdateWithItemDeleted(
    iListExistingID);
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoTraceManualEditingWidget::UpdateColorComboboxWithDeletedItem(
    std::list<ItemColorComboboxData> iListColors)
{
  this->m_SelectedColorComboBox->ListToUpdateWithItemDeleted(
    iListColors);
}*/