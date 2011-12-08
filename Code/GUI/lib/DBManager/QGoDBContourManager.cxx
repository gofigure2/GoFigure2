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

#include "QGoDBContourManager.h"
#include "GoDBContourRow.h"
#include <iostream>
#include <sstream>
#include <QMessageBox>
#include <QMenu>
#include "GoDBContourRow.h"

QGoDBContourManager::QGoDBContourManager(int iImgSessionID, QWidget *iparent) :
  QGoDBTraceManager(), m_ContourContainerInfoForVisu(NULL)
{
  this->SetInfo(iImgSessionID, iparent);
  this->m_TWContainer = new GoDBTWContainerForContourMesh(this->m_TraceName,
                                                          this->m_CollectionName, iImgSessionID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDBContourManager::~QGoDBContourManager()
{
  if ( this->m_TWContainer )
    {
    delete this->m_TWContainer;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::SetContoursInfoContainerForVisu(
  ContourContainer *iContainerForVisu)
{
  this->SetTracesInfoContainerForVisuTemplate< ContourContainer >(
    iContainerForVisu, &this->m_ContourContainerInfoForVisu);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::SetCollectionsTraceNames()
{
  this->m_TraceName = "contour";
  this->m_CollectionName = "mesh";
  this->m_CollectionOf = "None";
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::DisplayInfoAndLoadVisuContainerForAllContours(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoAndLoadVisuContainerWithAllTraces< GoDBTWContainerForContourMesh >
    (this->m_TWContainer, iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::DisplayInfoAndLoadVisuContainerForAllContoursForSpecificTPs(
  vtkMySQLDatabase *iDatabaseConnector, const std::list<unsigned int> & iListTPs)
{
  this->DisplayInfoAndLoadVisuContainerWithAllTracesForSpecificTPs< ContourMeshContainer >
    (iDatabaseConnector, this->m_ContourContainerInfoForVisu, iListTPs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
 void QGoDBContourManager::AddInfoInTWAndVisuContainerForContoursForSpecificTPs(
    vtkMySQLDatabase *iDatabaseConnector, const std::list<unsigned int> & iListTPs)
 {
   //this->AddInfoInTWAndContainerForVisuForSpecificTPs< ContourMeshContainer >
   //  (iDatabaseConnector, this->m_ContourContainerInfoForVisu, iListTPs);

   this->AddInfoForContoursInTWForSpecificTPs(iDatabaseConnector, iListTPs);
   std::list<unsigned int> ListIDs =
      this->m_CollectionOfTraces->GetTraceIDsBelongingToListTimePoints(
        iDatabaseConnector,  iListTPs);
    std::list<ContourMeshContainer::MultiIndexContainerElementType> list_of_traces =
      this->m_CollectionOfTraces->
      GetListStructureFromDB<ContourMeshContainer::MultiIndexContainerElementType>(
      iDatabaseConnector, this->m_ImgSessionID, ListIDs);
    /** \todo Nico: implement a method that get list_of_traces as argument and as this list
    of structure in the container for visu */
 }

 //-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::
AddInfoForContoursInTWForSpecificTPs(vtkMySQLDatabase *iDatabaseConnector,
    const std::list<unsigned int> & iListTPs)
{
  //int IndexShowColumn = this->m_TWContainer->GetIndexShowColumn();

  /*this->AddInfoForTracesInTWForSpecificTPsTemplate<GoDBTWContainerForContourMesh>(
    this->m_TWContainer,
    iDatabaseConnector, Qt::Unchecked, IndexShowColumn );*/
  //load the container with the traces infos for the TW for the TimePoints contained
    //in iListTPs:
  TWContainerType RowContainer =
      this->m_TWContainer->GetContainerLoadedWithAllFromDB(iDatabaseConnector, iListTPs);

  this->m_Table->InsertNewRows(RowContainer,
                               this->m_TWContainer->GetIndexForGroupColor(this->m_TraceName),
                               this->m_TWContainer->GetIndexForGroupColor(this->m_CollectionName),
                               this->m_TraceName,
                               this->m_CollectionName,
                               Qt::Unchecked);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

void QGoDBContourManager::DisplayInfoForAllTraces(
  vtkMySQLDatabase *iDatabaseConnector)

{
  int IndexShowColumn = this->m_TWContainer->GetIndexShowColumn();

  this->DisplayInfoForAllTracesTemplate< GoDBTWContainerForContourMesh >(
    this->m_TWContainer, iDatabaseConnector, Qt::Unchecked, IndexShowColumn);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::DisplayInfoForTracesForSpecificTPs(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::list<unsigned int> & iListTPs)
{
  int IndexShowColumn = this->m_TWContainer->GetIndexShowColumn();
  this->DisplayInfoForTracesForSpecificTPsTemplate< GoDBTWContainerForContourMesh >(
    this->m_TWContainer, iDatabaseConnector, Qt::Unchecked, iListTPs,
    IndexShowColumn);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::DisplayInfoForLastCreatedTrace(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForLastCreatedTraceTemplate< GoDBTWContainerForContourMesh >(
    this->m_TWContainer, iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::DisplayInfoForExistingTrace(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
{
  this->DisplayInfoForExistingTraceTemplate< GoDBTWContainerForContourMesh >(
    this->m_TWContainer, iDatabaseConnector, iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::
RemoveTracesFromTWAndContainerForVisuForSpecificTPs(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::list<unsigned int> & iListTPs)
{
  this->RemoveTracesFromTWAndContainerForVisuForSpecificTPsTemplate< ContourMeshContainer >
    (iDatabaseConnector, this->m_ContourContainerInfoForVisu, iListTPs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::AddActionsContextMenu(QMenu *iMenu)
{
  QGoDBTraceManager::AddActionsContextMenu(iMenu);
  this->AddSpecificActionsForContourMesh(iMenu);
  this->m_CheckedTracesMenu->addAction( tr("ReEdit the checked %1")
                                        .arg( this->m_TraceName.c_str() ),
                                        this, SLOT( ReEditTrace() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::AddActionForCreateNewCollectionFromCheckedTraces()
{
  this->m_CheckedTracesMenu->addAction(
    tr("Generate a new mesh from checked contours"),
    this, SLOT( CreateCorrespondingCollection() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::ChangeTraceColor()
{
  emit NeedToGetDatabaseConnection();

  this->UpdateTheTracesColor(this->m_DatabaseConnector);
  emit DBConnectionNotNeededAnymore();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBContourManager::UpdateTheTracesColor(
  vtkMySQLDatabase *iDatabaseConnector)
{
  return this->UpdateTheTracesColorTemplate< GoDBContourRow, ContourMeshContainer >(
           iDatabaseConnector, this->m_ContourContainerInfoForVisu);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBContourManager::SaveNewContourFromVisu(
  int iTCoord,
  unsigned int iXCoordMin, unsigned int iYCoordMin, unsigned int iZCoordMin,
  unsigned int iXCoordMax, unsigned int iYCoordMax,
  unsigned int iZCoordMax, vtkPolyData *iTraceNodes,
  vtkMySQLDatabase *iDatabaseConnector,
  unsigned int iMeshID)

{
  //if ( this->m_SelectedCollectionData->first != "Add a new mesh ..."
  //&& iMeshID != 0)
  if ( iMeshID != 0 )
    {
    iMeshID = ss_atoi< unsigned int >(this->m_SelectedCollectionData->first);
    }
  GoDBContourRow NewContour(this->m_ImgSessionID);

  int NewContourID = this->CreateNewTraceInDBFromVisu< GoDBContourRow >(
      iXCoordMin, iYCoordMin, iZCoordMin, iTCoord,//*this->m_CurrentTimePoint,
      iXCoordMax, iYCoordMax, iZCoordMax, iTraceNodes,
      *this->m_SelectedColorData,
      iDatabaseConnector, NewContour, iMeshID);
  // pointer to double has to be deleted after usage...
  double *rgba = this->GetVectorFromQColor(this->m_SelectedColorData->second);
  this->m_ContourContainerInfoForVisu->UpdateCurrentElementFromDB(
    NewContourID, rgba);
  delete[] rgba;

  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
  return NewContourID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBContourManager::SaveReeditedContourFromVisu(unsigned int iXCoordMin,
                                                              unsigned int iYCoordMin,
                                                              unsigned int iZCoordMin,
                                                              unsigned int iTCoord,
                                                              unsigned int iXCoordMax,
                                                              unsigned int iYCoordMax,
                                                              unsigned int iZCoordMax,
                                                              vtkPolyData *iContourNodes,
                                                              vtkMySQLDatabase *iDatabaseConnector)
{
  unsigned int TraceID =
    this->m_ContourContainerInfoForVisu->m_CurrentElement.TraceID;
  GoDBContourRow ReeditedContour;

  ReeditedContour.SetValuesForSpecificID(TraceID, iDatabaseConnector);
  this->SetTraceBoundingBoxAndPoints(iXCoordMin, iYCoordMin, iZCoordMin, iTCoord, iXCoordMax,
                                     iYCoordMax, iZCoordMax, iContourNodes, iDatabaseConnector,
                                     ReeditedContour);
  ReeditedContour.SaveInDB(iDatabaseConnector);
  this->DisplayInfoForExistingTrace(iDatabaseConnector, TraceID);
  return TraceID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::ReEditTrace()
{
  std::list< unsigned int > ListCheckedTraces =
    this->m_ContourContainerInfoForVisu->GetHighlightedElementsTraceID();
  if ( ListCheckedTraces.empty() )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select the %1 you want to reedit")
      .arg( this->m_TraceName.c_str() ) );
    msgBox.exec();
    }
  else
    {
    if ( ListCheckedTraces.size() != 1 )
      {
      QMessageBox msgBox;
      msgBox.setText(
        tr("Please select only one %1 to reedit")
        .arg( this->m_TraceName.c_str() ) );
      msgBox.exec();
      }
    else
      {
      emit TraceToReEdit( ListCheckedTraces.front() );
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::UpdateTWAndContainerForImportedTraces(
  const std::vector< int > & iVectorImportedTraces,
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->UpdateTWAndContainerWithImportedTracesTemplate<
    GoDBTWContainerForContourMesh >(
    this->m_TWContainer, iVectorImportedTraces, iDatabaseConnector);
  //update the visualization and the data from visu in the container for visu:
  this->m_ContourContainerInfoForVisu->
  UpdateVisualizationForGivenIDs< std::vector< int > >(
    iVectorImportedTraces);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::DeleteCheckedTraces(vtkMySQLDatabase *iDatabaseConnector)
{
  this->DeleteTracesTemplate< ContourMeshContainer >(iDatabaseConnector,
                                                     this->m_ContourContainerInfoForVisu);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBContourManager::GetListHighlightedIDs()
{
  return this->m_ContourContainerInfoForVisu->GetHighlightedElementsTraceID();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::UpdateHighlightedElementsInVisuContainer(
  int iTraceID)
{
  this->m_ContourContainerInfoForVisu->
  UpdateElementHighlightingWithGivenTraceID(iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::UpdateVisibleElementsInVisuContainer(int iTraceID)
{
  this->m_ContourContainerInfoForVisu->
  UpdateElementVisibilityWithGivenTraceID(iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::GetTracesInfoFromDBAndModifyContainerForVisu(
  vtkMySQLDatabase *iDatabaseConnector,
  std::list< unsigned int > iListTraceIDs)
{
  this->GetTracesInfoFromDBAndModifyContainerForVisuTemplate< ContourContainer >(
    this->m_ContourContainerInfoForVisu, iDatabaseConnector, iListTraceIDs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::SetColorCoding(bool IsChecked)
{
  this->SetColorCodingTemplate< ContourMeshContainer >(
    this->m_ContourContainerInfoForVisu, IsChecked);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::AddToSelectedCollection()
{
  if ( this->AreCheckedContoursFromCurrentTimepoint() )
    {
    QGoDBTraceManager::AddToSelectedCollection();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBContourManager::CreateCorrespondingCollection()
{
  if ( this->AreCheckedContoursFromCurrentTimepoint() )
    {
    QGoDBTraceManager::CreateCorrespondingCollection();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoDBContourManager::AreCheckedContoursFromCurrentTimepoint()
{
  std::list< unsigned int > ListCheckedContours =
    this->m_ContourContainerInfoForVisu->GetHighlightedElementsTraceID();
  if ( !ListCheckedContours.empty() )
    {
    emit                                NeedToGetDatabaseConnection();
    std::list< unsigned int >::iterator iter = ListCheckedContours.begin();

    while ( iter != ListCheckedContours.end() )
      {
      unsigned int TimepointContour =
        this->m_CollectionOfTraces->GetBoundedBoxTimePoint(this->m_DatabaseConnector, *iter);

      if ( TimepointContour !=
           static_cast< unsigned int >( *this->m_CurrentTimePoint ) )
        {
        emit PrintMessage(
          tr(
            "To see only the contours from the current timepoint in the table, right click on the table and select 'Show only in the table the contours for the current timepoint' ") );

        QMessageBox msgBox;
        msgBox.setText(
          tr("Please select only contours from the current timepoint: %1 !!")
          .arg(*this->m_CurrentTimePoint) );
        msgBox.exec();

        return false;
        }
      ++iter;
      }
    emit DBConnectionNotNeededAnymore();
    }
  return true;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoDBContourManager::
CleanTWAndContainerForGivenTimePoint(vtkMySQLDatabase *iDatabaseConnector,
                                     const std::list<unsigned int>& iTimePoints)
{
  this->RemoveTracesFromTWAndContainerForVisuForSpecificTPsTemplate<ContourContainer>(
          iDatabaseConnector, this->m_ContourContainerInfoForVisu, iTimePoints);
}
//-------------------------------------------------------------------------
