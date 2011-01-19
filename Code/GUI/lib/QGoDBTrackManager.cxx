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

#include "QGoDBTrackManager.h"
#include "GoDBTrackRow.h"
#include <iostream>
#include <sstream>

QGoDBTrackManager::QGoDBTrackManager(int iImgSessionID, QWidget *iparent):
  QGoDBTraceManager(), m_TrackContainerInfoForVisu(NULL)
{
  this->SetInfo(iImgSessionID, iparent);
  this->m_TWContainer = new GoDBTWContainerForTrack( iImgSessionID );
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
  this->DisplayInfoForAllTracesTemplate< GoDBTWContainerForTrack >(
    this->m_TWContainer, iDatabaseConnector,Qt::Unchecked);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DisplayInfoAndLoadVisuContainerForAllTracks(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoAndLoadVisuContainerWithAllTraces< GoDBTWContainerForTrack >
    (this->m_TWContainer,iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoDBTrackManager::DisplayInfoForLastCreatedTrack(
  vtkMySQLDatabase *iDatabaseConnector, GoFigureTrackAttributes *iTrackAttributes)
{
  this->m_TWContainer->SetTrackAttributes(iTrackAttributes);
  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
}*/

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DisplayInfoForLastCreatedTrace(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForLastCreatedTraceTemplate< GoDBTWContainerForTrack >(
    this->m_TWContainer, iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DisplayInfoForExistingTrace(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
{
  this->DisplayInfoForExistingTraceTemplate< GoDBTWContainerForTrack >(
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
  NameWithColorData NewTrackData(ConvertToString<unsigned int> (NewTrackID),
    this->m_SelectedColorData->second);
  emit AddNewTraceIDInTM( NewTrackData );
  return NewTrackID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBTrackManager::UpdateTheTracesColor(
  vtkMySQLDatabase *iDatabaseConnector)
{
  return this->UpdateTheTracesColorTemplate< GoDBTrackRow,
    TrackContainer >(iDatabaseConnector, this->m_TrackContainerInfoForVisu);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdateTWAndContainerForImportedTraces(
  std::vector< int > iVectorImportedTraces, vtkMySQLDatabase *iDatabaseConnector)
{
  this->UpdateTWAndContainerWithImportedTracesTemplate<
    GoDBTWContainerForTrack >(this->m_TWContainer,
    iVectorImportedTraces, iDatabaseConnector);
  //call the TrackContainer to give him iVectorImportedTraces
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DeleteCheckedTraces(vtkMySQLDatabase *iDatabaseConnector)
{
  this->DeleteTracesTemplate<TrackContainer>(iDatabaseConnector,
    this->m_TrackContainerInfoForVisu);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DeleteListTraces(vtkMySQLDatabase *iDatabaseConnector,
  std::list<unsigned int> iListTraces)
{
  this->DeleteTracesTemplate<TrackContainer>(iDatabaseConnector,
    this->m_TrackContainerInfoForVisu, iListTraces, false);
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
  vtkMySQLDatabase* iDatabaseConnector,
  std::list<unsigned int> iListTraceIDs)
{
  this->GetTracesInfoFromDBAndModifyContainerForVisuTemplate<TrackContainer>(
    this->m_TrackContainerInfoForVisu, iDatabaseConnector, iListTraceIDs);
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

  //calculate the values to be put in the table widget:
  this->m_TWContainer->SetTrackAttributes(
      &this->m_TrackContainerInfoForVisu->m_CurrentElement.ComputeAttributes());

  //update the table widget:
  if (TrackID == 0)
    {
    this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
    }
  else
    {
    this->DisplayInfoForExistingTrace(iDatabaseConnector, TrackID);
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
  QGoDBTraceManager::UpdateBoundingBoxes(iDatabaseConnector,iListTracesIDs, false);
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
      tr("Please select one and only one Track to be split"));
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
  std::list<unsigned int> HighlightedTrackIDs = 
    this->m_TrackContainerInfoForVisu->GetHighlightedElementsTraceID();

  if (HighlightedTrackIDs.size() == 0)
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one Track to be visualized in the widget"));
    msgBox.exec();
    }
  else
    {
    emit TrackIDToBeModifiedWithWidget(HighlightedTrackIDs);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DisplayOnlyCalculatedValuesForExistingTrack(
  GoFigureTrackAttributes *iTrackAttributes, unsigned iTrackID)
{
  if ( iTrackAttributes != 0 )
    {
    std::vector< std::string > ColumnNames (6);
    std::vector< std::string > Values (6);

    ColumnNames.at(0) = "Deplacement";
    Values.at(0) = ConvertToString< double >(iTrackAttributes->total_length);
    ColumnNames.at(1) = "Distance";
    Values.at(1) = ConvertToString< double >(iTrackAttributes->distance);
    ColumnNames.at(2) = "Theta";
    Values.at(2) = ConvertToString< double >(iTrackAttributes->theta);
    ColumnNames.at(3) = "Phi";
    Values.at(3) = ConvertToString< double >(iTrackAttributes->phi);
    ColumnNames.at(4) = "AvgSpeed";
    Values.at(4) = ConvertToString< double >(iTrackAttributes->avg_speed);
    ColumnNames.at(5) = "MaxSpeed";
    Values.at(5) = ConvertToString< double >(iTrackAttributes->max_speed);

    this->m_Table->AddValuesForID(ColumnNames, Values, iTrackID, "trackID");
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------