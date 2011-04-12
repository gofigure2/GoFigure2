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

#include "QGoTraceSettingsDockWidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>

// DEBUG
#include <QtDebug>
#include <iostream>

QGoTraceSettingsDockWidget::QGoTraceSettingsDockWidget(QWidget *iParent) :
  QDockWidget(iParent)
{
  this->SetUpUi();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTraceSettingsDockWidget::
~QGoTraceSettingsDockWidget()
{
  if ( this->m_SelectedCollectionData )
    {
    delete this->m_SelectedCollectionData;
    }
  if ( this->m_SelectedCellType )
    {
    delete this->m_SelectedCellType;
    }
  if ( this->m_SelectedSubCellType )
    {
    delete this->m_SelectedSubCellType;
    }
  if ( this->m_SelectedColorData )
    {
    delete this->m_SelectedColorData;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::SetUpUi()
{
  QWidget* TraceSettingsWidget = new QWidget(this);

  QHBoxLayout* ColorLayout = new QHBoxLayout;
  SetSelectedColorComboBox(ColorLayout);
  QHBoxLayout* TraceLayout = new QHBoxLayout;
  QVBoxLayout* TraceCollectionLayout = new QVBoxLayout;
  SetTraceCollectionColorComboBox(TraceCollectionLayout);
  QVBoxLayout* CellTypeLayout = new QVBoxLayout;
  SetCellTypeComboBox(CellTypeLayout);
  QVBoxLayout* SubCellLayout = new QVBoxLayout;
  SetSubCellTypeComboBox(SubCellLayout);

  QVBoxLayout* MainLayout = new QVBoxLayout;
  MainLayout->addLayout(TraceCollectionLayout);
  MainLayout->addLayout(ColorLayout);
  MainLayout->addLayout(CellTypeLayout);
  MainLayout->addLayout(SubCellLayout);

  TraceSettingsWidget->setLayout(MainLayout);
  TraceSettingsWidget->setSizePolicy(
    QSizePolicy::Expanding, QSizePolicy::Fixed);

  this->setWidget(TraceSettingsWidget);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsDockWidget::SetListCollectionID(
  std::list< ItemColorComboboxData > iListExistingID,
  std::string iCollectionIDtoSelect)
{
  if ( !iCollectionIDtoSelect.empty() )
    {
    this->m_CollectionColorComboBox->SetItemsFromListWithColor( iListExistingID,
                                                                this->m_CollectionName->text().toStdString() );
    this->m_CollectionColorComboBox->SetCurrentItemAndActivate(iCollectionIDtoSelect);
    }
  else
    {
    this->m_CollectionColorComboBox->InitializeTheListWithColor( iListExistingID,
                                                                 this->m_CollectionName->text().toStdString() );
    }
  /** \todo Lydie: when using lineages, remove the following*/
  /*if ( this->m_CollectionName->text() == "lineage" ) //at that time we don't
                                                     // show lineages
    {
    this->m_CollectionColorComboBox->hide();
    }
  else
    {
    this->m_CollectionColorComboBox->show();
    }*/
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::SetListColors(
  std::list< ItemColorComboboxData > iListColors, std::string iColorToSelect)
{
  this->SetListItemAndSelect< QGoSelectedColorComboBox >(this->m_SelectedColorComboBox,
                                                         iListColors, iColorToSelect);
  this->m_SelectedColorComboBox->SetCurrentItemAndActivate(iColorToSelect);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::SetListColorsWithSelectedOne(
  std::list< ItemColorComboboxData > iListColors)
{
  this->SetListColors(iListColors, this->m_SelectedColorData->first);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsDockWidget::SetListCellTypes(NamesDescrContainerType iCellTypesData,
                                              std::string iCellTypeToSelect)
{
  this->SetListItemAndSelect< QGoComboBox >(this->m_ChoseCellType,
                                            iCellTypesData, iCellTypeToSelect);
  this->m_ChoseCellType->SetCurrentItemAndActivate(iCellTypeToSelect);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsDockWidget::SetListCellTypeWithSelectedOne(
  NamesDescrContainerType iCellTypesData)
{
  this->SetListCellTypes(iCellTypesData, *this->m_SelectedCellType);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsDockWidget::SetListSubCellTypes(NamesDescrContainerType iSubCellTypesData,
                                                 std::string iSubCellTypeToSelect)
{
  this->SetListItemAndSelect< QGoComboBox >(this->m_ChoseSubCellType,
                                            iSubCellTypesData, iSubCellTypeToSelect);
  this->m_ChoseSubCellType->SetCurrentItemAndActivate(iSubCellTypeToSelect);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsDockWidget::SetListSubCellTypeWithSelectedOne(
  NamesDescrContainerType iSubCellTypesData)
{
  this->SetListSubCellTypes(iSubCellTypesData, *this->m_SelectedSubCellType);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsDockWidget::SetSelectedColorComboBox(
  QHBoxLayout* iColorLayout)
{
  this->m_SelectedColorComboBox = new QGoSelectedColorComboBox;
  QLabel* ColorLbl = new QLabel(tr("Color:"), this);
  iColorLayout->addWidget(ColorLbl);
  iColorLayout->addWidget(this->m_SelectedColorComboBox);

  this->m_SelectedColorData = new ItemColorComboboxData;
  
  QObject::connect( this->m_SelectedColorComboBox,
                    SIGNAL( ItemSelected(ItemColorComboboxData) ),
                    this, SLOT( UpdateValueSelectedColor(ItemColorComboboxData) ) );

  QObject::connect( this->m_SelectedColorComboBox,
                    SIGNAL( AddNewColorActivated() ),
                    this, SIGNAL( AddNewColor() ) );

  QObject::connect( this->m_SelectedColorComboBox,
                    SIGNAL( DeleteActivated() ),
                    this, SIGNAL( DeleteColor() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsDockWidget::SetTraceCollectionColorComboBox(
  QVBoxLayout* iLayoutTraceCollection)
{
  this->m_TraceName = new QLabel( tr("contour") );
  this->setWindowTitle(tr("contour settings") );
  QFont font;
  font.setCapitalization(QFont::AllUppercase);
  font.setBold(true);
  this->m_TraceName->setFont(font);
  QHBoxLayout *HLayoutForTrace = new QHBoxLayout;
  font.setCapitalization(QFont::Capitalize);
  font.setPointSize(8);
  font.setUnderline(true);
  font.setWeight(50);
  font.setBold(false);
  QLabel* TraceLbl = new QLabel(tr("Trace:"), this);
  TraceLbl->setFont(font);
  HLayoutForTrace->addWidget(TraceLbl);
  HLayoutForTrace->addWidget(this->m_TraceName);
  QLabel *CollectionLbl = new QLabel( tr("Collection:") ); 
  CollectionLbl->setFont(font);
  this->m_CollectionColorComboBox = new QGoCollectionColorComboBox;
  this->m_CollectionName = new QLabel ( tr("mesh") );
  font.setUnderline(false);
  font.setCapitalization(QFont::AllUppercase);
  font.setBold(true);
  this->m_CollectionName->setFont(font);

  QHBoxLayout *HLayoutForCollection = new QHBoxLayout;
  HLayoutForCollection->addWidget(this->m_CollectionName);
  HLayoutForCollection->addWidget(this->m_CollectionColorComboBox);
  iLayoutTraceCollection->addLayout(HLayoutForTrace);
  iLayoutTraceCollection->addWidget(CollectionLbl);
  iLayoutTraceCollection->addLayout(HLayoutForCollection);

  this->m_SelectedCollectionData = new ItemColorComboboxData;

  QObject::connect( this->m_CollectionColorComboBox,
                    SIGNAL( ItemSelected(ItemColorComboboxData) ),
                    this, SLOT( UpdateValueSelectedCollection(ItemColorComboboxData) ) );

  QObject::connect( this->m_CollectionColorComboBox,
                    SIGNAL( NewCollectionToCreate() ),
                    this, SIGNAL( NewCollectionToBeCreated() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void
QGoTraceManualEditingWidget::
SetEnableTraceCollectionColorBoxes(bool Enable)
{
  this->m_CollectionColorComboBox->setEnabled(Enable);
  this->m_SelectedColorComboBox->setEnabled(Enable);
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsDockWidget::SetCellTypeComboBox(QVBoxLayout* iCellLayout)
{
  this->m_ChoseCellType = new QGoComboBox("Add a new celltype...",
                                          this, "Delete a celltype...");

  QHBoxLayout *HLayoutForCellType = new QHBoxLayout;
  this->m_LabelCellType = new QLabel(tr("CellType:"), this);
  HLayoutForCellType->addWidget(this->m_LabelCellType);
  HLayoutForCellType->addWidget(m_ChoseCellType);

  iCellLayout->addLayout(HLayoutForCellType);

  this->m_SelectedCellType = new std::string;

  QObject::connect( this->m_ChoseCellType,
                    SIGNAL( ItemSelected(std::string) ),
                    this, SLOT( UpdateValueSelectedCellType(std::string) ) );

  QObject::connect( this->m_ChoseCellType,
                    SIGNAL( AddANewOneActivated() ),
                    this, SIGNAL( AddANewCellType() ) );

  QObject::connect( this->m_ChoseCellType,
                    SIGNAL( DeleteActivated() ),
                    this, SIGNAL( DeleteCellType() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsDockWidget::SetSubCellTypeComboBox(QVBoxLayout* iSubCellLayout)
{
  this->m_ChoseSubCellType = new QGoComboBox("Add a new subcelltype...", this,
                                             "Delete a subcelltype...");
  this->m_LabelSubCellType = new QLabel(tr("SubCellType:"), this);
  iSubCellLayout->addWidget(this->m_LabelSubCellType);
  iSubCellLayout->addWidget(m_ChoseSubCellType);

  this->m_SelectedSubCellType = new std::string;

  QObject::connect( this->m_ChoseSubCellType,
                    SIGNAL( ItemSelected(std::string) ),
                    this, SLOT( UpdateValueSelectedSubCellType(std::string) ) );

  QObject::connect( this->m_ChoseSubCellType,
                    SIGNAL( AddANewOneActivated() ),
                    this, SIGNAL( AddANewSubCellType() ) );

  QObject::connect( this->m_ChoseSubCellType,
                    SIGNAL( DeleteActivated() ),
                    this, SIGNAL( DeleteSubCellType() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::SetCurrentCellType(
  std::string iCellTypeText)
{
  this->m_ChoseCellType->SetCurrentItem(iCellTypeText);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::SetCurrentCellTypeToSelectedOne()
{
  this->SetCurrentCellType(*this->m_SelectedCellType);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::SetCurrentSubCellType(
  std::string iSubCellTypeText)
{
  this->m_ChoseSubCellType->SetCurrentItem(iSubCellTypeText);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::SetCurrentSubCellTypeToSelectedOne()
{
  this->SetCurrentSubCellType(*this->m_SelectedSubCellType);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::SetCurrentColor(std::string iColorText)
{
  this->m_SelectedColorComboBox->SetCurrentItem(iColorText);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::SetCurrentColorToSelectedOne()
{
  this->SetCurrentColor(this->m_SelectedColorData->first);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::SetCurrentCollectionID(std::string iID)
{
  this->m_CollectionColorComboBox->SetCurrentItem(iID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::UpdateTraceAndCollection(
  std::string iTrace, std::string iCollection)
{
  this->ChangeWindowTitle(iTrace);

  this->m_TraceName->setText( iTrace.c_str() );
  this->m_CollectionName->setText( iCollection.c_str() );
  this->m_CollectionName->show();
  
  if ( iTrace == "contour" || iTrace == "mesh" )
    {
    this->m_ChoseCellType->show();
    this->m_LabelCellType->show();
    this->m_ChoseSubCellType->show();
    this->m_LabelSubCellType->show();
    this->m_CollectionName->show();
    this->m_CollectionColorComboBox->show();
    }
  else
    {
    this->m_ChoseCellType->hide();
    this->m_LabelCellType->hide();
    this->m_ChoseSubCellType->hide();
    this->m_LabelSubCellType->hide();
    this->m_CollectionColorComboBox->hide();
    }

  if (iTrace == "lineage")
    {
    this->m_CollectionName->hide();
    }
  // update visualization
  this->show();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::ChangeWindowTitle(std::string iTrace)
{
  /*if (iTrace == "contour")
    {
    this->setWindowTitle("Contour Settings");
    }
  if (iTrace == "mesh")
    {
    this->setWindowTitle("Mesh Settings");
    }
  if (iTrace == "track")
    {
    this->setWindowTitle("Track Settings");
    }*/
  QString Title(tr("%1 Settings").arg(iTrace.c_str() ) );
  QFont Font;
  Font.setCapitalization(QFont::Capitalize);
  this->setFont(Font);
  this->setWindowTitle(Title);
  if (iTrace == "lineage")
    {
    //this->setWindowTitle("Lineage Settings");
    this->m_CollectionName->hide();
    }
  if (iTrace == "lineage" || iTrace == "track")
    {
    this->m_CollectionColorComboBox->hide();
    }
  else
    {
    this->m_CollectionName->show();
    this->m_CollectionColorComboBox->show();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoTraceSettingsDockWidget::GetTraceName()
{
  return this->m_TraceName->text().toStdString();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::AddANewCollectionID(
  std::pair<std::string, QColor> iNewCollectionID)
{
  this->m_CollectionColorComboBox->AddItemWithColor(iNewCollectionID, true);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string * QGoTraceSettingsDockWidget::GetPointerSelectedCellType()
{
  return this->m_SelectedCellType;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string * QGoTraceSettingsDockWidget::GetPointerSelectedSubCellType()
{
  return this->m_SelectedSubCellType;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTraceSettingsDockWidget::ItemColorComboboxData *
QGoTraceSettingsDockWidget::GetPointerCollectionData()
{
  return this->m_SelectedCollectionData;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTraceSettingsDockWidget::ItemColorComboboxData *
QGoTraceSettingsDockWidget::GetPointerColorData()
{
  return this->m_SelectedColorData;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::UpdateValueSelectedCollection(
  ItemColorComboboxData iCollectionData)
{
  std::string CollectionID = iCollectionData.first;

  if ( CollectionID.size() > 9 )
    {
    if ( CollectionID.substr(0, 9) == "Add a new" )
      {
      this->m_SelectedCollectionData->first = "0";
      }
    }
  else
    {
    *this->m_SelectedCollectionData = iCollectionData;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** \todo Lydie: except when adding a new value, these methods are called
twice when something changes in the comboboxes*/
void QGoTraceSettingsDockWidget::UpdateValueSelectedCellType(std::string iCellType)
{
  *this->m_SelectedCellType = iCellType;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::UpdateValueSelectedSubCellType(std::string iSubCellType)
{
  *this->m_SelectedSubCellType = iSubCellType;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsDockWidget::UpdateValueSelectedColor(
  ItemColorComboboxData iColorData)
{
  *this->m_SelectedColorData = iColorData;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoTraceSettingsDockWidget::GetCurrentSelectedCollectionID()
{
  return atoi( this->m_SelectedCollectionData->first.c_str() );
}