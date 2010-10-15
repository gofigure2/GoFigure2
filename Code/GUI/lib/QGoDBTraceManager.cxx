/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 1972 $  // Revision of last commit
  Date: $Date: 2010-08-16 12:23:05 -0400 (Mon, 16 Aug 2010) $  // Date of last commit
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

#include "QGoDBTraceManager.h"
#include <iostream>
#include <sstream>
#include <QMenu>
#include <QMessageBox>

QGoDBTraceManager::QGoDBTraceManager():
  m_Table(NULL), m_CollectionOfTraces(NULL), m_TraceContainerInfoForVisu(NULL)
{}

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
                                                          this->m_TraceName, this->m_CollectionOf, iImgSessionID);

  this->m_Table = new QGoTableWidget(iParent);
  QObject::connect( this->m_Table, SIGNAL( CheckedRowsChanged(int) ),
                    this, SLOT( UpdateHighlightedElementsInVisuContainer(int) ) );
  QObject::connect( this->m_Table, SIGNAL( VisibleRowsChanged(int) ),
                    this, SLOT( UpdateVisibleElementsInVisuContainer(int) ) );

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
std::list< unsigned int > QGoDBTraceManager::GetAllTracesIDForAGivenZCoord(
  vtkMySQLDatabase *iDatabaseConnector, int iZCoord)
{
  return GetColumnForBoundedValue(this->m_TraceNameID,
                                  this->m_TraceName, ConvertToString< unsigned int >(this->m_ImgSessionID),
                                  "ZCoord", ConvertToString< int >(iZCoord), iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::UpdateTWAndContainerForAllTraces(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForAllTraces(iDatabaseConnector);
  std::list< ContourMeshStructure > list_of_traces;
  GetTracesInfoFromDBAndModifyContainer(
    list_of_traces,
    iDatabaseConnector,
    this->m_TraceName, this->m_CollectionName, this->m_ImgSessionID);

  std::list< ContourMeshStructure >::iterator it = list_of_traces.begin();

  while ( it != list_of_traces.end() )
    {
    this->m_TraceContainerInfoForVisu->Insert(*it);
    ++it;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::UpdateTWAndContainerForExistingTraces(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
{
  this->DisplayInfoForExistingTrace(iDatabaseConnector, iTraceID);
  std::list< ContourMeshStructure > list_of_traces;
  GetTracesInfoFromDBAndModifyContainer(list_of_traces,
                                        iDatabaseConnector, this->m_TraceName,
                                        this->m_CollectionName, iTraceID);

  std::list< ContourMeshStructure >::iterator it = list_of_traces.begin();
  while ( it != list_of_traces.end() )
    {
    this->m_TraceContainerInfoForVisu->Insert(*it);
    ++it;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::UpdateTWAndContainerForExistingTraces(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iTraceIDs)
{
  this->DisplayInfoForExistingTraces(iDatabaseConnector, iTraceIDs);
  std::vector< int >                  TraceIDs;
  std::list< unsigned int >::iterator iter = iTraceIDs.begin();
  while ( iter != iTraceIDs.end() )
    {
    TraceIDs.push_back(*iter);
    iter++;
    }
  //GetTracesInfoFromDBAndModifyContainer(this->m_TraceContainerInfoForVisu,iDatabaseConnector,
  //  this->m_TraceName,this->m_CollectionName,
  // this->m_ImgSessionID,-1,iTraceIDs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoDBTraceManager::UpdateTWAndContainerForLastCreatedTrace(
  vtkMySQLDatabase* iDatabaseConnector)
{
  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
  //ContourMeshStructure ToPush = GetTracesInfoFromDB(iDatabaseConnector,
  //  this->m_TraceName,this->m_CollectionName, this->m_ImgSessionID);
 // GetTracesInfoFromDB(iDatabaseConnector,
 //   this->m_TraceName,this->m_CollectionName, this->m_ImgSessionID);
  //pb: info from visu to
  //std::vector<int> ListIDs(1);
  //ListIDs.push_back(this->GetLastCreatedTraceID(iDatabaseConnector));
  //GetTracesInfoFromDBAndModifyContainer(this->m_TraceContainerInfoForVisu,
  //  iDatabaseConnector, this->m_TraceName,
  //  this->m_CollectionName, this->m_ImgSessionID,-1,ListIDs);
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::UpdateTWAndContainerForDeletedTraces(
  std::list< unsigned int > iTraceIDs)
{
  this->m_TraceContainerInfoForVisu->DeleteAllHighlightedElements();
  this->m_Table->DeleteCheckedRows(this->m_TraceNameID, iTraceIDs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBTraceManager::GetListHighlightedIDs()
{
  return this->m_TraceContainerInfoForVisu->GetHighlightedElementsTraceID();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::SetTracesInfoContainerForVisu(
  ContourMeshContainer *iContainerForVisu)
{
  this->m_TraceContainerInfoForVisu = iContainerForVisu;
  QObject::connect( this->m_TraceContainerInfoForVisu,
                    SIGNAL( TracePicked(uint, Qt::CheckState) ),
                    this,
                    SLOT( CheckTheTraceInTW(uint, Qt::CheckState) ) );
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
void QGoDBTraceManager::UpdateHighlightedElementsInVisuContainer(int iTraceID)
{
  this->m_TraceContainerInfoForVisu->
  UpdateElementHighlightingWithGivenTraceID(iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::UpdateVisibleElementsInVisuContainer(int iTraceID)
{
  this->m_TraceContainerInfoForVisu->
  UpdateElementVisibilityWithGivenTraceID(iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::AddGeneralActionsContextMenu(QMenu *iMenu)
{
  iMenu->addAction( tr("Check the selected %1s")
                    .arg( this->m_TraceName.c_str() ), this, SLOT( CheckSelectedRows() ) );
  iMenu->addAction( tr("Uncheck the selected %1s")
                    .arg( this->m_TraceName.c_str() ), this, SLOT( UncheckSelectedRows() ) );
  iMenu->addAction( tr("Show the selected %1s")
                    .arg( this->m_TraceName.c_str() ), this, SLOT( ShowSelectedRows() ) );
  iMenu->addAction( tr("Hide the selected %1s")
                    .arg( this->m_TraceName.c_str() ), this, SLOT( HideSelectedRows() ) );

  iMenu->addAction( tr("Change the color for the checked %1 to the selected color").arg( this->m_TraceName.c_str() ),
                    this, SLOT( ChangeTraceColor() ) );
  iMenu->addAction( tr("Delete checked %1s").arg( this->m_TraceName.c_str() ),
                    this, SLOT( DeleteTracesFromContextMenu() ) );

  iMenu->addAction( tr("Add to selected %1%2").arg( this->m_CollectionName.c_str() )
                    .arg("ID"), this, SLOT( AddToSelectedCollection() ) );

  iMenu->addAction( tr("Copy Selection"),
                    this->m_Table, SLOT( CopySelection() ) );
  iMenu->addAction( tr("Copy table"), this->m_Table, SLOT( CopyTable() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::AddSpecificActionsForContourMesh(QMenu *iMenu)
{
  /** \todo when using lineages, remove the following*/
  //for the time being, as we don't use lineages
  iMenu->addAction( tr("Go to this %1")
                    .arg( this->m_TraceName.c_str() ),
                    this, SLOT( GoToTheTrace() ) );
  /** \todo when using lineage, put it in the generalActionsContextMenu*/
  this->AddActionForCreateNewCollectionFromCheckedTraces(iMenu);
  /*iMenu->addAction(
      tr("Add to selected %1 : %2").arg(this->m_CollectionName.c_str())
      .arg(this->m_SelectedCollectionData.first.c_str()), this, SLOT(AddToSelectedCollection()));
  iMenu->addAction(tr("ReEdit the checked %1").arg(this->m_TraceName.c_str()),
                           this, SLOT(ReEditTrace()));*/
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::AddActionForCreateNewCollectionFromCheckedTraces(
  QMenu *iMenu)
{
  iMenu->addAction( tr("Create a new %1 from checked %2s")
                    .arg( this->m_CollectionName.c_str() )
                    .arg( this->m_TraceName.c_str() ),
                    this, SLOT( CreateCorrespondingCollection() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::CheckSelectedRows()
{
  this->m_Table->CheckSelectedRows(this->m_TraceName, this->m_TraceNameID);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::UncheckSelectedRows()
{
  this->m_Table->UncheckSelectedRows(this->m_TraceName, this->m_TraceNameID);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::ShowSelectedRows()
{
  this->m_Table->ShowSelectedRows(this->m_TraceName, this->m_TraceNameID);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::HideSelectedRows()
{
  this->m_Table->HideSelectedRows(this->m_TraceName, this->m_TraceNameID);
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
std::list< unsigned int > QGoDBTraceManager::GetListTracesIDsFromThisCollectionOf(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTraces)
{
  return this->m_CollectionOfTraces->GetListTracesIDsFromThisCollectionOf(
           iDatabaseConnector, iListTraces);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::DisplayInfoForExistingTraces(vtkMySQLDatabase *
                                                     iDatabaseConnector, std::list< unsigned int > iListTraces)
{
  std::list< unsigned int >::iterator iter = iListTraces.begin();
  while ( iter != iListTraces.end() )
    {
    this->DisplayInfoForExistingTrace(iDatabaseConnector, *iter);
    iter++;
    }
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::DeleteTracesFromContextMenu()
{
  std::list< unsigned int > ListTracesIDToDelete =
    this->m_TraceContainerInfoForVisu->GetHighlightedElementsTraceID();

  if ( ListTracesIDToDelete.empty() )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1 to be deleted")
      .arg( this->m_TraceName.c_str() ) );
    msgBox.exec();
    }
  else
    {
    int r = QMessageBox::warning(this->m_Table, tr(""),
                                 tr("Are you sure you want to delete\n"
                                    "permanently the selected %1s?").arg( this->m_TraceName.c_str() ),
                                 QMessageBox::Yes,
                                 QMessageBox::No | QMessageBox::Default);
    if ( r == QMessageBox::Yes )
      {
      //as it impacts also on the collection and the collectionOf,
      //a signal has to be emitted for another traceManager:
      emit TracesToDelete();
      }
    }
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::DeleteTraces(
  vtkMySQLDatabase *iDatabaseConnector)
{
  std::list< unsigned int > ListTracesIDs =
    this->m_TraceContainerInfoForVisu->GetHighlightedElementsTraceID();
  this->m_CollectionOfTraces->DeleteTracesInDB(
    ListTracesIDs, iDatabaseConnector);
  this->UpdateTWAndContainerForDeletedTraces(ListTracesIDs);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::UpdateCollectionID(vtkMySQLDatabase *iDatabaseConnector,
                                           std::list< unsigned int > iListTracesIDs, int iCollectionID)
{
  this->m_CollectionOfTraces->UpdateCollectionIDOfSelectedTraces(iListTracesIDs,
                                                                 iCollectionID, iDatabaseConnector);
  this->DisplayInfoForExistingTraces(iDatabaseConnector, iListTracesIDs);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
std::list< unsigned int > QGoDBTraceManager::GetListCollectionIDs(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTracesIDs)
{
  return this->m_CollectionOfTraces->GetListCollectionIDs(
           iDatabaseConnector, iListTracesIDs);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
void QGoDBTraceManager::UpdateBoundingBoxes(vtkMySQLDatabase *iDatabaseConnector,
                                            std::list< unsigned int > iListTracesIDs)
{
  this->m_CollectionOfTraces->
  RecalculateDBBoundingBox(iDatabaseConnector, iListTracesIDs);
  this->DisplayInfoForExistingTraces(iDatabaseConnector, iListTracesIDs);
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
double * QGoDBTraceManager::GetVectorFromQColor(QColor iColor)
{
  qreal r, g, b, a;

  iColor.getRgbF(&r, &g, &b, &a);
  double *rgba = new double[4];
  rgba[0] = r;
  rgba[1] = g;
  rgba[2] = b;
  rgba[3] = a;
  return rgba;
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
std::list< QGoDBTraceManager::NameWithColorData >
QGoDBTraceManager::GetAllTraceIDsWithColor(
  vtkMySQLDatabase *iDatabaseConnector, int iTimePoint)
{
  if ( iTimePoint == -1 )
    {
    return this->m_CollectionOfTraces->GetAllTracesIDsWithColor(
             iDatabaseConnector);
    }
  else
    {
    return this->m_CollectionOfTraces->GetTracesIDsWithColorForATimePoint(
             iDatabaseConnector, static_cast< unsigned int >( iTimePoint ) );
    }
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
void QGoDBTraceManager::GoToTheTrace()
{
  std::list< unsigned int > ListCheckedTraces =
    this->m_TraceContainerInfoForVisu->GetHighlightedElementsTraceID();
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

  emit NeedToGoToTheLocation( atoi( CoordCenter.GetMapValue("XCoord").c_str() ),
                              atoi( CoordCenter.GetMapValue("YCoord").c_str() ),
                              atoi( CoordCenter.GetMapValue("ZCoord").c_str() ),
                              atoi( CoordCenter.GetMapValue("TCoord").c_str() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTraceManager::CreateCorrespondingCollection()
{
  emit NewCollectionFromCheckedTraces(
    this->m_TraceContainerInfoForVisu->GetHighlightedElementsTraceID() );
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
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
  emit CheckedTracesToAddToSelectedCollection(
    this->m_TraceContainerInfoForVisu->GetHighlightedElementsTraceID() );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBTraceManager::GetLastCreatedTracesIDs(
  vtkMySQLDatabase *iDatabaseConnector, int iNumberOfTraceIDs)
{
  return this->m_CollectionOfTraces->GetLastCreatedTracesIDs(
           iDatabaseConnector, iNumberOfTraceIDs);
}
