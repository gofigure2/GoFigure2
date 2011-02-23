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

#include "QGoDBLineageManager.h"
#include "GoDBLineageRow.h"
#include <iostream>
#include <sstream>

QGoDBLineageManager::QGoDBLineageManager(int iImgSessionID, QWidget *iparent) :
  QGoDBTraceManager()//, m_TrackContainerInfoForVisu(NULL)
{
  this->SetInfo(iImgSessionID, iparent);
  this->m_TWContainer = new GoDBTWContainerForTrackLineage(this->m_TraceName,
                                                          this->m_CollectionName, iImgSessionID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDBLineageManager::~QGoDBLineageManager()
{
  if ( this->m_TWContainer )
    {
    delete this->m_TWContainer;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoDBTrackManager::SetTracksInfoContainerForVisu(
  TrackContainer *iContainerForVisu)
{
  this->SetTracesInfoContainerForVisuTemplate< TrackContainer >(
    iContainerForVisu, &this->m_TrackContainerInfoForVisu);

  QObject::connect( this->m_TrackContainerInfoForVisu,
                    SIGNAL( NeedMeshesInfoForImportedTrack(unsigned int) ),
                    this,
                    SIGNAL( NeedMeshesInfoForImportedTrack(unsigned int) ) );
}*/

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::SetCollectionsTraceNames()
{
  this->m_TraceName = "lineage";
  this->m_CollectionName = "None";
  this->m_CollectionOf = "track";
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DisplayInfoForAllTraces(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForAllTracesTemplate< GoDBTWContainerForTrackLineage >(
    this->m_TWContainer, iDatabaseConnector, Qt::Unchecked);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DisplayInfoAndLoadVisuContainerForAllLineages(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForAllTraces(iDatabaseConnector); //for the time being as there is no lineage container
  //this->DisplayInfoAndLoadVisuContainerWithAllTraces< GoDBTWContainerForTrackLineage >
  //  (this->m_TWContainer, iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DisplayInfoForLastCreatedTrace(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForLastCreatedTraceTemplate< GoDBTWContainerForTrackLineage >(
    this->m_TWContainer, iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DisplayInfoForExistingTrace(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
{
  this->DisplayInfoForExistingTraceTemplate< GoDBTWContainerForTrackLineage >(
    this->m_TWContainer, iDatabaseConnector, iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*unsigned int QGoDBTrackManager::CreateNewTrackWithNoMesh(
  vtkMySQLDatabase *iDatabaseConnector)
{
  GoDBTrackRow NewTrack;
  unsigned int NewTrackID =
    this->m_CollectionOfTraces->CreateCollectionWithNoTracesNoPoints< GoDBTrackRow >(
      iDatabaseConnector, *this->m_SelectedColorData, NewTrack);

  this->m_TrackContainerInfoForVisu->ResetCurrentElement();
  this->m_TrackContainerInfoForVisu->UpdateCurrentElementFromDB(
    NewTrackID, this->GetVectorFromQColor(this->m_SelectedColorData->second), true);
  this->m_TrackContainerInfoForVisu->InsertCurrentElement();
  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
  NameWithColorData NewTrackData(ConvertToString< unsigned int >(NewTrackID),
                                 this->m_SelectedColorData->second);
  emit AddNewTraceIDInTS(NewTrackData);
  return NewTrackID;
}*/

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBLineageManager::UpdateTheTracesColor(
  vtkMySQLDatabase *iDatabaseConnector)
{
  std::list< unsigned int > List = std::list<unsigned int>();
  return List;
 // return this->UpdateTheTracesColorTemplate< GoDBTrackRow,
 //                                            TrackContainer >(iDatabaseConnector, this->m_TrackContainerInfoForVisu);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::UpdateTWAndContainerForImportedTraces(
  std::vector< int > iVectorImportedTraces, vtkMySQLDatabase *iDatabaseConnector)
{
  //this->UpdateTWAndContainerWithImportedTracesTemplate<
  //  GoDBTWContainerForLineage >(this->m_TWContainer,
                             // iVectorImportedTraces, iDatabaseConnector);
  
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DeleteCheckedTraces(vtkMySQLDatabase *iDatabaseConnector)
{
  //this->DeleteTracesTemplate< TrackContainer >(iDatabaseConnector,
   //                                            this->m_TrackContainerInfoForVisu);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DeleteListTraces(vtkMySQLDatabase *iDatabaseConnector,
                                         std::list< unsigned int > iListTraces)
{
  //this->DeleteTracesTemplate< TrackContainer >(iDatabaseConnector,
  //                                             this->m_TrackContainerInfoForVisu, iListTraces, false);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBLineageManager::GetListHighlightedIDs()
{
  std::list< unsigned int > List = std::list<unsigned int>();
  return List;
  //return this->m_TrackContainerInfoForVisu->GetHighlightedElementsTraceID();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::UpdateHighlightedElementsInVisuContainer(
  int iTraceID)
{
  //this->m_TrackContainerInfoForVisu->
  //UpdateElementHighlightingWithGivenTraceID(iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::UpdateVisibleElementsInVisuContainer(int iTraceID)
{
  //this->m_TrackContainerInfoForVisu->
  //UpdateElementVisibilityWithGivenTraceID(iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::GetTracesInfoFromDBAndModifyContainerForVisu(
  vtkMySQLDatabase *iDatabaseConnector,
  std::list< unsigned int > iListTraceIDs)
{
  //this->GetTracesInfoFromDBAndModifyContainerForVisuTemplate< TrackContainer >(
  //  this->m_TrackContainerInfoForVisu, iDatabaseConnector, iListTraceIDs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoDBTrackManager::SaveTrackCurrentElement(
  vtkMySQLDatabase *iDatabaseConnector)
{
  GoDBTrackRow TrackToSave(this->m_ImgSessionID);
  unsigned int TrackID = this->m_TrackContainerInfoForVisu->m_CurrentElement.TraceID;

  if ( TrackID != 0 )
    {
    TrackToSave.SetValuesForSpecificID(TrackID, iDatabaseConnector);
    }

  //save the track into the database
  TrackToSave.SetThePointsFromPolydata(
    this->m_TrackContainerInfoForVisu->m_CurrentElement.Nodes);
  TrackID = TrackToSave.SaveInDB(iDatabaseConnector);

  //save the track into the container:
  this->m_TrackContainerInfoForVisu->InsertCurrentElement();

  //calculate the values to be put in the table widget:
  GoFigureTrackAttributes trackAttributes(
    this->m_TrackContainerInfoForVisu->m_CurrentElement.ComputeAttributes() );
  this->m_TWContainer->SetTrackAttributes(&trackAttributes);

  //update the table widget:
  if ( TrackID == 0 )
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
  std::map< unsigned int, double * > iMeshesInfo, vtkMySQLDatabase *iDatabaseConnector)
{
  this->m_TrackContainerInfoForVisu->UpdateCurrentElementMap(iMeshesInfo);
  this->SaveTrackCurrentElement(iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdateTrackPolydataForVisu(vtkMySQLDatabase *iDatabaseConnector,
                                                   unsigned int iTrackID)
{
  std::list< double * > ListCenters =
    this->m_CollectionOfTraces->GetCoordinateCenterBoundingBox(
      iDatabaseConnector, iTrackID);
  this->m_TrackContainerInfoForVisu->UpdatePointsForATrack(iTrackID, ListCenters);
  this->SaveTrackCurrentElement(iDatabaseConnector);
}
*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoDBTrackManager::UpdateBoundingBoxes(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTracesIDs)
{
  QGoDBTraceManager::UpdateBoundingBoxes(iDatabaseConnector, iListTracesIDs, false);
  std::list< unsigned int >::iterator iter = iListTracesIDs.begin();
  while ( iter != iListTracesIDs.end() )
    {
    this->UpdateTrackPolydataForVisu(iDatabaseConnector, *iter);
    iter++;
    }
}*/

//-------------------------------------------------------------------------
void QGoDBLineageManager::SetColorCoding(bool IsChecked)
{
 // this->SetColorCodingTemplate< TrackContainer >(
  //  this->m_TrackContainerInfoForVisu, IsChecked);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoDBTrackManager::AddActionsContextMenu(QMenu *iMenu)
{
  QGoDBTraceManager::AddActionsContextMenu(iMenu);
  //this->m_CheckedTracesMenu->addAction
  QMenu *SplitMergeMenu = new QMenu(tr("Split/Merge them"), iMenu);
  SplitMergeMenu->addAction( tr("Using the Widget"),
                             this, SLOT( SplitMergeTrackWithWidget() ) );
  SplitMergeMenu->addAction( tr("Split your track"), this, SLOT( TrackIDToEmit() ) );
  SplitMergeMenu->addAction( tr("Merge your 2 tracks"), this, SLOT( MergeTracks() ) );
  iMenu->addAction( SplitMergeMenu->menuAction() );
}*/

//-------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*void QGoDBTrackManager::TrackIDToEmit()
{
  std::list< unsigned int > HighlightedTrackIDs =
    this->m_TrackContainerInfoForVisu->GetHighlightedElementsTraceID();

  if ( HighlightedTrackIDs.size() != 1 )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please check one and only one Track to be split") );
    msgBox.exec();
    }
  else
    {
    emit NeedToGetDatabaseConnection();
    emit TrackToSplit( HighlightedTrackIDs.front(),
                       this->GetListTracesIDsFromThisCollectionOf(this->m_DatabaseConnector,
                                                                  HighlightedTrackIDs) );
    emit DBConnectionNotNeededAnymore();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::SplitMergeTrackWithWidget()
{
  std::list< unsigned int > HighlightedTrackIDs =
    this->m_TrackContainerInfoForVisu->GetHighlightedElementsTraceID();

  if ( HighlightedTrackIDs.size() == 0 )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please check at least one Track to be visualized in the widget") );
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
    int timeInterval = m_TrackContainerInfoForVisu->getTimeInterval();

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
    Values.at(4) = ConvertToString< double >
        (iTrackAttributes->avg_speed / timeInterval);
    ColumnNames.at(5) = "MaxSpeed";
    Values.at(5) = ConvertToString< double >
        (iTrackAttributes->max_speed / timeInterval);

    this->m_Table->AddValuesForID(ColumnNames, Values, iTrackID, "trackID");
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::MergeTracks()
{
  std::list< unsigned int > CheckedTrack =
    this->m_TrackContainerInfoForVisu->GetHighlightedElementsTraceID();
  if ( CheckedTrack.size() != 2 )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please check two and only two tracks to be merged") );
    msgBox.exec();
    }
  else
    {
    emit         NeedToGetDatabaseConnection();
    unsigned int TrackIDToKeep = 0;
    unsigned int TrackIDToDelete = 0;
    if ( this->CheckOverlappingTracks(CheckedTrack, TrackIDToKeep,
                                      TrackIDToDelete, this->m_DatabaseConnector) )
      {
      QMessageBox msgBox;
      msgBox.setText(
        tr("The two tracks are overlapping, it is not possible to merge them !!") );
      msgBox.exec();
      }
    else
      {
      std::list< unsigned int > TraceIDToDelete;
      TraceIDToDelete.push_back(TrackIDToDelete);
      std::list< unsigned int > MeshesBelongingToTrackToDelete =
        this->m_CollectionOfTraces->GetListTracesIDsFromThisCollectionOf(
          this->m_DatabaseConnector, TraceIDToDelete);
      this->DeleteListTraces(this->m_DatabaseConnector, TraceIDToDelete);
      emit MeshesToAddToTrack(MeshesBelongingToTrackToDelete, TrackIDToKeep);
      }
    emit DBConnectionNotNeededAnymore();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoDBTrackManager::CheckOverlappingTracks(
  std::list< unsigned int > iTrackIDs, unsigned int & ioTraceIDToKeep,
  unsigned int & ioTraceIDToDelete, vtkMySQLDatabase *iDatabaseConnector)
{
  unsigned int TraceID1 = 0, TraceID2 = 0;
  unsigned int TimePointMin1 = 0, TimePointMin2 = 0, TimePointMax1 = 0,
    TimePointMax2 = 0;
  bool oTracksOverlapping = true;

  std::list< unsigned int >::iterator iter = iTrackIDs.begin();
  if ( iter == iTrackIDs.end() )
    {
    std::cout << "Pb, there should have been 2 tracks instead of 0 in this method" << std::endl;
    return oTracksOverlapping;
    }
  TraceID1 = *iter;
  TimePointMin1 = this->m_CollectionOfTraces->GetBoundedBoxTimePoint(
      iDatabaseConnector, TraceID1, true);
  TimePointMax1 = this->m_CollectionOfTraces->GetBoundedBoxTimePoint(
      iDatabaseConnector, TraceID1, false);
  ++iter;
  if ( iter == iTrackIDs.end() )
    {
    std::cout << "Pb, there should have been 2 tracks instead of 1 in this method" << std::endl;
    return oTracksOverlapping;
    }

  TraceID2 = *iter;
  TimePointMin2 = this->m_CollectionOfTraces->GetBoundedBoxTimePoint(
      iDatabaseConnector, TraceID2, true);
  TimePointMax2 = this->m_CollectionOfTraces->GetBoundedBoxTimePoint(
      iDatabaseConnector, TraceID2, false);

  if ( TimePointMin2 > TimePointMax1 )
    {
    oTracksOverlapping = false;
    ioTraceIDToKeep = TraceID2;
    ioTraceIDToDelete = TraceID1;
    }

  if ( TimePointMin1 > TimePointMax2 )
    {
    oTracksOverlapping = false;
    ioTraceIDToKeep = TraceID1;
    ioTraceIDToDelete = TraceID2;
    }

  return oTracksOverlapping;
}*/