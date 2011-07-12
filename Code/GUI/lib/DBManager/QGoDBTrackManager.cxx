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
#include "GoDBTrackFamilyRow.h"
#include <iostream>
#include <sstream>

#include "TrackStructure.h"
#include "ConvertToStringHelper.h"


QGoDBTrackManager::QGoDBTrackManager(int iImgSessionID, QWidget *iparent) :
  QGoDBTraceManager(), m_TrackContainerInfoForVisu(NULL)
{
  this->SetInfo(iImgSessionID, iparent);
  this->m_TWContainer = new GoDBTWContainerForTrack(iImgSessionID);
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
  this->SetTracesInfoContainerForVisuTemplate< TrackContainer >(
    iContainerForVisu, &this->m_TrackContainerInfoForVisu);

  QObject::connect( this->m_TrackContainerInfoForVisu,
                    SIGNAL( NeedMeshesInfoForImportedTrack(unsigned int) ),
                    this,
                    SIGNAL( NeedMeshesInfoForImportedTrack(unsigned int) ) );
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
    this->m_TWContainer, iDatabaseConnector, Qt::Unchecked);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DisplayInfoAndLoadVisuContainerForAllTracks(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoAndLoadVisuContainerWithAllTraces< GoDBTWContainerForTrack >
    (this->m_TWContainer, iDatabaseConnector);
}

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
  // pointer to double has to be deleted after usage...
  double* color = this->GetVectorFromQColor(this->m_SelectedColorData->second);
  this->m_TrackContainerInfoForVisu->UpdateCurrentElementFromDB(
    NewTrackID, color, true);
  delete[] color;
  this->m_TrackContainerInfoForVisu->InsertCurrentElement();
  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
  NameWithColorData NewTrackData(ConvertToString< unsigned int >(NewTrackID),
                                 this->m_SelectedColorData->second);
  emit AddNewTraceIDInTS(NewTrackData);
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
  //if the tracks to be deleted belongs to some divisions, the divisions need to
  //be deleted first:
  std::list<unsigned int> ListDivisionsToDelete = 
    this->m_CollectionOfTraces->GetTrackFamilyID(iDatabaseConnector, 
    this->GetListHighlightedIDs() );
  std::list<unsigned int>::iterator iter = ListDivisionsToDelete.begin();
  std::list<unsigned int> TrackIDsWithNoLineage = std::list<unsigned int>();
  std::list<unsigned int> LineagesToDelete = std::list<unsigned int>();
  while( iter != ListDivisionsToDelete.end() )
    {
    if (*iter != 0)
      {
      emit NeedToGetDatabaseConnection(); 
      GoDBTrackFamilyRow Division(*iter, this->m_DatabaseConnector);
      this->DeleteOneDivision(Division, this->m_DatabaseConnector, TrackIDsWithNoLineage, LineagesToDelete);
      }
    ++iter;
    }

  if (!TrackIDsWithNoLineage.empty() ) //set the lineageID to 0 and update the bounding boxes of the previous lineages
    {
    emit CheckedTracksToAddToSelectedLineage(TrackIDsWithNoLineage, 0, LineagesToDelete);  
    }

  emit NeedToGetDatabaseConnection(); 
  this->DeleteTracesTemplate< TrackContainer >(this->m_DatabaseConnector,
                                               this->m_TrackContainerInfoForVisu);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DeleteListTraces(vtkMySQLDatabase *iDatabaseConnector,
                                         std::list< unsigned int > iListTraces)
{
  this->DeleteTracesTemplate< TrackContainer >(iDatabaseConnector,
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
  vtkMySQLDatabase *iDatabaseConnector,
  std::list< unsigned int > iListTraceIDs)
{
  this->GetTracesInfoFromDBAndModifyContainerForVisuTemplate< TrackContainer >(
    this->m_TrackContainerInfoForVisu, iDatabaseConnector, iListTraceIDs);
}

//-------------------------------------------------------------------------
// add pointer to structure instead
//-------------------------------------------------------------------------
void QGoDBTrackManager::SaveTrackCurrentElement(
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

  this->m_TrackContainerInfoForVisu->ResetCurrentElement();

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
void QGoDBTrackManager::SaveTrackStructure(
  vtkMySQLDatabase *iDatabaseConnector, TrackStructure* iStructure)
{
  GoDBTrackRow TrackToSave(this->m_ImgSessionID);
  unsigned int TrackID = iStructure->TraceID;

  if ( TrackID != 0 )
    {
    TrackToSave.SetValuesForSpecificID(TrackID, iDatabaseConnector);
    }

  //save the track into the database
  TrackToSave.SetThePointsFromPolydata(iStructure->Nodes);
  TrackID = TrackToSave.SaveInDB(iDatabaseConnector);

  //calculate the values to be put in the table widget:
  GoFigureTrackAttributes trackAttributes( iStructure->ComputeAttributes() );
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
  this->m_TrackContainerInfoForVisu->ImportTrackInCurrentElement(iMeshesInfo);
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
  TrackStructure* structure =
      this->m_TrackContainerInfoForVisu->UpdatePointsForATrack(iTrackID, ListCenters);
  SaveTrackStructure(iDatabaseConnector, structure);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdateBoundingBoxes(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTracesIDs)
{
  QGoDBTraceManager::UpdateBoundingBoxes(iDatabaseConnector, iListTracesIDs, false);
  std::list< unsigned int >::iterator iter = iListTracesIDs.begin();
  while ( iter != iListTracesIDs.end() )
    {
    this->UpdateTrackPolydataForVisu(iDatabaseConnector, *iter);
    ++iter;
    }
}

//-------------------------------------------------------------------------
void QGoDBTrackManager::SetColorCoding(bool IsChecked)
{
  this->SetColorCodingTemplate< TrackContainer >(
    this->m_TrackContainerInfoForVisu, IsChecked);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::AddActionsContextMenu(QMenu *iMenu)
{
  QGoDBTraceManager::AddActionsContextMenu(iMenu);

  QMenu *SplitMergeMenu = new QMenu(tr("Split/Merge them"), iMenu);
  SplitMergeMenu->addAction( tr("Using the Widget"),
                             this, SLOT( SplitMergeTrackWithWidget() ) );
  SplitMergeMenu->addAction( tr("Split your track"), this, SLOT( TrackIDToEmit() ) );
  SplitMergeMenu->addAction( tr("Merge your 2 tracks"), this, SLOT( MergeTracks() ) );
  iMenu->addAction( SplitMergeMenu->menuAction() );

  this->m_CheckedTracesMenu->addAction( tr("Create a new division from checked %1s")
                                        .arg( this->m_TraceName.c_str() ),
                                        this, SLOT( CreateCorrespondingTrackFamily() ) );
  this->m_CheckedTracesMenu->addAction( tr("Delete the division for this tracks") ,
                                        this, SLOT( DeleteTheDivisions() ) );
}

//-------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoDBTrackManager::TrackIDToEmit()
{
  std::list< unsigned int > HighlightedTrackIDs =
    this->m_TrackContainerInfoForVisu->GetHighlightedElementsTraceID();

  if ( HighlightedTrackIDs.size() != 1 )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please check one and only one Track to split") );
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
    std::cout <<"DisplayOnlyCalculatedValuesForExistingTrack: "<< iTrackAttributes << std::endl;
  if ( iTrackAttributes != 0 )
    {
    int timeInterval = m_TrackContainerInfoForVisu->getTimeInterval();

    std::vector< std::string > ColumnNames (7);
    std::vector< std::string > Values (7);

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
    ColumnNames.at(6) = "AvgVolume";
    Values.at(6) = ConvertToString< double >(iTrackAttributes->avg_volume);

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
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::CreateCorrespondingTrackFamily()
{
  int MotherID = 0;
  std::list<unsigned int> DaughtersIDs = std::list<unsigned int>();
  std::list<unsigned int> CheckedTracks = this->GetListHighlightedIDs();
  if (CheckedTracks.size() != 3)
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select 3 tracks to add your division") );
    msgBox.exec();
    return;
    }
  emit NeedToGetDatabaseConnection();
  if (this->IdentifyMotherDaughtersToCreateTrackFamily(this->m_DatabaseConnector, 
    this->GetListHighlightedIDs(), MotherID, DaughtersIDs) )
    {
    int TrackFamilyID =  this->CreateTrackFamily(this->m_DatabaseConnector, 
      MotherID, DaughtersIDs);
    if (TrackFamilyID != -1)
      {
      //check the lineageID of the mother:
      std::list<unsigned int> TrackID;
      TrackID.push_back(MotherID);
      std::list<unsigned int> LineageIDToCheck = 
        this->m_CollectionOfTraces->GetListCollectionIDs(this->m_DatabaseConnector,TrackID);
      //update the trackFamilyID for the daughters:
      std::list<unsigned int>::iterator iter = DaughtersIDs.begin();
      while(iter != DaughtersIDs.end() )
        {
        this->UpdateTrackFamilyIDForDaughter(this->m_DatabaseConnector, 
          *iter, TrackFamilyID);
        ++iter;
        }  
      //if the daughters belongs to other lineages, they need to be deleted and all the tracks belonging
      //to these lineages have to be added to the DaughtersIDs list to be added to the mother lineage
      std::list<unsigned int> PreviousLineagesToDelete = 
          this->GetTrackIDFromDaughtersFamilies(this->m_DatabaseConnector, DaughtersIDs);
      if (!LineageIDToCheck.empty())
        {
        //the mother track already belong to the lineage, need to add the daughters and her families:
        emit CheckedTracksToAddToSelectedLineage(DaughtersIDs, LineageIDToCheck.front(),
          PreviousLineagesToDelete);
        }
      else
        {
        //need to add to the new lineage the mother track, the daughters and her families:
        DaughtersIDs.push_back(MotherID);
        emit NewLineageToCreateFromTracks(DaughtersIDs, MotherID, PreviousLineagesToDelete);
        }
      }
    } 
  emit DBConnectionNotNeededAnymore();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoDBTrackManager::IdentifyMotherDaughtersToCreateTrackFamily(
  vtkMySQLDatabase* iDatabaseConnector,
  std::list<unsigned int> iListTracksID, int &ioMotherID,
  std::list<unsigned int> &ioDaughtersID)
{
  //get the trackid with the lowest timepoint and check that there is only one:
  ioMotherID = this->m_CollectionOfTraces->GetTraceIDWithLowestTimePoint(
    iDatabaseConnector,iListTracksID);
  if (ioMotherID == -1)
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Can not create the division as two of your selected tracks can be the mother") );
    msgBox.exec();
    return false;
    }
  //check that none of the 2 others tracks overlap the mother:
  std::list<unsigned int>::iterator iter = iListTracksID.begin();
  //to change: modify the method CheckOverlappingTracks:
  unsigned int ioTraceIDToKeep = 0;
  unsigned int ioTraceIDToDelete = 0; 
  while(iter != iListTracksID.end())
    {
    if (*iter != static_cast<unsigned int>(ioMotherID) )
      {
      std::list<unsigned int> TracksOverlappingToCheck;
      TracksOverlappingToCheck.push_back(ioMotherID);
      TracksOverlappingToCheck.push_back(*iter);
      if (this->CheckOverlappingTracks(TracksOverlappingToCheck, 
        ioTraceIDToKeep, ioTraceIDToDelete, iDatabaseConnector) )
        {
        QMessageBox msgBox;
        msgBox.setText(
          tr("Can not create the division as one daughter is overlapping the mother") );
        msgBox.exec();
        return false;
        }
      ioDaughtersID.push_back(*iter);
      }
    iter++;
    } 
  return true;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoDBTrackManager::CreateTrackFamily(vtkMySQLDatabase* iDatabaseConnector,
   unsigned int iMotherTrackID, std::list<unsigned int> iDaughtersID)
{
  int oTrackFamilyID = -1;
  //check that the mother is not already a mother in a trackfamily:
  GoDBTrackFamilyRow TrackFamily;
  TrackFamily.SetField<unsigned int>("TrackIDMother", iMotherTrackID);
  if (TrackFamily.DoesThisTrackFamilyAlreadyExists(iDatabaseConnector) != -1)
    {
    QMessageBox msgBox;
    msgBox.setText(
          tr("Can not create the division as the Mother track is already mother of other daugthers") );
    msgBox.exec();
    return oTrackFamilyID;
    }
  if(iDaughtersID.size() != 2)
    {
    std::cout<<"Pb, there is more than 2 daughters to create the division !!";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return oTrackFamilyID;
    }
  std::list<unsigned int>::iterator iter = iDaughtersID.begin();
  unsigned int TrackIDDaughterOne = *iter;
  ++iter;
  unsigned int TrackIDDaughterTwo = *iter;
  TrackFamily.SetField<unsigned int>("TrackIDDaughter1", TrackIDDaughterOne);
  
  TrackFamily.SetField<unsigned int>("TrackIDDaughter2", TrackIDDaughterTwo);
  this->m_TrackContainerInfoForVisu->AddDivision(iMotherTrackID, TrackIDDaughterOne, 
    TrackIDDaughterTwo);
  return TrackFamily.SaveInDB(iDatabaseConnector);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdateTrackFamilyIDForDaughter(
  vtkMySQLDatabase* iDatabaseConnector,
  unsigned int iDaughterID, unsigned int iTrackFamilyID)
{
  GoDBTrackRow Daughter;
  Daughter.SetValuesForSpecificID(iDaughterID, iDatabaseConnector);
  Daughter.SetField<unsigned int>("TrackFamilyID", iTrackFamilyID);
  Daughter.SaveInDB(iDatabaseConnector);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<unsigned int> QGoDBTrackManager::GetTrackIDFromDaughtersFamilies(
  vtkMySQLDatabase* iDatabaseConnector,
  std::list<unsigned int> &ioTrackIDsOfTheFamilies)
{
  //delete the daughtersLineageID then as they are useless:
  std::list<unsigned int> DaughtersLineageID = 
      this->m_CollectionOfTraces->GetListCollectionIDs(iDatabaseConnector, 
      ioTrackIDsOfTheFamilies);

  std::list<unsigned int> FirstDaugthersIDs = ioTrackIDsOfTheFamilies;
  std::list<unsigned int>::iterator iter = FirstDaugthersIDs.begin();
  ioTrackIDsOfTheFamilies.clear(); //in order not to have twice the daughtersIDs

  while(iter != FirstDaugthersIDs.end() )
    {
    std::list<unsigned int> ListDaughterID;
    ListDaughterID.push_back(*iter);
    std::list<unsigned int> LineageIDofDaughter = 
      this->m_CollectionOfTraces->GetListCollectionIDs(iDatabaseConnector, ListDaughterID);
    if (LineageIDofDaughter.empty()) //need to put back the ID of the daughter as it won't be returned by the query
      {
      ioTrackIDsOfTheFamilies.push_back(*iter);
      }
    else
      {
      std::list<unsigned int> CollectionID;
      CollectionID.push_back(LineageIDofDaughter.front());
      std::list<unsigned int> TrackIDsofTheFamily = 
        this->m_CollectionOfTraces->GetTraceIDsBelongingToCollectionID(
        iDatabaseConnector, CollectionID);
      std::list<unsigned int>::iterator iterTrackIDs = TrackIDsofTheFamily.begin();
      while (iterTrackIDs != TrackIDsofTheFamily.end() )
        {
        ioTrackIDsOfTheFamilies.push_back(*iterTrackIDs);
        ++iterTrackIDs;
        }
      }
    ++iter;
    }

  return DaughtersLineageID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::LoadInfoVisuContainerForTrackFamilies(
  vtkMySQLDatabase *iDatabaseConnector)
{
  std::list<unsigned int> ListTrackIDs = 
    this->m_CollectionOfTraces->GetTrackFamilyDataFromDB(iDatabaseConnector);
  this->m_TrackContainerInfoForVisu->SetListOfDivisions(ListTrackIDs);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DeleteTheDivisions()
{
  //check that the selected traces are all mother, if not message in the status bar:
  std::list<unsigned int> TrackIDNotMother = std::list<unsigned int>();
  std::list<unsigned int> CheckedTracks = 
    this->m_TrackContainerInfoForVisu->GetHighlightedElementsTraceID();
  std::list<unsigned int> TrackIDsWithNoLineage = std::list<unsigned int>();
  std::list<unsigned int> LineagesToDelete = std::list<unsigned int>();
  if (CheckedTracks.empty() )
    {
    QMessageBox msgBox;
    msgBox.setText(
          tr("Please select the MotherTracks you want the divisions to be deleted") );
    msgBox.exec();
    return;
    }
  std::list<unsigned int>::iterator iter = CheckedTracks.begin();
  while (iter != CheckedTracks.end())
    {
    emit NeedToGetDatabaseConnection();
    GoDBTrackFamilyRow Division;
    Division.SetField("TrackIDMother", *iter);
    int TrackFamilyToDelete = Division.DoesThisTrackFamilyAlreadyExists(this->m_DatabaseConnector);
    if (TrackFamilyToDelete == -1) //the selected track is not a mother
      {
      TrackIDNotMother.push_back(*iter);
      }
    else
      {
      Division.SetValuesForSpecificID(TrackFamilyToDelete, this->m_DatabaseConnector); //set the value of the existing TrackFamily
      this->DeleteOneDivision(Division, this->m_DatabaseConnector, TrackIDsWithNoLineage, LineagesToDelete );
      }
    ++iter;
    }

  this->PrintAMessageForTracksWithNoDivision(TrackIDNotMother);

  if (!TrackIDsWithNoLineage.empty() ) //set the lineageID to 0 and update the bounding boxes of the previous lineages
    {
    emit CheckedTracksToAddToSelectedLineage(TrackIDsWithNoLineage, 0, LineagesToDelete);  
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::DeleteOneDivision(GoDBTrackFamilyRow iDivision,
  vtkMySQLDatabase* iDatabaseConnector,
  std::list<unsigned int> &ioTrackIDsNoLineage,
  std::list<unsigned int> &ioMotherLineageToDelete)
{
  std::list<unsigned int> DaughtersIDs;
  DaughtersIDs.push_back( iDivision.GetMapValue<unsigned int>("TrackIDDaughter1") );
  DaughtersIDs.push_back( iDivision.GetMapValue<unsigned int>("TrackIDDaughter2") );
  int MotherID = iDivision.GetMapValue<int>("TrackIDMother");
  bool IsPartOfBiggerLineage = true;

  if (!this->IsTheTrackADaughter(MotherID, this->m_DatabaseConnector) ) // set the lineageID to 0
    {
    ioTrackIDsNoLineage.push_back(MotherID);
    IsPartOfBiggerLineage = false;
    }
  
  //delete the division from the database:
  iDivision.DeleteFromDB(this->m_DatabaseConnector);

  //update the different values for the daughters of the division:
  this->UpdateFormerDaughtersOfADeletedDivision(DaughtersIDs, ioTrackIDsNoLineage, 
    IsPartOfBiggerLineage);
 
  //delete the division from the visu:
  this->m_TrackContainerInfoForVisu->DeleteADivision( MotherID );

  if (!IsPartOfBiggerLineage) //the mother is not a daughter and the daughters are not mother, the lineage need to be deleted
    {
    emit NeedToGetDatabaseConnection();
    GoDBTrackRow Mother(MotherID, this->m_DatabaseConnector);
    emit DBConnectionNotNeededAnymore();
    ioMotherLineageToDelete.push_back(Mother.GetMapValue<unsigned int>("lineageID") );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdateFormerDaughtersOfADeletedDivision(
  std::list<unsigned int> iDaughtersID, 
  std::list<unsigned int> &ioTrackIDsNoLineage,
  bool &ioPartOfHigherLineage)
{
  std::list<unsigned int>::iterator iter = iDaughtersID.begin();
  while (iter != iDaughtersID.end() )
    {
    if (*iter != 0)
      {
      emit NeedToGetDatabaseConnection();
      this->UpdateTrackFamilyIDForDaughter(this->m_DatabaseConnector, *iter, 0);
      if (this->IsTheTrackAMother(*iter, this->m_DatabaseConnector) )
        {
        this->CreateALineageWithFormerDaughterOfADeletedDivision(*iter, 
          this->m_DatabaseConnector, ioPartOfHigherLineage);
        }
      else
        {
        ioTrackIDsNoLineage.push_back(*iter);
        }
      }
     ++iter;
    }
  emit DBConnectionNotNeededAnymore();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::PrintAMessageForTracksWithNoDivision(
  std::list<unsigned int> iTracksNoDivision)
{
  if (!iTracksNoDivision.empty() )
  {
  std::string Message = "Nothing has been done for these tracks ";
  std::list<unsigned int>::iterator iter = iTracksNoDivision.begin();
  while (iter != iTracksNoDivision.end() )
    {
    Message += ConvertToString<unsigned int>(*iter);
    Message += ", ";
    ++iter;
    }
  Message += "because they are not mothers of any divisions";
  emit PrintMessage(Message.c_str());
  }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBTrackManager::IsTheTrackAMother(unsigned int iDaughterID, 
  vtkMySQLDatabase* iDatabaseConnector)
{
  unsigned int TrackFamilyID;
  GoDBTrackFamilyRow Family;
  Family.SetField<unsigned int>("TrackIDMother", iDaughterID);
  TrackFamilyID = Family.DoesThisTrackFamilyAlreadyExists(iDatabaseConnector);
  if (TrackFamilyID != -1)
    {
    return TrackFamilyID;
    }
  return 0;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBTrackManager::IsTheTrackADaughter(unsigned int iTrackID, 
  vtkMySQLDatabase* iDatabaseConnector)
{
  GoDBTrackRow Track(iTrackID, iDatabaseConnector);
  return ss_atoi<unsigned int> (Track.GetMapValue("TrackFamilyID") );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::CreateALineageWithFormerDaughterOfADeletedDivision(
  unsigned int iDaughterID, vtkMySQLDatabase* iDatabaseConnector, 
  bool &ioPartOfHigherLineage)
{
    //GoDBTrackRow Daughter(iDaughterID, iDatabaseConnector);
  std::list<unsigned int> PreviousLineageToDelete = std::list<unsigned int>();
  if (!ioPartOfHigherLineage) //the lineage should not be deleted if higher tracks belong to it
    {
    GoDBTrackRow Daughter(iDaughterID, iDatabaseConnector);
    PreviousLineageToDelete.push_back( Daughter.GetMapValue<unsigned int>("lineageID") );
      //get the previous lineage ID of the daughter
    ioPartOfHigherLineage = true; // the second daughter will have a lineage set to 0 anyway
    }

  //get all the tracks daugthers of the DaughterID:
  std::list<unsigned int> TracksIDs = this->m_TrackContainerInfoForVisu->GetSubLineage(iDaughterID);
  if (TracksIDs.size() > 1)
    {
    emit NewLineageToCreateFromTracks(TracksIDs, iDaughterID, PreviousLineageToDelete); //need to create a new lineage with 
      //the family of the daughter
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoDBTrackManager::CheckMeshCanBeAddedToTrack(
  vtkMySQLDatabase* iDatabaseConnector, unsigned int iTrackID, 
  unsigned int iMeshTimePoint,
  std::list<unsigned int> &ioMotherTrackDivisionToUpdate)
{
  std::string Message = "";
  unsigned int TrackMinTimePoint = 
    this->m_CollectionOfTraces->GetBoundedBoxTimePoint(iDatabaseConnector, iTrackID, true);
  unsigned int TrackMaxTimePoint = 
    this->m_CollectionOfTraces->GetBoundedBoxTimePoint(iDatabaseConnector, iTrackID, false);
  //if the mesh inside the bounding box of the track, no pb:
  if (iMeshTimePoint > TrackMinTimePoint && iMeshTimePoint < TrackMaxTimePoint)
    {
    return Message;
    }

  unsigned int DivisionIDTrackIsAMother = this->IsTheTrackAMother(iTrackID, iDatabaseConnector);
  unsigned int DivisionIDTrackIsADaughter = this->IsTheTrackADaughter(iTrackID, iDatabaseConnector);
  //if the track doesn't belong to any division, no pb:
  if (!DivisionIDTrackIsAMother && !DivisionIDTrackIsADaughter)
    {
    return Message;
    }
  
  if (DivisionIDTrackIsAMother)
    {
    if (iMeshTimePoint >= TrackMinTimePoint)
      {     
      //check that the meshtimepoint < timepoint min daughter if not, message
      unsigned int DivisionToUpdate = this->CheckBoundingBoxDivisionAsAMother(
        iDatabaseConnector, iMeshTimePoint, DivisionIDTrackIsAMother );
      if (DivisionToUpdate != 0)
        {
        ioMotherTrackDivisionToUpdate.push_back(DivisionToUpdate);
        }
      else
        {
        Message = "The mesh will not belong to any track as there is a problem with the division";
        return Message;
        }
      }
    }

  if (DivisionIDTrackIsADaughter)
    {
    if (iMeshTimePoint > TrackMaxTimePoint)
      {
      return Message;
      }
      //check that the meshtimepoint > timepoint max mother if not, message
    unsigned int DivisionToUpdate = 
      this->CheckBoundingBoxDivisionAsADaughter(iDatabaseConnector, iMeshTimePoint,
      DivisionIDTrackIsADaughter);
    if (DivisionToUpdate != 0)
      {
      ioMotherTrackDivisionToUpdate.push_back(DivisionToUpdate);
      }
    else
      {
      Message = "The mesh will not belong to any track as there is a problem with the division";
      return Message;
      }
    }
  return Message;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBTrackManager::CheckBoundingBoxDivisionAsAMother(
  vtkMySQLDatabase* iDatabaseConnector, unsigned int iTimePoint, unsigned int iTrackFamilyID )
{
  if(iTrackFamilyID != 0)
    {
    GoDBTrackFamilyRow Division(iTrackFamilyID, iDatabaseConnector);
    if ( (iTimePoint < this->m_CollectionOfTraces->GetBoundedBoxTimePoint(
         iDatabaseConnector, Division.GetMapValue<unsigned int>("TrackIDDaughter1") )
         && (iTimePoint < this->m_CollectionOfTraces->GetBoundedBoxTimePoint(
         iDatabaseConnector, Division.GetMapValue<unsigned int>("TrackIDDaughter2") ) ) ) )
      {
      return Division.GetMapValue<unsigned int>("TrackIDMother");
      }
    return 0;
    }
  return 0;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBTrackManager::CheckBoundingBoxDivisionAsADaughter(
  vtkMySQLDatabase* iDatabaseConnector, unsigned int iTimePoint, unsigned int iTrackFamilyID )
{
  if (iTrackFamilyID != 0)
      {
      GoDBTrackFamilyRow Division(iTrackFamilyID, iDatabaseConnector);
      if ( iTimePoint > this->m_CollectionOfTraces->GetBoundedBoxTimePoint(
        iDatabaseConnector, Division.GetMapValue<unsigned int>("TrackIDMother") ) )
        {
        return Division.GetMapValue<unsigned int>("TrackIDMother");
        }
      return 0;
      }   
    return 0;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<unsigned int> QGoDBTrackManager::GetDivisionIDsTheTrackBelongsTo(
  vtkMySQLDatabase* iDatabaseConnector, unsigned int iTrackID )
{
  std::list<unsigned int> Tracks;
    Tracks.push_back(iTrackID);
    std::list<unsigned int> oDivisionIDs = this->m_CollectionOfTraces->GetTrackFamilyID(
      iDatabaseConnector, Tracks );
  return oDivisionIDs;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdateDivisions(std::list<unsigned int> iListMotherTrackIDs)
{
  std::list<unsigned int>::iterator iter = iListMotherTrackIDs.begin();
  while (iter != iListMotherTrackIDs.end() )
    {
    this->m_TrackContainerInfoForVisu->CreateDivisionPolydata(*iter);
    ++iter;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::
AddVolume(const unsigned int& iTrackID, const double& iVolume)
{
  this->m_TrackContainerInfoForVisu->AddVolume(iTrackID, iVolume);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoDBTrackManager::
RemoveVolumes(std::list< std::pair<unsigned int, double> > iVolumes)
{
  std::list< std::pair<unsigned int, double> >::iterator it =
          iVolumes.begin();
  while(it != iVolumes.end())
    {
    std::cout << "Remove: " << std::endl;
    std::cout << "track: " << (*it).first << std::endl;
    std::cout << "volume: " << (-1)*((*it).second) << std::endl;
    this->m_TrackContainerInfoForVisu->AddVolume((*it).first, (-1)*((*it).second));
    ++it;
    }
}
//-------------------------------------------------------------------------
