/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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

#include "QGoTraceManualEditingWidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>


QGoTraceManualEditingWidget::
QGoTraceManualEditingWidget( QWidget* iParent) :
  QWidget( iParent )
{
  this->setupUi( this );
  SetColorTraceComboBox();
  SetColorIDCollectionComboBox();
  SetCellTypeComboBox();
  SetSubCellTypeComboBox();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTraceManualEditingWidget::
~QGoTraceManualEditingWidget()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoTraceManualEditingWidget::GetCurrentCollectionID()
{
  std::string temp = this->ColorIDCollectionComboBox->GetCurrentColorData().first;
  if( temp.empty() )
    {
    return -1;
    }
  else
    {
    return atoi( temp.c_str() );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetCollectionID(
  std::list<std::pair<std::string,QColor> > iListExistingID)
{
  std::list<std::pair<std::string,QColor> >::iterator iter =
    iListExistingID.begin();
  while (iter != iListExistingID.end())
    {
    QColor color = iter->second;
    QString name = iter->first.c_str();
    this->ColorIDCollectionComboBox->addColor(color,name);
    iter++;
    }
  QString Text(tr("Add a new %1...").arg(CollectionName->text()));
  this->ColorIDCollectionComboBox->setColorDialogEnabled(true,Text.toStdString());
  this->ColorIDCollectionComboBox->show();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetColorTraceComboBox()
{
  ColorTraceComboBox = new QtColorComboBox;
  ColorTraceComboBox->setCreationCollection(false);
  TraceName = new QLabel(tr("contour"));

  QHBoxLayout* HLayoutForTrace = new QHBoxLayout;
  HLayoutForTrace->addWidget(this->TraceLbl);
  HLayoutForTrace->addWidget(TraceName);

  this->VLayoutTraceColor->addLayout(HLayoutForTrace);
  this->VLayoutTraceColor->addWidget(ColorLbl);
  this->VLayoutTraceColor->addWidget(ColorTraceComboBox);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetColorIDCollectionComboBox()
{
  ColorIDCollectionComboBox = new QtColorComboBox;
  ColorIDCollectionComboBox->setCreationCollection(true);
  CollectionName = new QLabel (tr("mesh"));

  /// \todo make the names change with the interaction
  QHBoxLayout* HLayoutForCollection = new QHBoxLayout;
  HLayoutForCollection->addWidget(CollectionName);
  HLayoutForCollection->addWidget(ColorIDCollectionComboBox);
  this->VLayoutCollection->addLayout(HLayoutForCollection);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetEnableTraceCollectionColorBoxes(bool Enable)
{
  this->ColorIDCollectionComboBox->setEnabled(Enable);
  this->ColorTraceComboBox->setEnabled(Enable);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetCellTypeComboBox()
{
  this->m_ChoseCellType = new QComboBox(this);
  QHBoxLayout* HLayoutForCellType = new QHBoxLayout;
  HLayoutForCellType->addWidget(m_ChoseCellType);
  this->VLayoutCellType->addLayout(HLayoutForCellType);
  QObject::connect(this->m_ChoseCellType,SIGNAL(currentIndexChanged(QString)),
    SLOT(CheckUserAction(QString)));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetListCellTypes(
  QStringList iListCellTypes)
{
  this->m_ChoseCellType->clear();
  this->m_ChoseCellType->addItems(iListCellTypes);
  this->m_ChoseCellType->addItem(tr("Add a celltype..."));
  this->m_ChoseCellType->addItem(tr("Delete a celltype..."));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::CheckUserAction(QString iCellTypeText)
{
 if (iCellTypeText == "Add a celltype...")
   {
   emit AddANewCellType();
   }
 if (iCellTypeText == "Delete a celltype...")
   {
   emit DeleteCellType();
   }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetSubCellTypeComboBox()
{
  this->m_ChoseSubCellType = new QComboBox(this);
  this->VLayoutSubCellType->addWidget(LabelSubCellType);
  this->VLayoutSubCellType->addWidget(m_ChoseSubCellType);
  QObject::connect(this->m_ChoseSubCellType,SIGNAL(currentIndexChanged(QString)),
    SLOT(CheckUserActionSubCell(QString)));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::SetListSubCellTypes(
  QStringList iListSubCellTypes)
{
  this->m_ChoseSubCellType->clear();
  this->m_ChoseSubCellType->addItems(iListSubCellTypes);
  this->m_ChoseSubCellType->addItem(tr("Add a subcelltype..."));
  this->m_ChoseSubCellType->addItem(tr("Delete a subcelltype..."));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceManualEditingWidget::CheckUserActionSubCell(
  QString iCellTypeText)
{
 if (iCellTypeText == "Add a subcelltype...")
   {
   emit AddANewSubCellType();
   }
 if (iCellTypeText == "Delete a subcelltype...")
   {
   emit DeleteSubCellType();
   }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------