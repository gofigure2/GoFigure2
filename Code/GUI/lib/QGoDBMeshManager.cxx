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

#include "QGoDBMeshManager.h"
#include "GoDBMeshRow.h"
#include <iostream>
#include <sstream>

QGoDBMeshManager::QGoDBMeshManager(int iImgSessionID, QWidget *iparent):
QGoDBTraceManager(),m_MeshContainerInfoForVisu(NULL)
{
  this->SetInfo(iImgSessionID, iparent);
  this->m_TWContainer = new GoDBTWContainerForMesh(iImgSessionID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDBMeshManager::~QGoDBMeshManager()
{
  if ( this->m_TWContainer )
    {
    delete this->m_TWContainer;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::SetMeshesInfoContainerForVisu(
  ContourMeshContainer *iContainerForVisu)
{
  this->SetTracesInfoContainerForVisuTemplate<ContourMeshContainer>(
    iContainerForVisu,&this->m_MeshContainerInfoForVisu);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

void QGoDBMeshManager::SetCollectionsTraceNames()
{
  this->m_TraceName = "mesh";
  this->m_CollectionName = "track";
  this->m_CollectionOf = "contour";
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::DisplayInfoAndLoadVisuContainerForAllMeshes(
  vtkMySQLDatabase *iDatabaseConnector,unsigned int iTimePoint)
{
  this->DisplayInfoAndLoadVisuContainerWithAllTraces< GoDBTWContainerForMesh >
    (this->m_TWContainer, iDatabaseConnector);
  this->UpdateTracesVisibilityForGivenTimePoint(iTimePoint);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::DisplayInfoForAllTraces(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForAllTracesTemplate< GoDBTWContainerForMesh >(
    this->m_TWContainer, iDatabaseConnector,Qt::Unchecked);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::UpdateTracesVisibilityForGivenTimePoint(unsigned int iTimePoint)
{
  std::list<unsigned int> ListMeshes = 
    this->m_MeshContainerInfoForVisu->GetElementsTraceIDForGivenTimePoint(
    iTimePoint);
  this->m_Table->SetVisibleStateForListTraceIDs(
    ListMeshes,Qt::Checked,this->m_TraceName);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::DisplayInfoForLastCreatedTrace(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoForLastCreatedTraceTemplate< GoDBTWContainerForMesh >(
    this->m_TWContainer, iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::DisplayOnlyVolumeAreaForExistingMesh(
  GoFigureMeshAttributes *iMeshAttributes, unsigned iMeshID)
{
  if ( iMeshAttributes != 0 )
    {
    std::vector< std::string > ColumnNames (2);
    std::vector< std::string > Values (2);
    ColumnNames.at(0) = "SurfaceArea";
    Values.at(0) = ConvertToString< double >(iMeshAttributes->m_Area);
    ColumnNames.at(1) = "Volume";
    Values.at(1) = ConvertToString< double >(iMeshAttributes->m_Volume);
    this->m_Table->AddValuesForID(ColumnNames, Values, iMeshID, "meshID");
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::DisplayInfoForLastCreatedMesh(
  vtkMySQLDatabase *iDatabaseConnector, GoFigureMeshAttributes *iMeshAttributes)
{
  this->m_TWContainer->SetMeshAttributes(iMeshAttributes);
  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::DisplayInfoForExistingTrace(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID)
{
  this->DisplayInfoForExistingTraceTemplate< GoDBTWContainerForMesh >(
    this->m_TWContainer, iDatabaseConnector, iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::DisplayInfoForExistingTraceForMesh(
  vtkMySQLDatabase *iDatabaseConnector, int iTraceID,
  GoFigureMeshAttributes *iMeshAttributes)
{
  this->m_TWContainer->SetMeshAttributes(iMeshAttributes);
  this->DisplayInfoForExistingTrace(iDatabaseConnector, iTraceID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::AddActionsContextMenu(QMenu *iMenu)
{
  QGoDBTraceManager::AddActionsContextMenu(iMenu);
  this->AddSpecificActionsForContourMesh(iMenu);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBMeshManager::SaveNewMeshFromVisu(
  unsigned int iXCoordMin, unsigned int iYCoordMin, unsigned int iZCoordMin,
  unsigned int iTCoord, unsigned int iXCoordMax, unsigned int iYCoordMax,
  unsigned int iZCoordMax, vtkPolyData *iTraceNodes,
  vtkMySQLDatabase *iDatabaseConnector, NameWithColorData iColor,
  unsigned int iTrackID, GoFigureMeshAttributes *iMeshAttributes,
  std::string iCellType, std::string iSubCellType)
{
  GoDBMeshRow NewMesh(this->m_ImgSessionID);
  NewMesh.SetCellType(iDatabaseConnector, iCellType);
  NewMesh.SetSubCellType(iDatabaseConnector, iSubCellType);
  this->SetMeshBoundingBoxAndPoints(iXCoordMin, iYCoordMin, iZCoordMin, iTCoord,
                                    iXCoordMax, iYCoordMax, iZCoordMax, iTraceNodes, iDatabaseConnector, NewMesh,
                                    iMeshAttributes);
  //save the intensities for each channel !!!
  unsigned int NewMeshID = this->m_CollectionOfTraces->CreateNewTraceInDB< GoDBMeshRow >(
    NewMesh, iDatabaseConnector, iColor, iTrackID);
  double *rgba = this->GetVectorFromQColor(iColor.second);
  this->m_MeshContainerInfoForVisu->UpdateCurrentElementFromDB(
    NewMeshID, rgba);
  this->DisplayInfoForLastCreatedMesh(iDatabaseConnector, iMeshAttributes);
  return NewMeshID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::SaveGeneratedMeshFromVisu(unsigned int iXCoordMin,
                                                 unsigned int iYCoordMin,
                                                 unsigned int iZCoordMin,
                                                 unsigned int iTCoord,
                                                 unsigned int iXCoordMax,
                                                 unsigned int iYCoordMax,
                                                 unsigned int iZCoordMax,
                                                 vtkPolyData *iTraceNodes,
                                                 vtkMySQLDatabase *iDatabaseConnector,
                                                 GoFigureMeshAttributes *iMeshAttributes)
{
  unsigned int TraceID =
    this->m_MeshContainerInfoForVisu->m_CurrentElement.TraceID;
  GoDBMeshRow GeneratedMesh;

  GeneratedMesh.SetValuesForSpecificID(TraceID, iDatabaseConnector);

  this->SetMeshBoundingBoxAndPoints(iXCoordMin,
                                    iYCoordMin,
                                    iZCoordMin,
                                    iTCoord,
                                    iXCoordMax,
                                    iYCoordMax,
                                    iZCoordMax,
                                    iTraceNodes,
                                    iDatabaseConnector,
                                    GeneratedMesh,
                                    iMeshAttributes);
  //save the intensity for each channel !!!
  GeneratedMesh.SaveInDB(iDatabaseConnector);
  this->DisplayInfoForExistingTraceForMesh(iDatabaseConnector, TraceID, iMeshAttributes);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBMeshManager::CreateNewMeshWithNoContourNoPoints(
  vtkMySQLDatabase *iDatabaseConnector, NameWithColorData iColor, unsigned int iTimePoint,
  std::string iCellType, std::string iSubCellType, unsigned int iTrackID)
{
  GoDBMeshRow NewMesh;

  NewMesh.SetCellType(iDatabaseConnector, iCellType);
  NewMesh.SetSubCellType(iDatabaseConnector, iSubCellType);
  if ( iTrackID != 0 )
    {
    NewMesh.SetCollectionID(iTrackID);
    }
  unsigned int NewMeshID =
    this->m_CollectionOfTraces->CreateCollectionWithNoTracesNoPoints< GoDBMeshRow >(
      iDatabaseConnector, iColor, NewMesh, iTimePoint);

  double *color = this->GetVectorFromQColor(iColor.second);
  this->m_MeshContainerInfoForVisu->UpdateCurrentElementFromDB(
    NewMeshID, color);
  delete[] color;

  this->m_MeshContainerInfoForVisu->InsertCurrentElement();
  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);
  return NewMeshID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBMeshManager::UpdateTheTracesColor(
  vtkMySQLDatabase *iDatabaseConnector, NameWithColorData iNewColor)
{
  return this->UpdateTheTracesColorTemplate< GoDBMeshRow,ContourMeshContainer >(
    iDatabaseConnector,this->m_MeshContainerInfoForVisu,iNewColor);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::UpdateBoundingBoxes(vtkMySQLDatabase *iDatabaseConnector,
                                           std::list< unsigned int > iListTracesIDs)
{
  std::list< unsigned int > ListMeshesWithNoPoints =
    this->m_CollectionOfTraces->GetListTracesIDWithNoPoints(
      iListTracesIDs, iDatabaseConnector);
  if ( !ListMeshesWithNoPoints.empty() )
    {
    QGoDBTraceManager::UpdateBoundingBoxes(iDatabaseConnector, ListMeshesWithNoPoints);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::SetMeshBoundingBoxAndPoints(unsigned int iXCoordMin,
                                                   unsigned int iYCoordMin,
                                                   unsigned int iZCoordMin,
                                                   unsigned int iTCoord,
                                                   unsigned int iXCoordMax,
                                                   unsigned int iYCoordMax,
                                                   unsigned int iZCoordMax,
                                                   vtkPolyData *iTraceNodes,
                                                   vtkMySQLDatabase *iDatabaseConnector,
                                                   GoDBMeshRow & iMesh,
                                                   GoFigureMeshAttributes *iMeshAttributes)
{
  GoDBCoordinateRow coord_min = this->GetCoordinateFromInt(iXCoordMin,
                                                           iYCoordMin, iZCoordMin, iTCoord);
  GoDBCoordinateRow coord_max = this->GetCoordinateFromInt(iXCoordMax,
                                                           iYCoordMax, iZCoordMax, iTCoord);

  iMesh.SetTheDataFromTheVisu(iDatabaseConnector, iTraceNodes,
                              coord_min, coord_max, iMeshAttributes);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::UpdateTWAndContainerForImportedTraces(
  std::vector< int > iVectorImportedTraces, vtkMySQLDatabase *iDatabaseConnector)
{
  this->UpdateTWAndContainerWithImportedTracesTemplate<
    GoDBTWContainerForMesh>(this->m_TWContainer,
   iVectorImportedTraces, iDatabaseConnector);
  //update the visualization and the data from visu in the container for visu:
  this->m_MeshContainerInfoForVisu->
    UpdateVisualizationForGivenIDs< std::vector< int > >(
      iVectorImportedTraces, false);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::DeleteTraces(vtkMySQLDatabase *iDatabaseConnector)
{
  this->DeleteTracesTemplate<ContourMeshContainer>(iDatabaseConnector,
    this->m_MeshContainerInfoForVisu);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBMeshManager::GetListHighlightedIDs()
{
  return this->m_MeshContainerInfoForVisu->GetHighlightedElementsTraceID();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::UpdateHighlightedElementsInVisuContainer(
  int iTraceID)
{
  this->m_MeshContainerInfoForVisu->
    UpdateElementHighlightingWithGivenTraceID(iTraceID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::UpdateVisibleElementsInVisuContainer(int iTraceID)
{
  this->m_MeshContainerInfoForVisu->
    UpdateElementVisibilityWithGivenTraceID(iTraceID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::GetTracesInfoFromDBAndModifyContainerForVisu(
  vtkMySQLDatabase* iDatabaseConnector,std::vector<int> iVectIDs)
{
  std::list<ContourMeshStructure> list_of_traces;
  GetTracesInfoFromDBAndModifyContainer(
      list_of_traces,
      iDatabaseConnector, this->m_TraceName, this->m_CollectionName,
      this->m_ImgSessionID, -1, iVectIDs);

  std::list< ContourMeshStructure >::iterator it = list_of_traces.begin();

  while ( it != list_of_traces.end() )
    {
    this->m_MeshContainerInfoForVisu->Insert(*it);
    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::SetColorCoding(bool IsChecked)
{
  this->SetColorCodingTemplate<ContourMeshContainer>(
    this->m_MeshContainerInfoForVisu,IsChecked);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoDBMeshManager::BackFromColorCoding()
{
	this->SetBackFromColorCodingTemplate<ContourMeshContainer>(
    this->m_MeshContainerInfoForVisu);
}*/
