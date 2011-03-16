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
  QGoDBTraceManager(), m_LineageContainerInfoForVisu(NULL)
{
  this->SetInfo(iImgSessionID, iparent);
  this->m_TWContainer = new GoDBTWContainerForLineage(iImgSessionID);
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
void QGoDBLineageManager::SetLineagesInfoContainerForVisu(
  LineageContainer *iContainerForVisu)
{
  this->SetTracesInfoContainerForVisuTemplate< LineageContainer >(
    iContainerForVisu, &this->m_LineageContainerInfoForVisu);

  /*QObject::connect( this->m_TrackContainerInfoForVisu,
                    SIGNAL( NeedMeshesInfoForImportedTrack(unsigned int) ),
                    this,
                    SIGNAL( NeedMeshesInfoForImportedTrack(unsigned int) ) );*/
}

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
  this->DisplayInfoForAllTracesTemplate< GoDBTWContainerForLineage >(
    this->m_TWContainer, iDatabaseConnector, Qt::Unchecked);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DisplayInfoAndLoadVisuContainerForAllLineages(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoAndLoadVisuContainerWithAllTraces< GoDBTWContainerForLineage >
    (this->m_TWContainer, iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DisplayInfoForLastCreatedTrace(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForLastCreatedTraceTemplate< GoDBTWContainerForLineage >(
    this->m_TWContainer, iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DisplayInfoForExistingTrace(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
{
  this->DisplayInfoForExistingTraceTemplate< GoDBTWContainerForLineage >(
    this->m_TWContainer, iDatabaseConnector, iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBLineageManager::CreateNewLineageWithTrackRoot(
      vtkMySQLDatabase *iDatabaseConnector, unsigned int iTrackRoot)
{
  GoDBLineageRow NewLineage;
  unsigned int NewLineageID =
    this->m_CollectionOfTraces->CreateCollectionWithNoTracesNoPoints< GoDBLineageRow >(
      iDatabaseConnector, *this->m_SelectedColorData, NewLineage);
  this->UpdateTrackRootSelectedLineage(iDatabaseConnector, NewLineageID, iTrackRoot);

  this->m_LineageContainerInfoForVisu->InsertNewLineage(NewLineageID, 
    this->GetVectorFromQColor(this->m_SelectedColorData->second), iTrackRoot,
    false);
  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
  
  return NewLineageID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBLineageManager::UpdateTheTracesColor(
  vtkMySQLDatabase *iDatabaseConnector)
{
  return this->UpdateTheTracesColorTemplate< GoDBLineageRow,
                                             LineageContainer >(iDatabaseConnector, this->m_LineageContainerInfoForVisu);
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
  this->DeleteTracesTemplate< LineageContainer >(iDatabaseConnector,
                                               this->m_LineageContainerInfoForVisu);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::DeleteListTraces(vtkMySQLDatabase *iDatabaseConnector,
                                         std::list< unsigned int > iListTraces)
{
  this->DeleteTracesTemplate< LineageContainer >(iDatabaseConnector,
                                               this->m_LineageContainerInfoForVisu, iListTraces, false);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBLineageManager::GetListHighlightedIDs()
{
  return this->m_LineageContainerInfoForVisu->GetHighlightedElementsTraceID();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::UpdateHighlightedElementsInVisuContainer(
  int iTraceID)
{
  this->m_LineageContainerInfoForVisu->
    UpdateElementHighlightingWithGivenTraceID(iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::UpdateVisibleElementsInVisuContainer(int iTraceID)
{
  this->m_LineageContainerInfoForVisu->
    UpdateElementVisibilityWithGivenTraceID(iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::GetTracesInfoFromDBAndModifyContainerForVisu(
  vtkMySQLDatabase *iDatabaseConnector,
  std::list< unsigned int > iListTraceIDs)
{
  this->GetTracesInfoFromDBAndModifyContainerForVisuTemplate< LineageContainer >(
    this->m_LineageContainerInfoForVisu, iDatabaseConnector, iListTraceIDs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::SetColorCoding(bool IsChecked)
{
 // this->SetColorCodingTemplate< TrackContainer >(
  //  this->m_TrackContainerInfoForVisu, IsChecked);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBLineageManager::UpdateTrackRootSelectedLineage(
  vtkMySQLDatabase* iDatabaseConnector,
  unsigned int iLineageID, unsigned int iTrackIDRoot) 
{
  GoDBLineageRow LastLineage;
  LastLineage.SetValuesForSpecificID(iLineageID, iDatabaseConnector);
  LastLineage.SetField("TrackIDRoot", iTrackIDRoot);
  LastLineage.SaveInDB(iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------