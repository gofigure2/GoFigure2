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
  QGoDBTraceManager()
{
  this->SetInfo(iImgSessionID, iparent);
  this->m_TWContainer = new GoDBTWContainerForTrackLineage(this->m_TraceName,
                                                           this->m_CollectionName, iImgSessionID);
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
  this->DisplayInfoAndLoadVisuContainerWithAllTraces< GoDBTWContainerForTrackLineage >(
    this->m_TWContainer, iDatabaseConnector);
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
  vtkMySQLDatabase *iDatabaseConnector, NameWithColorData iColor)
{
  GoDBTrackRow NewTrack;
  unsigned int NewTrackID =
    this->m_CollectionOfTraces->CreateCollectionWithNoTracesNoPoints< GoDBTrackRow >(
      iDatabaseConnector, iColor, NewTrack);

  this->m_TraceContainerInfoForVisu->UpdateCurrentElementFromDB(
    NewTrackID, this->GetVectorFromQColor(iColor.second) );
  this->m_TraceContainerInfoForVisu->InsertCurrentElement();
  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
  return NewTrackID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBTrackManager::UpdateTheTracesColor(
  vtkMySQLDatabase *iDatabaseConnector, NameWithColorData iNewColor)
{
  return this->UpdateTheTracesColorTemplate< GoDBTrackRow >(iDatabaseConnector,
                                                            iNewColor);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBTrackManager::UpdateTWAndContainerForImportedTraces(
  std::vector< int > iVectorImportedTraces, vtkMySQLDatabase *iDatabaseConnector)
{
  this->UpdateTWAndContainerWithImportedTracesTemplate<
    GoDBTWContainerForTrackLineage >(this->m_TWContainer,
                                     iVectorImportedTraces, iDatabaseConnector);
}
