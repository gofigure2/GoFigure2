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

#include "QGoTraceSettingsWidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFont>

#include <iostream>

QGoTraceSettingsWidget::QGoTraceSettingsWidget(QWidget *iParent) :
  QWidget(iParent)
{
  this->SetUpUi();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTraceSettingsWidget::
~QGoTraceSettingsWidget()
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
void QGoTraceSettingsWidget::SetUpUi()
{
  QWidget* TraceSettingsWidget = new QWidget(this);

 // QHBoxLayout* ColorLayout = new QHBoxLayout;
  //SetSelectedColorComboBox(ColorLayout);
  //QHBoxLayout* TraceLayout = new QHBoxLayout;
  
  //QHBoxLayout* TraceCollectionLayout = new QHBoxLayout;
 // SetTraceCollectionColorComboBox(TraceCollectionLayout);

  //QHBoxLayout* CellTypeLayout = new QHBoxLayout;
  //SetCellTypeComboBox(CellTypeLayout);

  //QHBoxLayout* SubCellLayout = new QHBoxLayout;
  //SetSubCellTypeComboBox(SubCellLayout);

  QHBoxLayout* MainLayout = new QHBoxLayout;
  QLabel* Blank = new QLabel(this);

  //MainLayout->addLayout(TraceCollectionLayout);
  SetTraceCollectionColorComboBox(MainLayout, Blank);
  MainLayout->addWidget(Blank);
  SetSelectedColorComboBox(MainLayout);
  //MainLayout->addLayout(ColorLayout);
  MainLayout->addWidget(Blank);
  SetCellTypeComboBox(MainLayout);
  SetSubCellTypeComboBox(MainLayout);
  //MainLayout->addLayout(CellTypeLayout);
  //MainLayout->addLayout(SubCellLayout);

  this->SetWidgetFont();
  this->UpdateTraceAndCollection("contour", "mesh");

  this->setLayout(MainLayout);
  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsWidget::SetListCollectionID(
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
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetListColors(
  std::list< ItemColorComboboxData > iListColors, std::string iColorToSelect)
{
  this->SetListItemAndSelect< QGoSelectedColorComboBox >(this->m_SelectedColorComboBox,
                                                         iListColors, iColorToSelect);
  this->m_SelectedColorComboBox->SetCurrentItemAndActivate(iColorToSelect);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetListColorsWithSelectedOne(
  std::list< ItemColorComboboxData > iListColors)
{
  this->SetListColors(iListColors, this->m_SelectedColorData->first);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsWidget::SetListCellTypes(NamesDescrContainerType iCellTypesData,
                                              std::string iCellTypeToSelect)
{
  this->SetListItemAndSelect< QGoComboBox >(this->m_ChoseCellType,
                                            iCellTypesData, iCellTypeToSelect);
  this->m_ChoseCellType->SetCurrentItemAndActivate(iCellTypeToSelect);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsWidget::SetListCellTypeWithSelectedOne(
  NamesDescrContainerType iCellTypesData)
{
  this->SetListCellTypes(iCellTypesData, *this->m_SelectedCellType);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsWidget::SetListSubCellTypes(NamesDescrContainerType iSubCellTypesData,
                                                 std::string iSubCellTypeToSelect)
{
  this->SetListItemAndSelect< QGoComboBox >(this->m_ChoseSubCellType,
                                            iSubCellTypesData, iSubCellTypeToSelect);
  this->m_ChoseSubCellType->SetCurrentItemAndActivate(iSubCellTypeToSelect);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsWidget::SetListSubCellTypeWithSelectedOne(
  NamesDescrContainerType iSubCellTypesData)
{
  this->SetListSubCellTypes(iSubCellTypesData, *this->m_SelectedSubCellType);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsWidget::SetSelectedColorComboBox(
  QHBoxLayout* iColorLayout)
{
  QString Tooltip(tr("Color to be applied to your new traces") );

  this->m_SelectedColorComboBox = new QGoSelectedColorComboBox;
  this->m_SelectedColorComboBox->setToolTip(Tooltip);
  QLabel* ColorLbl = new QLabel(tr("Color:"), this);
  ColorLbl->setToolTip(Tooltip);
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
QGoTraceSettingsWidget::SetTraceCollectionColorComboBox(
  QHBoxLayout* iLayoutTraceCollection, QLabel* iLabel)
{
  //this->m_TraceName = new QLabel(this);
  QString Tooltip(tr("Current trace you are working on") );
  this->m_SelectedTrace = new QComboBox(this);
  this->m_SelectedTrace->setToolTip(Tooltip);

  QStringList ListTraces;
  ListTraces.append("contour");
  ListTraces.append("mesh");
  ListTraces.append("track");
  ListTraces.append("lineage");
  this->m_SelectedTrace->addItems(ListTraces);
  //QHBoxLayout *HLayoutForTrace = new QHBoxLayout;
  
  //this->m_TraceLbl = new QLabel(tr("Trace:"), this);
  //HLayoutForTrace->addWidget(this->m_TraceLbl);
  //HLayoutForTrace->addWidget(this->m_TraceName);
  
  //this->m_CollectionLbl = new QLabel( tr("Collection:") ); 
  QString Tooltip2(tr("Corresponding collection for the selected trace") );
   
  this->m_CollectionName = new QLabel ( tr("mesh") );
  this->m_CollectionName->setToolTip(Tooltip2);
  QString Tooltip3(tr("ID of the collection your new traces will belong to") );
  this->m_CollectionColorComboBox = new QGoCollectionColorComboBox;
  this->m_CollectionColorComboBox->setToolTip(Tooltip3);
  

  //QHBoxLayout *HLayoutForCollection = new QHBoxLayout;
  //HLayoutForCollection->addWidget(this->m_CollectionName);
  //HLayoutForCollection->addWidget(this->m_CollectionColorComboBox);
  //iLayoutTraceCollection->addLayout(HLayoutForTrace);
  //iLayoutTraceCollection->addWidget(this->m_CollectionLbl);
  iLayoutTraceCollection->addWidget(this->m_SelectedTrace);
  iLayoutTraceCollection->addWidget(iLabel);
  iLayoutTraceCollection->addWidget(this->m_CollectionName);
  iLayoutTraceCollection->addWidget(this->m_CollectionColorComboBox);
  //iLayoutTraceCollection->addLayout(HLayoutForCollection);

  this->m_SelectedCollectionData = new ItemColorComboboxData;

  QObject::connect( this->m_CollectionColorComboBox,
                    SIGNAL( ItemSelected(ItemColorComboboxData) ),
                    this, SLOT( UpdateValueSelectedCollection(ItemColorComboboxData) ) );

  QObject::connect( this->m_CollectionColorComboBox,
                    SIGNAL( NewCollectionToCreate() ),
                    this, SIGNAL( NewCollectionToBeCreated() ) );

  QObject::connect( this->m_SelectedTrace,
                    SIGNAL( currentIndexChanged ( int ) ),
                    this, SLOT ( TraceToUpdate( int ) ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTraceSettingsWidget::SetCellTypeComboBox(
  QHBoxLayout* iCellLayout)
{
  QString Tooltip(tr("Celltype that will be applied to your new meshes") );
  this->m_ChoseCellType = new QGoComboBox("Add a new celltype...",
                                          this, "Delete a celltype...");
  this->m_ChoseCellType->setToolTip(Tooltip);

  QHBoxLayout *HLayoutForCellType = new QHBoxLayout;
  this->m_LabelCellType = new QLabel(tr("CellType:"), this);
  this->m_LabelCellType->setToolTip(Tooltip);

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
QGoTraceSettingsWidget::SetSubCellTypeComboBox(
  QHBoxLayout* iSubCellLayout)
{
  QString Tooltip(tr("SubCellulartype that will be applied to your new meshes") );
  this->m_ChoseSubCellType = new QGoComboBox("Add a new subcelltype...", this,
                                             "Delete a subcelltype...");
  this->m_ChoseSubCellType->setToolTip(Tooltip);
  this->m_LabelSubCellType = new QLabel(tr("SubCellType:"), this);
  this->m_LabelSubCellType->setToolTip(Tooltip);
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
void QGoTraceSettingsWidget::SetCurrentCellType(
  std::string iCellTypeText)
{
  this->m_ChoseCellType->SetCurrentItem(iCellTypeText);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetCurrentCellTypeToSelectedOne()
{
  this->SetCurrentCellType(*this->m_SelectedCellType);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetCurrentSubCellType(
  std::string iSubCellTypeText)
{
  this->m_ChoseSubCellType->SetCurrentItem(iSubCellTypeText);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetCurrentSubCellTypeToSelectedOne()
{
  this->SetCurrentSubCellType(*this->m_SelectedSubCellType);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetCurrentColor(std::string iColorText)
{
  this->m_SelectedColorComboBox->SetCurrentItem(iColorText);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetCurrentColorToSelectedOne()
{
  this->SetCurrentColor(this->m_SelectedColorData->first);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetCurrentCollectionID(std::string iID)
{
  this->m_CollectionColorComboBox->SetCurrentItem(iID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::UpdateTraceAndCollection(std::string iTrace, 
  std::string iCollection)
{
  this->m_SelectedTrace->setCurrentIndex(
    this->m_SelectedTrace->findText(iTrace.c_str() ) );
  this->UpdateCollection(iCollection);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::UpdateCollection(
  //std::string iTrace, 
  std::string iCollection)
{
  //this->ChangeWindowTitle(iTrace);

  //this->m_TraceName->setText( iTrace.c_str() );
  this->m_CollectionName->setText( iCollection.c_str() );
  this->m_CollectionName->show();
  //this->m_CollectionLbl->show();
  
  //if ( iTrace == "contour" || iTrace == "mesh" )
  if (this->m_SelectedTrace->currentText() == "contour" || 
    this->m_SelectedTrace->currentText() == "mesh")
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

  //if (iTrace == "lineage")
  if (this->m_SelectedTrace->currentText() == "lineage")
    {
    this->m_CollectionName->hide();
    //this->m_CollectionLbl->hide();
    }
  // update visualization
  this->show();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoTraceSettingsWidget::ChangeWindowTitle(std::string iTrace)
{
  QString Title(tr("%1 Settings").arg(iTrace.c_str() ) );
  this->setWindowTitle(Title);
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetWidgetFont()
{
  //all widget:
  QFont Font;
  Font.setCapitalization(QFont::Capitalize);
  this->setFont(Font);

  //trace and collection name:
  Font.setCapitalization(QFont::AllUppercase);
  Font.setBold(true);
  //this->m_TraceName->setFont(Font);
  this->m_SelectedTrace->setFont(Font);
  this->m_CollectionName->setFont(Font);

  //trace and collection label:
  //Font.setCapitalization(QFont::Capitalize);
  //Font.setPointSize(8);
  //Font.setUnderline(true);
  //Font.setWeight(50);
  //Font.setBold(false);
  
  //this->m_TraceLbl->setFont(Font);
  //this->m_CollectionLbl->setFont(Font);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoTraceSettingsWidget::GetTraceName()
{
  //return this->m_TraceName->text().toStdString();
  return this->m_SelectedTrace->currentText().toStdString();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::AddANewCollectionID(
  std::pair<std::string, QColor> iNewCollectionID)
{
  this->m_CollectionColorComboBox->AddItemWithColor(iNewCollectionID, true);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string * QGoTraceSettingsWidget::GetPointerSelectedCellType()
{
  return this->m_SelectedCellType;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string * QGoTraceSettingsWidget::GetPointerSelectedSubCellType()
{
  return this->m_SelectedSubCellType;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTraceSettingsWidget::ItemColorComboboxData *
QGoTraceSettingsWidget::GetPointerCollectionData()
{
  return this->m_SelectedCollectionData;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTraceSettingsWidget::ItemColorComboboxData *
QGoTraceSettingsWidget::GetPointerColorData()
{
  return this->m_SelectedColorData;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::UpdateValueSelectedCollection(
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
void QGoTraceSettingsWidget::UpdateValueSelectedCellType(std::string iCellType)
{
  *this->m_SelectedCellType = iCellType;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::UpdateValueSelectedSubCellType(std::string iSubCellType)
{
  *this->m_SelectedSubCellType = iSubCellType;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::UpdateValueSelectedColor(
  ItemColorComboboxData iColorData)
{
  *this->m_SelectedColorData = iColorData;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoTraceSettingsWidget::GetCurrentSelectedCollectionID()
{
  return atoi( this->m_SelectedCollectionData->first.c_str() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::TraceToUpdate( int iIndexTrace ) 
{
  if (this->m_SelectedTrace->currentText() == "contour")
    {
    this->UpdateCollection("mesh");
    }
  if (this->m_SelectedTrace->currentText() == "mesh")
    {
    this->UpdateCollection("track");
    }
  if (this->m_SelectedTrace->currentText() == "track")
    {
    this->UpdateCollection("lineage");
    }
  if (this->m_SelectedTrace->currentText() == "lineage")
    {
    this->UpdateCollection("");
    }

  emit TraceChanged( iIndexTrace );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetPointerSelectedCellType(std::string* iCellType)
{
  this->m_SelectedCellType = iCellType;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetPointerSelectedSubCellType(std::string* iSubCellType)
{
  this->m_SelectedSubCellType = iSubCellType;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetPointerCollectionData(ItemColorComboboxData* iCollectionData)
{
  this->m_SelectedCollectionData = iCollectionData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetPointerColorData(ItemColorComboboxData* iColorData)
{
  this->m_SelectedColorData = iColorData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTraceSettingsWidget::SetSelectedPointersToNull()
{
  this->m_SelectedCollectionData = NULL;
  this->m_SelectedColorData = NULL;
  this->m_SelectedCellType = NULL;
  this->m_SelectedSubCellType = NULL;
}