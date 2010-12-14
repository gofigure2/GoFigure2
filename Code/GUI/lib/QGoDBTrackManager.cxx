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

#include "QGoDBTrackManager.h"
#include "GoDBTrackRow.h"
#include <iostream>
#include <sstream>

QGoDBTrackManager::QGoDBTrackManager(int iImgSessionID, QWidget *iparent):
  QGoDBTraceManager(),m_TrackContainerInfoForVisu(NULL)
{
  this->SetInfo(iImgSessionID, iparent);
  this->m_TWContainer = new GoDBTWContainerForTrackLineage(this->m_TraceName,
                                                           this->m_CollectionName,
                                                           iImgSessionID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDBTrackManager::~QGoDBTrackManager()
{
  if ( this->m_TWContainer )
    {
    delete this->m_TWContainer;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::SetTracksInfoContainerForVisu(
  TrackContainer *iContainerForVisu)
{
  this->SetTracesInfoContainerForVisuTemplate<TrackContainer>(
    iContainerForVisu,&this->m_TrackContainerInfoForVisu);

  QObject::connect(this->m_TrackContainerInfoForVisu,
                   SIGNAL(NeedMeshesInfoForImportedTrack(unsigned int) ),
                   this,
                   SIGNAL(NeedMeshesInfoForImportedTrack(unsigned int) ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::SetCollectionsTraceNames()
{
  this->m_TraceName = "track";
  this->m_CollectionName = "lineage";
  this->m_CollectionOf = "mesh";
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DisplayInfoForAllTraces(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForAllTracesTemplate< GoDBTWContainerForTrackLineage >(
    this->m_TWContainer, iDatabaseConnector,Qt::Unchecked);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DisplayInfoAndLoadVisuContainerForAllTracks(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoAndLoadVisuContainerWithAllTraces< GoDBTWContainerForTrackLineage >
    (this->m_TWContainer,iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DisplayInfoForLastCreatedTrace(vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForLastCreatedTraceTemplate< GoDBTWContainerForTrackLineage >(
    this->m_TWContainer, iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DisplayInfoForExistingTrace(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
{
  this->DisplayInfoForExistingTraceTemplate< GoDBTWContainerForTrackLineage >(
    this->m_TWContainer, iDatabaseConnector, iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBTrackManager::CreateNewTrackWithNoMesh(
  vtkMySQLDatabase *iDatabaseConnector)
{
  GoDBTrackRow NewTrack;
  unsigned int NewTrackID =
    this->m_CollectionOfTraces->CreateCollectionWithNoTracesNoPoints< GoDBTrackRow >(
      iDatabaseConnector, *this->m_SelectedColorData, NewTrack);

  this->m_TrackContainerInfoForVisu->ResetCurrentElement();
  this->m_TrackContainerInfoForVisu->UpdateCurrentElementFromDB(
   NewTrackID, this->GetVectorFromQColor(this->m_SelectedColorData->second),true);
  this->m_TrackContainerInfoForVisu->InsertCurrentElement();
  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
  emit RefreshListCollectionIDsTM ( ConvertToString<unsigned int> (NewTrackID),
    iDatabaseConnector);
  return NewTrackID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBTrackManager::UpdateTheTracesColor(
  vtkMySQLDatabase *iDatabaseConnector)
{
  return this->UpdateTheTracesColorTemplate< GoDBTrackRow,
    TrackContainer >(iDatabaseConnector,this->m_TrackContainerInfoForVisu);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdateTWAndContainerForImportedTraces(
  std::vector< int > iVectorImportedTraces, vtkMySQLDatabase *iDatabaseConnector)
{
  this->UpdateTWAndContainerWithImportedTracesTemplate<
    GoDBTWContainerForTrackLineage>(this->m_TWContainer,
    iVectorImportedTraces, iDatabaseConnector);
  //call the TrackContainer to give him iVectorImportedTraces
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DeleteTraces(vtkMySQLDatabase *iDatabaseConnector)
{
  this->DeleteTracesTemplate<TrackContainer>(iDatabaseConnector,
    this->m_TrackContainerInfoForVisu);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBTrackManager::GetListHighlightedIDs()
{
  return this->m_TrackContainerInfoForVisu->GetHighlightedElementsTraceID();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdateHighlightedElementsInVisuContainer(
  int iTraceID)
{
  this->m_TrackContainerInfoForVisu->
    UpdateElementHighlightingWithGivenTraceID(iTraceID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdateVisibleElementsInVisuContainer(int iTraceID)
{
  this->m_TrackContainerInfoForVisu->
    UpdateElementVisibilityWithGivenTraceID(iTraceID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::GetTracesInfoFromDBAndModifyContainerForVisu(
  vtkMySQLDatabase* iDatabaseConnector,std::vector<int> iVectIDs)
{
  std::list<TrackStructure> list_of_traces;
  GetTracesInfoFromDBAndModifyContainer(
      list_of_traces,
      iDatabaseConnector, this->m_TraceName, this->m_CollectionName,
      this->m_ImgSessionID,iVectIDs);

  std::list< TrackStructure >::iterator it = list_of_traces.begin();

  while ( it != list_of_traces.end() )
    {
    this->m_TrackContainerInfoForVisu->Insert(*it);
    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::SaveTrackCurrentElement(
  vtkMySQLDatabase* iDatabaseConnector)
{
  GoDBTrackRow TrackToSave(this->m_ImgSessionID);
  unsigned int TrackID = this->m_TrackContainerInfoForVisu->m_CurrentElement.TraceID;
  if (TrackID != 0)
    {
    TrackToSave.SetValuesForSpecificID(TrackID,iDatabaseConnector);
    }

  //save the track into the database
  TrackToSave.SetThePointsFromPolydata(
    this->m_TrackContainerInfoForVisu->m_CurrentElement.Nodes);
  TrackID = TrackToSave.SaveInDB(iDatabaseConnector);
  
  //save the track into the container:
  this->m_TrackContainerInfoForVisu->InsertCurrentElement();

  if (TrackID == 0)
    {
    this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdatePointsOfCurrentElementForImportedTrack(
	  std::map<unsigned int,double*> iMeshesInfo)
{
	this->m_TrackContainerInfoForVisu->UpdateCurrentElementMap(iMeshesInfo);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdateTrackPolydataForVisu(vtkMySQLDatabase *iDatabaseConnector,
  unsigned int iTrackID)
{
  std::list<double*> ListCenters =
    this->m_CollectionOfTraces->GetCoordinateCenterBoundingBox(
    iDatabaseConnector,iTrackID);
  this->m_TrackContainerInfoForVisu->UpdatePointsForATrack(iTrackID,ListCenters);
  this->SaveTrackCurrentElement(iDatabaseConnector);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdateBoundingBoxes(
  vtkMySQLDatabase *iDatabaseConnector,std::list< unsigned int > iListTracesIDs)
{
  QGoDBTraceManager::UpdateBoundingBoxes(iDatabaseConnector,iListTracesIDs);
  std::list<unsigned int>::iterator iter = iListTracesIDs.begin();
  while(iter != iListTracesIDs.end())
    {
    this->UpdateTrackPolydataForVisu(iDatabaseConnector,*iter);
    iter++;
    }
}
//-------------------------------------------------------------------------
void QGoDBTrackManager::SetColorCoding(bool IsChecked)
{
  this->SetColorCodingTemplate<TrackContainer>(
    this->m_TrackContainerInfoForVisu,IsChecked);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::AddActionsContextMenu(QMenu *iMenu)
{
  QGoDBTraceManager::AddActionsContextMenu(iMenu);
  QMenu* SplitMenu = new QMenu(tr("Split"),iMenu);

  SplitMenu->addAction(tr("Your track"), this, SLOT(TrackIDToEmit() ) );
  SplitMenu->addAction(tr("Using the Widget"),
                       this, SLOT( SplitTrackWithWidget() ) );
  iMenu->addAction(SplitMenu->menuAction() );
  //QAction* SplitMenuAction = SplitMenu->menuAction(); 
  //QAction* SplitMenuAction = new QAction(tr("Split your track"),iMenu);
  //QObject::connect(iMenu,activated(int) , SLOT(SelectionClick(int)
  //iMenu->connectItem(res, this, SLOT(SelectionClick(int)));

  //QObject::connect(SplitMenuAction, SIGNAL( triggered() ),
    //this, SLOT(TrackIDToEmit() ) );

  //iMenu->addAction(SplitMenuAction);
}
//-------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoDBTrackManager::TrackIDToEmit()
{
  std::list<unsigned int> HighlightedTrackIDs = 
    this->m_TrackContainerInfoForVisu->GetHighlightedElementsTraceID();

  if (HighlightedTrackIDs.size() != 1)
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select one and only one Track to be split")
      .arg( this->m_TraceName.c_str() ) );
    msgBox.exec();
    }
  else
    {
    emit NeedToGetDatabaseConnection();
    emit TrackToSplit(HighlightedTrackIDs.front(),
      this->GetListTracesIDsFromThisCollectionOf(this->m_DatabaseConnector,
        HighlightedTrackIDs) );
    emit DBConnectionNotNeededAnymore();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::SplitTrackWithWidget()
{
  std::list <std::pair<unsigned int, std::list<unsigned int> > >TrackIDWithMeshIDs;
  std::list<unsigned int> CheckedTracks = 
    this->m_TrackContainerInfoForVisu->GetHighlightedElementsTraceID();
  std::list<unsigned int>::iterator iter = CheckedTracks.begin();
  while (iter != CheckedTracks.end() )
  {
    std::pair<unsigned int, std::list<unsigned int> > temp;
    temp.first = *iter;
    std::list<unsigned int> TrackID;
    TrackID.push_back(*iter);
    temp.second = this->GetListTracesIDsFromThisCollectionOf(this->m_DatabaseConnector,TrackID);
    iter++;
  }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
