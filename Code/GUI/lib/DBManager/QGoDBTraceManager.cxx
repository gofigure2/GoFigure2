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

#include "QGoDBTraceManager.h"
#include <iostream>
#include <sstream>
#include <QMenu>
#include <QMessageBox>

QGoDBTraceManager::QGoDBTraceManager(QObject *iParent) :
  QObject(iParent), m_SelectedCollectionData(NULL),
  m_SelectedColorData(NULL), m_CurrentTimePoint(NULL), m_Table(NULL),
  m_CollectionOfTraces(NULL), m_DatabaseConnector(NULL),
  m_IsColorCodingOn(false), m_IsShowOnlyCurrentTimePointOn(false)
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDBTraceManager::~QGoDBTraceManager()
{
  if ( this->m_CollectionOfTraces )
    {
    delete this->m_CollectionOfTraces;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::SetInfo(unsigned int iImgSessionID, QWidget *iParent)
{
  SetCollectionsTraceNames();
  this->m_TraceNameID = this->GetTheNameIDFromName(this->m_TraceName);
  this->m_CollectionNameID = this->GetTheNameIDFromName(this->m_CollectionName);
  this->m_CollectionOfID = this->GetTheNameIDFromName(this->m_CollectionOf);

  this->m_CollectionOfTraces = new GoDBCollectionOfTraces(this->m_CollectionName,
                                                          this->m_TraceName,
                                                          this->m_CollectionOf,
                                                          iImgSessionID);

  this->m_Table = new QGoTableWidget(iParent);
  QObject::connect( this->m_Table, SIGNAL( CheckedRowsChanged(int) ),
                    this, SLOT( UpdateHighlightedElementsInVisuContainer(int) ) );
  QObject::connect( this->m_Table, SIGNAL( VisibleRowsChanged(int) ),
                    this, SLOT( UpdateVisibleElementsInVisuContainer(int) ) );

  QObject::connect( this->m_Table, SIGNAL(customContextMenuRequested (const QPoint &) ),
                    this, SLOT(	CreateContextMenu ( const QPoint &) ) );

  this->m_ImgSessionID = iImgSessionID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoDBTraceManager::GetTheNameIDFromName(std::string iName)
{
  std::string oNameID = iName;

  oNameID += "ID";
  return oNameID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTableWidget * QGoDBTraceManager::GetTableWidget()
{
  return this->m_Table;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoDBTraceManager::GetLastCreatedTraceID(
  vtkMySQLDatabase *iDatabaseConnector)
{
  return MaxValueForOneColumnInTable( iDatabaseConnector,
                                      this->m_TraceNameID, this->m_TraceName, "ImagingSessionID",
                                      ConvertToString< unsigned int >(this->m_ImgSessionID) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::CreateContextMenu(const QPoint & iPos)
{
  QMenu *ContextMenu = new QMenu;

  this->AddActionsContextMenu(ContextMenu);
  ContextMenu->exec( this->m_Table->mapToGlobal(iPos) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::AddActionsContextMenu(QMenu *iMenu)
{
  this->AddGeneralActionsContextMenu(iMenu);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::AddGeneralActionsContextMenu(QMenu *iMenu)
{
  m_CheckedTracesMenu = new QMenu( tr("With the checked %1s").arg( this->m_TraceName.c_str() ) );
  m_CheckedTracesMenu->addAction( tr("Delete them"),
                                  this, SLOT( DeleteTracesFromContextMenu() ) );
  //m_CheckedTracesMenu->addAction( tr("Add to selected %1 %2").arg(
  // this->m_CollectionName.c_str() )
  //                    .arg(this->m_SelectedCollectionData->first.c_str()),
  // this, SLOT( AddToSelectedCollection() ) );
  m_CheckedTracesMenu->addAction( tr("Change their color to the selected one : %1")
                                  .arg( this->m_SelectedColorData->first.c_str() ),
                                  this, SLOT( ChangeTraceColor() ) );
  iMenu->addAction( this->m_CheckedTracesMenu->menuAction() );

  QMenu *SelectedTracesMenu = new QMenu( tr("With the selected %1s").arg( this->m_TraceName.c_str() ) );
  SelectedTracesMenu->addAction( tr("Check the selected %1s")
                                 .arg( this->m_TraceName.c_str() ), this, SLOT( CheckSelectedRows() ) );
  SelectedTracesMenu->addAction( tr("Uncheck the selected %1s")
                                 .arg( this->m_TraceName.c_str() ), this, SLOT( UncheckSelectedRows() ) );
  SelectedTracesMenu->addAction( tr("Show the selected %1s")
                                 .arg( this->m_TraceName.c_str() ), this, SLOT( ShowSelectedRows() ) );
  SelectedTracesMenu->addAction( tr("Hide the selected %1s")
                                 .arg( this->m_TraceName.c_str() ), this, SLOT( HideSelectedRows() ) );

  iMenu->addAction( SelectedTracesMenu->menuAction() );

  QMenu *ColorMenu = new QMenu( tr("Change color of your %1s").arg( this->m_TraceName.c_str() ) );

  ColorMenu->addAction( tr("To the selected color for the checked %1s")
                        .arg( this->m_TraceName.c_str() ),
                        this, SLOT( ChangeTraceColor() ) );

  QAction *ColorCoding = new QAction(tr("Based on the selected column"), ColorMenu);
  ColorCoding->setCheckable(true);
  ColorCoding->setChecked(m_IsColorCodingOn);
  QObject::connect( ColorCoding, SIGNAL( triggered (bool) ), this, SLOT( SetColorCoding(bool) ) );
  ColorMenu->addAction(ColorCoding);
  iMenu->addAction( ColorMenu->menuAction() );

  iMenu->addAction( tr("Copy Selection"),
                    this->m_Table, SLOT( CopySelection() ) );
  iMenu->addAction( tr("Copy table"), this->m_Table, SLOT( CopyTable() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::AddSpecificActionsForContourMesh(QMenu *iMenu)
{
  /** \todo Lydie: when using lineages, remove the following*/
  //for the time being, as we don't use lineages
  iMenu->addAction( tr("Go to this %1")
                    .arg( this->m_TraceName.c_str() ),
                    this, SLOT( GoToTheTrace() ) );
  QAction *ShowCurrentTimePoint = new QAction(tr("Show only in the table the %1 for the current timepoint")
                                              .arg( this->m_TraceName.c_str() ), iMenu);
  ShowCurrentTimePoint->setCheckable(true);
  ShowCurrentTimePoint->setChecked(this->m_IsShowOnlyCurrentTimePointOn);
  QObject::connect( ShowCurrentTimePoint,
                    SIGNAL( triggered (bool) ),
                    this,
                    SLOT( ShowOnlyRowsForCurrentTimePoint(bool) ) );
  iMenu->addAction(ShowCurrentTimePoint);
  /** \todo Lydie: when using lineage, put it in the generalActionsContextMenu*/
  this->AddActionForAddingCheckedTracesToCollection();
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::AddActionForAddingCheckedTracesToCollection()
{
  this->m_CheckedTracesMenu->addAction( tr("Add to selected %1 %2")
                                        .arg( this->m_CollectionName.c_str() )
                                        .arg( this->m_SelectedCollectionData->first.c_str() ),
                                        this, SLOT( AddToSelectedCollection() ) );

  this->m_CheckedTracesMenu->addAction( tr("Create a new %1 from checked %2s")
                                        .arg( this->m_CollectionName.c_str() )
                                        .arg( this->m_TraceName.c_str() ),
                                        this, SLOT( CreateCorrespondingCollection() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::CheckSelectedRows()
{
  this->m_Table->ChangeCheckStateSelectedRows(this->m_TraceName,
                                              this->m_TraceNameID, Qt::Checked);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::UncheckSelectedRows()
{
  this->m_Table->ChangeCheckStateSelectedRows(this->m_TraceName,
                                              this->m_TraceNameID, Qt::Unchecked);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::ShowSelectedRows()
{
  this->m_Table->ChangeVisibilityStateSelectedRows(this->m_TraceName,
                                                   this->m_TraceNameID, Qt::Checked);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::HideSelectedRows()
{
  this->m_Table->ChangeVisibilityStateSelectedRows(this->m_TraceName,
                                                   this->m_TraceNameID, Qt::Unchecked);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::ChangeTraceColor()
{
  //as it impacts also the collectionof, a signal has to be emitted:
  //but not for contourManager (no collectionof) where ChangeTraceColor()
  //is then reimplemented
  emit TraceColorToChange();
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
std::list< unsigned int >
QGoDBTraceManager::GetListTracesIDsFromThisCollectionOf(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::list< unsigned int > & iListTraces)
{
  return this->m_CollectionOfTraces->GetListTracesIDsFromThisCollectionOf(
           iDatabaseConnector, iListTraces);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
std::list< unsigned int >
QGoDBTraceManager::GetListTracesIDsBelongingToCollectionIDs(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::list< unsigned int > & iListCollectionIDs)
{
  return this->m_CollectionOfTraces->GetTraceIDsBelongingToCollectionID(iDatabaseConnector,
                                                                        iListCollectionIDs);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void
QGoDBTraceManager::
DisplayInfoForExistingTraces(vtkMySQLDatabase * iDatabaseConnector,
                             const std::list< unsigned int > & iListTraces)
{
  this->m_Table->setSortingEnabled(false);
  std::list< unsigned int >::const_iterator iter = iListTraces.begin();
  while ( iter != iListTraces.end() )
    {
    this->DisplayInfoForExistingTrace(iDatabaseConnector, *iter);
    ++iter;
    }
  this->m_Table->setSortingEnabled(true);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
bool QGoDBTraceManager::CheckThatThereAreTracesToDelete(
  const std::list<unsigned int> & iListTracesIDToDelete)
{

  if ( iListTracesIDToDelete.empty() )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please check at least one %1 to be deleted")
      .arg( this->m_TraceName.c_str() ) );
    msgBox.exec();
    return false;
    }
  else
    {
    switch( QMessageBox::warning(this->m_Table, tr(""),
                                 tr("Are you sure you want to delete\n"
                                    "permanently the selected %1s?").arg( this->m_TraceName.c_str() ),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No))
      {
      case QMessageBox::Yes:
        return true;
      case QMessageBox::No:
        return false;
      default:
        return false;
      }
    }
}
//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::DeleteTracesFromContextMenu()
{
  std::list< unsigned int > ListTracesIDToDelete =
    this->GetListHighlightedIDs();

  if (this->CheckThatThereAreTracesToDelete(ListTracesIDToDelete) )
      {
      //as it impacts also on the collection and the collectionOf,
      //a signal has to be emitted for another traceManager:
      emit CheckedTracesToDelete();
      }
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void
QGoDBTraceManager::UpdateCollectionID(vtkMySQLDatabase *iDatabaseConnector,
                                      const std::list< unsigned int > & iListTracesIDs,
                                      int iCollectionID)
{
  this->m_CollectionOfTraces->UpdateCollectionIDOfSelectedTraces(iListTracesIDs,
                                                                 iCollectionID, iDatabaseConnector);
  this->DisplayInfoForExistingTraces(iDatabaseConnector, iListTracesIDs);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
std::list< unsigned int >
QGoDBTraceManager::GetListCollectionIDs(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::list< unsigned int > & iListTracesIDs)
{
  return this->m_CollectionOfTraces->GetListCollectionIDs(
           iDatabaseConnector, iListTracesIDs);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void
QGoDBTraceManager::UpdateBoundingBoxes(vtkMySQLDatabase *iDatabaseConnector,
                                       const std::list< unsigned int > & iListTracesIDs,
                                       bool UpdateTW)
{
  this->m_CollectionOfTraces->RecalculateDBBoundingBox(iDatabaseConnector, iListTracesIDs);

  if ( UpdateTW )
    {
    this->DisplayInfoForExistingTraces(iDatabaseConnector, iListTracesIDs);
    }
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
double * QGoDBTraceManager::GetVectorFromQColor(QColor iColor)
{
  qreal r, g, b, a;

  iColor.getRgbF(&r, &g, &b, &a);
  double *rgba = new double[4];
  rgba[0] = static_cast< double >( r );
  rgba[1] = static_cast< double >( g );
  rgba[2] = static_cast< double >( b );
  rgba[3] = static_cast< double >( a );
  return rgba;
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
std::list< QGoDBTraceManager::NameWithColorData >
QGoDBTraceManager::GetAllTraceIDsWithColor(
  vtkMySQLDatabase *iDatabaseConnector, std::string & ioIDToSelect)
{
  ioIDToSelect = this->m_LastSelectedTraceAsCollection;

  return this->m_CollectionOfTraces->GetAllTracesIDsWithColor(
           iDatabaseConnector);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::CheckTheTraceInTW(unsigned int iTraceID,
                                          Qt::CheckState iState)
{
  this->m_Table->SetCheckStateForTraceID(iTraceID,
                                         this->m_TraceName, iState, false);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::ShowTheTraceInTW(unsigned int iTraceID,
                                         Qt::CheckState iState)
{
  this->m_Table->SetVisibleStateForTraceID(iTraceID,
                                           this->m_TraceName, iState, false);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::GoToTheTrace()
{
  std::list< unsigned int > ListCheckedTraces =
    this->GetListHighlightedIDs();
  if ( ListCheckedTraces.size() != 1 )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select one and only one %1 to go to")
      .arg( this->m_TraceName.c_str() ) );
    msgBox.exec();
    return;
    }
  GoDBCoordinateRow CoordCenter =
    this->m_Table->GetCoordinateCenterBoundingBox(ListCheckedTraces.front(),
                                                  this->m_TraceName);

  emit NeedToGoToTheLocation( CoordCenter.GetMapValue<int>("XCoord"),
                              CoordCenter.GetMapValue<int>("YCoord"),
                              CoordCenter.GetMapValue<int>("ZCoord"),
                              CoordCenter.GetMapValue<int>("TCoord") );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::CreateCorrespondingCollection()
{
  std::list< unsigned int > ListCheckedTraces =
    this->GetListHighlightedIDs();
  if ( ListCheckedTraces.empty() )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1 to create the %2")
      .arg( this->m_TraceName.c_str() )
      .arg( this->m_CollectionName.c_str() ) );
    msgBox.exec();
    return;
    }
  emit NewCollectionFromCheckedTraces(ListCheckedTraces);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

GoDBCoordinateRow QGoDBTraceManager::GetCoordinateFromInt(int iXCoord,
                                                          int iYCoord, int iZCoord, int iTCoord)
{
  GoDBCoordinateRow coord;

  coord.SetField< unsigned int >("XCoord", iXCoord);
  coord.SetField< unsigned int >("YCoord", iYCoord);
  coord.SetField< unsigned int >("ZCoord", iZCoord);
  coord.SetField< unsigned int >("TCoord", iTCoord);
  return coord;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::AddToSelectedCollection()
{
  std::list< unsigned int > ListCheckedTraces = this->GetListHighlightedIDs();
  if ( ListCheckedTraces.empty() )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please check at least one %1 to be part of the %2")
      .arg( this->m_TraceName.c_str() )
      .arg( this->m_CollectionName.c_str() ) );
    msgBox.exec();
    return;
    }
  emit CheckedTracesToAddToSelectedCollection(
    this->GetListHighlightedIDs() );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBTraceManager::GetLastCreatedTracesIDs(
  vtkMySQLDatabase *iDatabaseConnector, int iNumberOfTraceIDs)
{
  return this->m_CollectionOfTraces->GetLastCreatedTracesIDs(
           iDatabaseConnector, iNumberOfTraceIDs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::SetDatabaseConnection(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->m_DatabaseConnector = iDatabaseConnector;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::SetSelectedCollection(NameWithColorData *iCollectionData)
{
  this->m_SelectedCollectionData = iCollectionData;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::SetCurrentTimePoint(int *iTimePoint)
{
  this->m_CurrentTimePoint = iTimePoint;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::SetSelectedColor(NameWithColorData *iColorData)
{
  this->m_SelectedColorData = iColorData;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::ShowOnlyRowsForCurrentTimePoint(bool IsChecked)
{
  this->m_IsShowOnlyCurrentTimePointOn = IsChecked;

  if ( IsChecked )
    {
    this->m_Table->ShowOnlyRowsForTimePoint(*this->m_CurrentTimePoint);
    }
  else
    {
    this->m_Table->ShowAllRows();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::CheckShowRows()
{
  if ( this->m_IsShowOnlyCurrentTimePointOn )
    {
    this->ShowOnlyRowsForCurrentTimePoint(true);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::UpdateLastSelectedOneAsCollection()
{
  this->m_LastSelectedTraceAsCollection =
    this->m_SelectedCollectionData->first;
}
//-------------------------------------------------------------------------
