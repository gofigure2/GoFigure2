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

#include "QGoDBMeshManager.h"
#include "GoDBMeshRow.h"
#include <iostream>
#include <sstream>

QGoDBMeshManager::QGoDBMeshManager(int iImgSessionID, QWidget *iparent) :
  QGoDBTraceManager(), m_MeshContainerInfoForVisu(NULL), m_SelectedCellType(NULL),
  m_SelectedSubCellType(NULL)
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
  MeshContainer *iContainerForVisu)
{
  this->SetTracesInfoContainerForVisuTemplate< MeshContainer >(
    iContainerForVisu, &this->m_MeshContainerInfoForVisu);
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
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->DisplayInfoAndLoadVisuContainerWithAllTraces< 
    GoDBTWContainerForMesh >(this->m_TWContainer, iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::DisplayInfoAndLoadVisuContainerForAllMeshesForSpecificTPs(
  vtkMySQLDatabase *iDatabaseConnector, std::list<unsigned int> iListTPs)
{
  this->DisplayInfoAndLoadVisuContainerWithAllTracesForSpecificTPs< ContourMeshContainer >
    (iDatabaseConnector, this->m_MeshContainerInfoForVisu, iListTPs);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
 void QGoDBMeshManager::AddInfoInTWAndVisuContainerForMeshesForSpecificTPs(
    vtkMySQLDatabase *iDatabaseConnector, std::list<unsigned int> iListTPs)
 {
   //this->AddInfoInTWAndContainerForVisuForSpecificTPs< ContourMeshContainer >
   //  (iDatabaseConnector, this->m_ContourContainerInfoForVisu, iListTPs);

   this->AddInfoForMeshesInTWForSpecificTPs(iDatabaseConnector, iListTPs);
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
void QGoDBMeshManager::AddInfoForMeshesInTWForSpecificTPs(vtkMySQLDatabase *iDatabaseConnector, 
    std::list<unsigned int> iListTPs)
{
  int IndexShowColumn = this->m_TWContainer->GetIndexShowColumn();

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
                                 this->m_TraceName, this->m_CollectionName,Qt::Unchecked);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::DisplayInfoForAllTraces(
  vtkMySQLDatabase *iDatabaseConnector)
{
  int IndexShowColumn = this->m_TWContainer->GetIndexShowColumn();

  this->DisplayInfoForAllTracesTemplate< GoDBTWContainerForMesh >(
    this->m_TWContainer, iDatabaseConnector, Qt::Unchecked, IndexShowColumn);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::DisplayInfoForTracesForSpecificTPs(
  vtkMySQLDatabase *iDatabaseConnector, std::list<unsigned int> iListTPs)
{
  int IndexShowColumn = this->m_TWContainer->GetIndexShowColumn();
  this->DisplayInfoForTracesForSpecificTPsTemplate<GoDBTWContainerForMesh >(
    this->m_TWContainer, iDatabaseConnector, Qt::Unchecked, iListTPs,
    IndexShowColumn);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::RemoveTracesFromTWAndContainerForVisuForSpecificTPs(
                                                    vtkMySQLDatabase *iDatabaseConnector,
                                                    std::list<unsigned int> iListTPs)
{
  this->RemoveTracesFromTWAndContainerForVisuForSpecificTPsTemplate< ContourMeshContainer >
    (iDatabaseConnector, this->m_MeshContainerInfoForVisu, iListTPs);
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
  this->m_CheckedTracesMenu->addAction(
    tr("Add the selected CellType '%1' to the checked meshes")
    .arg( this->m_SelectedCellType->c_str() ), this, SLOT( UpdateCellType() ) );
  this->m_CheckedTracesMenu->addAction(
    tr("Add the selected SubCellType '%1' to the checked meshes")
    .arg( this->m_SelectedSubCellType->c_str() ), this, SLOT( UpdateSubCellType() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBMeshManager::SaveNewMeshFromVisu(
  unsigned int iXCoordMin, unsigned int iYCoordMin, unsigned int iZCoordMin,
  unsigned int iXCoordMax, unsigned int iYCoordMax,
  unsigned int iZCoordMax, int iTCoord,//int iTShift, 
  vtkPolyData *iTraceNodes,
  vtkMySQLDatabase *iDatabaseConnector,
  GoFigureMeshAttributes *iMeshAttributes,
  unsigned int iTrackID)
{
  GoDBMeshRow NewMesh(this->m_ImgSessionID);

  NewMesh.SetCellType(iDatabaseConnector, *this->m_SelectedCellType);
  NewMesh.SetSubCellType(iDatabaseConnector, *this->m_SelectedSubCellType);

  this->SetMeshBoundingBoxAndPoints(iXCoordMin, iYCoordMin, iZCoordMin,
                                    iXCoordMax, iYCoordMax, iZCoordMax, iTraceNodes, iDatabaseConnector, NewMesh,
                                    iMeshAttributes, iTCoord);
  //save the intensities for each channel !!!
  unsigned int NewMeshID = this->m_CollectionOfTraces->CreateNewTraceInDB< GoDBMeshRow >(
      NewMesh, iDatabaseConnector, *this->m_SelectedColorData, iTrackID );
  // pointer to double has to be deleted after usage...
  double *rgba = this->GetVectorFromQColor(this->m_SelectedColorData->second);
  this->m_MeshContainerInfoForVisu->UpdateCurrentElementFromDB(
    NewMeshID, rgba);
  this->m_MeshContainerInfoForVisu->UpdateCurrentElementCollection(iTrackID);
  delete[] rgba;
  this->DisplayInfoForLastCreatedMesh(iDatabaseConnector, iMeshAttributes);
  return NewMeshID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBMeshManager::SaveNewMeshFromVisu(
  unsigned int iXCoordMin, unsigned int iYCoordMin, unsigned int iZCoordMin,
  unsigned int iXCoordMax, unsigned int iYCoordMax,
  unsigned int iZCoordMax, int iTCoord, vtkPolyData *iTraceNodes,
  vtkMySQLDatabase *iDatabaseConnector,
  GoFigureMeshAttributes *iMeshAttributes)
{
  return SaveNewMeshFromVisu(iXCoordMin, iYCoordMin, iZCoordMin, iXCoordMax, iYCoordMax, 
    iZCoordMax, iTCoord, iTraceNodes, iDatabaseConnector, iMeshAttributes, 
    ss_atoi<unsigned int>(this->m_SelectedCollectionData->first) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBMeshManager::SaveNewMeshWithNoTrackFromVisu(
  unsigned int iXCoordMin, unsigned int iYCoordMin, unsigned int iZCoordMin,
  unsigned int iXCoordMax, unsigned int iYCoordMax,
  unsigned int iZCoordMax, int iTShift, vtkPolyData *iTraceNodes,
  vtkMySQLDatabase *iDatabaseConnector,
  GoFigureMeshAttributes *iMeshAttributes)
{
  return SaveNewMeshFromVisu(iXCoordMin, iYCoordMin, iZCoordMin, iXCoordMax, iYCoordMax, 
    iZCoordMax, iTShift, iTraceNodes, iDatabaseConnector, iMeshAttributes, 0 );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

void QGoDBMeshManager::SaveGeneratedMeshFromVisu(unsigned int iXCoordMin,
                                                 unsigned int iYCoordMin,
                                                 unsigned int iZCoordMin,
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
  vtkMySQLDatabase *iDatabaseConnector)
{
  GoDBMeshRow NewMesh;

  NewMesh.SetCellType(iDatabaseConnector, *this->m_SelectedCellType);
  NewMesh.SetSubCellType(iDatabaseConnector, *this->m_SelectedSubCellType);

  //unsigned int TrackID = 0;

  // if( this->m_SelectedCollectionData->first != "Add a new mesh ..."
  // && this->m_SelectedCollectionData->first != "Add a new track ...")
  //   {
  // TrackID = ss_atoi<unsigned int>(this->m_SelectedCollectionData->first);
  //   }
  // if ( TrackID != 0 )
  //   {
  //NewMesh.SetCollectionID(TrackID);
  //   }
  NewMesh.SetCollectionID( ss_atoi< unsigned int >(this->m_SelectedCollectionData->first) );

  unsigned int NewMeshID =
    this->m_CollectionOfTraces->CreateCollectionWithNoTracesNoPoints< GoDBMeshRow >(
      iDatabaseConnector, *this->m_SelectedColorData, NewMesh, *this->m_CurrentTimePoint);
  // pointer to double has to be deleted after usage...
  double *color = this->GetVectorFromQColor(this->m_SelectedColorData->second);
  this->m_MeshContainerInfoForVisu->ResetCurrentElement();
  this->m_MeshContainerInfoForVisu->UpdateCurrentElementFromDB(
    NewMeshID, color);
  delete[] color;
  this->m_MeshContainerInfoForVisu->InsertCurrentElement();
  this->DisplayInfoForLastCreatedTrace(iDatabaseConnector);

  NameWithColorData NewMeshData(ConvertToString< unsigned int >(NewMeshID),
                                this->m_SelectedColorData->second);
  emit AddNewTraceIDInTS(NewMeshData);
  return NewMeshID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoDBMeshManager::UpdateTheTracesColor(
  vtkMySQLDatabase *iDatabaseConnector)
{
  return this->UpdateTheTracesColorTemplate< GoDBMeshRow, ContourMeshContainer >(
           iDatabaseConnector, this->m_MeshContainerInfoForVisu);
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
                                                   unsigned int iXCoordMax,
                                                   unsigned int iYCoordMax,
                                                   unsigned int iZCoordMax,
                                                   vtkPolyData *iTraceNodes,
                                                   vtkMySQLDatabase *iDatabaseConnector,
                                                   GoDBMeshRow & iMesh,
                                                   GoFigureMeshAttributes *iMeshAttributes,
                                                   int iTCoord)
                                                   //int iShift)
{
  GoDBCoordinateRow coord_min = this->GetCoordinateFromInt(iXCoordMin,
                                                           iYCoordMin, iZCoordMin, iTCoord);//*this->m_CurrentTimePoint + iShift);
  GoDBCoordinateRow coord_max = this->GetCoordinateFromInt(iXCoordMax,
                                                           iYCoordMax, iZCoordMax, iTCoord);//*this->m_CurrentTimePoint + +iShift);

  iMesh.SetTheDataFromTheVisu(iDatabaseConnector, iTraceNodes,
                              coord_min, coord_max, iMeshAttributes);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::UpdateTWAndContainerForImportedTraces(
  std::vector< int > iVectorImportedTraces, vtkMySQLDatabase *iDatabaseConnector)
{
  this->UpdateTWAndContainerWithImportedTracesTemplate<
    GoDBTWContainerForMesh >(this->m_TWContainer,
                             iVectorImportedTraces, iDatabaseConnector);
  //update the visualization and the data from visu in the container for visu:
  this->m_MeshContainerInfoForVisu->
  UpdateVisualizationForGivenIDs< std::vector< int > >(
    iVectorImportedTraces);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::DeleteCheckedTraces(vtkMySQLDatabase *iDatabaseConnector)
{
  this->DeleteTracesTemplate< ContourMeshContainer >(iDatabaseConnector,
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
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTraceIDs)
{
  this->GetTracesInfoFromDBAndModifyContainerForVisuTemplate< MeshContainer >(
    this->m_MeshContainerInfoForVisu, iDatabaseConnector, iListTraceIDs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
MeshContainer * QGoDBMeshManager::GetMeshesInfoFromDBAndCreateContainerForVisu(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListCollectionIDs)
{
  std::list< unsigned int > ListMeshesInvolved =
    this->GetListTracesIDsBelongingToCollectionIDs(iDatabaseConnector, iListCollectionIDs);
  MeshContainer *                   oMeshContainer = new MeshContainer(this, NULL);
  std::list< ContourMeshStructure > ListMeshesInfo =
    this->m_CollectionOfTraces->GetListStructureFromDB< ContourMeshStructure >(
      iDatabaseConnector, this->m_ImgSessionID, ListMeshesInvolved);

  std::list< ContourMeshStructure >::iterator it = ListMeshesInfo.begin();

  while ( it != ListMeshesInfo.end() )
    {
    oMeshContainer->Insert(*it);
    ++it;
    }
  return oMeshContainer;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::map< unsigned int, double * > QGoDBMeshManager::GetMeshesInfoForImportedMesh(
  std::list< unsigned int > iMeshesIDs)
{
  return this->m_MeshContainerInfoForVisu->GetMeshesPoints(iMeshesIDs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::SetColorCoding(bool IsChecked)
{
  this->SetColorCodingTemplate< ContourMeshContainer >(
    this->m_MeshContainerInfoForVisu, IsChecked);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::SetSelectedCellType(std::string *iCellType)
{
  this->m_SelectedCellType = iCellType;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::SetSelectedSubCellType(std::string *iSubCellType)
{
  this->m_SelectedSubCellType = iSubCellType;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::UpdateCellType()
{
  emit NeedToGetDatabaseConnection();

  std::list< unsigned int > ListCheckedMeshes =
    this->m_MeshContainerInfoForVisu->GetHighlightedElementsTraceID();
  int CellTypeID = GoDBMeshRow::GetCellTypeID(this->m_DatabaseConnector,
                                              *this->m_SelectedCellType);
  this->m_CollectionOfTraces->UpdateValueForListTraces(
    this->m_DatabaseConnector, "CellTypeID", ConvertToString< int >(CellTypeID),
    ListCheckedMeshes);
  this->DisplayInfoForExistingTraces(this->m_DatabaseConnector, ListCheckedMeshes);
  emit DBConnectionNotNeededAnymore();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBMeshManager::UpdateSubCellType()
{
  emit NeedToGetDatabaseConnection();

  std::list< unsigned int > ListCheckedMeshes =
    this->m_MeshContainerInfoForVisu->GetHighlightedElementsTraceID();
  int SubCellTypeID = GoDBMeshRow::GetSubCellTypeID(this->m_DatabaseConnector,
                                                    *this->m_SelectedSubCellType);
  this->m_CollectionOfTraces->UpdateValueForListTraces(
    this->m_DatabaseConnector, "SubCellularID", ConvertToString< int >(SubCellTypeID),
    ListCheckedMeshes);
  this->DisplayInfoForExistingTraces(this->m_DatabaseConnector, ListCheckedMeshes);
  emit DBConnectionNotNeededAnymore();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoDBMeshManager::ReassignTrackIDForPreviousMeshWithSameTimePoint(vtkMySQLDatabase *iDatabaseConnector,
                                                                               unsigned int iTrackID,
                                                                               unsigned int iTimePoint)
{
  unsigned int oExistingMeshID = 0;

  //get meshID with same timepoint and same TrackID:
  std::list< unsigned int > ExistingMeshID =
    this->m_CollectionOfTraces->GetTraceIDsWithTimePointAndCollectionID(
      iDatabaseConnector, iTrackID, iTimePoint);
  if ( ExistingMeshID.empty() )
    {
    return oExistingMeshID;
    }
  if ( ExistingMeshID.size() > 1 )
    {
    std::cout << "there is more than 1 existing mesh for this track at this timepoint ";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return oExistingMeshID;
    }
  oExistingMeshID = ExistingMeshID.front();
  GoDBMeshRow ExistingMesh(oExistingMeshID, iDatabaseConnector);
  // modify table widget and DB
  ExistingMesh.SetCollectionID(0);
  ExistingMesh.SaveInDB(iDatabaseConnector);
  // modify visu container as well
  m_MeshContainerInfoForVisu->AssignToGivenCollection(0, ExistingMeshID);
  this->DisplayInfoForExistingTrace(iDatabaseConnector, oExistingMeshID); //update
                                                                          // the
                                                                          // TW
  return oExistingMeshID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoDBMeshManager::CheckExistingMeshesForTheTrack(
  unsigned int iTrackID, vtkMySQLDatabase *iDatabaseConnector, 
  int iTCoord)
{
  QString MessageToPrint("");

  if ( iTrackID != 0 ) //if the user has not created a track yet
    {
    unsigned int MeshIDKickedOut =
      this->ReassignTrackIDForPreviousMeshWithSameTimePoint(
        iDatabaseConnector, iTrackID, iTCoord);
    if ( MeshIDKickedOut != 0 )
      {
      MessageToPrint = QString::number(MeshIDKickedOut);
      }
    }
  return MessageToPrint;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoDBMeshManager::CheckExistingMeshesForTheTrack(
  unsigned int iTrackID, vtkMySQLDatabase *iDatabaseConnector,
  std::list< unsigned int > & ioListMeshIDs,
  std::list< unsigned int > & ioNullListMeshIDs)
{
  QString MessageQString("");

  if ( iTrackID != 0 )
    {
    std::string               MessageToPrint = "";
    std::list< unsigned int > ListTimePoints =
      this->m_CollectionOfTraces->GetListTimePointsFromTraceIDs(iDatabaseConnector, ioListMeshIDs);
    if ( !ListTimePoints.empty() )
      {
      std::string                         MeshIDToPrint = "";
      std::list< unsigned int >::iterator iter = ListTimePoints.begin();
      while ( iter != ListTimePoints.end() )
        {
        unsigned int MeshIDKickedOut =
          this->ReassignTrackIDForPreviousMeshWithSameTimePoint(
            iDatabaseConnector, iTrackID, *iter);
        if ( MeshIDKickedOut != 0 )
          {
          ioNullListMeshIDs.push_back(MeshIDKickedOut);
          MeshIDToPrint += ConvertToString< unsigned int >(MeshIDKickedOut);
          MeshIDToPrint += ", ";
          }
        ++iter;
        }
      if ( !MeshIDToPrint.empty() )
        {
        MeshIDToPrint = MeshIDToPrint.substr(0, MeshIDToPrint.size() - 2);
        MessageToPrint += "The trackID of the meshes ";
        MessageToPrint += MeshIDToPrint;
        MessageToPrint += " have been reassigned to 0";
        }
      }
    MessageQString = MessageToPrint.c_str();
    }
  return MessageQString;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoDBMeshManager::CheckListMeshesFromDifferentTimePoints(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListMeshIDs,
  std::list< unsigned int > & ioListMeshIDsToBePartOfTrack,
  std::list< unsigned int > & ioListMeshIDsToReassign)
{
  std::string MessageToPrint = "";

  ioListMeshIDsToBePartOfTrack = iListMeshIDs;

  std::list< unsigned int >::iterator iter;

  if ( !iListMeshIDs.empty() )
    {
    std::list< unsigned int > TimePointsWithSeveralMeshes =
      this->m_CollectionOfTraces->GetTimePointWithSeveralTracesFromTheList(
        iDatabaseConnector, iListMeshIDs);

    iter = TimePointsWithSeveralMeshes.begin();

    while ( iter != TimePointsWithSeveralMeshes.end() )
      {
      int MaxMeshIDForTimePoint =
        this->m_CollectionOfTraces->GetMaxTraceIDsForSpecificTimePoint(
          iDatabaseConnector, iListMeshIDs, *iter);
      if ( MaxMeshIDForTimePoint != -1 )
        {
        std::list< unsigned int > TraceIDs =
          this->m_CollectionOfTraces->GetNonMaxTraceIDsForSpecificTimePoint(
            iDatabaseConnector, iListMeshIDs, *iter, MaxMeshIDForTimePoint);
        std::copy( TraceIDs.begin(), TraceIDs.end(), std::back_inserter(ioListMeshIDsToReassign) );
        std::list< unsigned int >::iterator iterTraceIDToRemove = TraceIDs.begin();
        while ( iterTraceIDToRemove != TraceIDs.end() )
          {
          std::list< unsigned int >::iterator Find =
            std::find(ioListMeshIDsToBePartOfTrack.begin(), ioListMeshIDsToBePartOfTrack.end(),
                      *iterTraceIDToRemove);
          ioListMeshIDsToBePartOfTrack.erase(Find);
          ++iterTraceIDToRemove;
          }
        }
      ++iter;
      }
    if ( !ioListMeshIDsToReassign.empty() )
      {
      MessageToPrint = "Warning: the meshIDs ";
      std::list< unsigned int >::iterator iterIDs = ioListMeshIDsToReassign.begin();
      while ( iterIDs != ioListMeshIDsToReassign.end() )
        {
        std::string temp = ConvertToString< unsigned int >(*iterIDs);
        MessageToPrint += temp;
        MessageToPrint += ", ";
        ++iterIDs;
        }
      MessageToPrint  = MessageToPrint.substr(0, MessageToPrint.size() - 1);
      MessageToPrint  += "have not been reassigned ";
      MessageToPrint += "to the trackID because several meshes were selected for the same ";
      MessageToPrint += "timepoints ";

      iter = TimePointsWithSeveralMeshes.begin();

      while ( iter != TimePointsWithSeveralMeshes.end() )
        {
        std::string temp = ConvertToString< unsigned int >(*iter);
        MessageToPrint += temp;
        MessageToPrint += ", ";
        ++iter;
        }
      MessageToPrint = MessageToPrint.substr(0, MessageToPrint.size() - 2);
      }
    }
  return MessageToPrint;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int >
QGoDBMeshManager::GetMeshesWithTimePointInfToTheCheckedOne(
  unsigned int iTrackID, vtkMySQLDatabase *iDatabaseConnector,
  std::list< unsigned int > iListMeshesBelongingToTrack)
{
  std::list< unsigned int >               oListMeshesTimePointInf = std::list< unsigned int >();
  std::pair< unsigned int, unsigned int > InfoSplitMesh =
    this->GetInfoForTheOnlyOneCheckedMeshOfTheTrack(iDatabaseConnector, iTrackID);
  if ( InfoSplitMesh.first != 0 )
    {
    oListMeshesTimePointInf = this->m_CollectionOfTraces->GetTraceIDsWithTimePointInf(
        iDatabaseConnector, iListMeshesBelongingToTrack, InfoSplitMesh.second);
    //oListMeshesTimePointSup.push_back(InfoSplitMesh.first);
    }
  return oListMeshesTimePointInf;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::pair< unsigned int, unsigned int > QGoDBMeshManager::GetInfoForTheOnlyOneCheckedMeshOfTheTrack(
  vtkMySQLDatabase *iDatabaseConnector,
  unsigned int
  iTrackID)
{
  std::pair< unsigned int, unsigned int > oInfo =
    std::pair< unsigned int, unsigned int >(0, 0);
  std::list< unsigned int > ListCheckedMeshes =
    this->m_MeshContainerInfoForVisu->GetHighlightedElementsTraceID();
  std::list< unsigned int > ListCheckedMeshesBelongingToTrackID =
    std::list< unsigned int >();
  if ( !ListCheckedMeshes.empty() )
    {
    ListCheckedMeshesBelongingToTrackID =
      this->m_CollectionOfTraces->GetTraceIDsBelongingToCollectionID(
        iDatabaseConnector, ListCheckedMeshes, iTrackID);
    }
  if ( ListCheckedMeshesBelongingToTrackID.size() != 1 )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select one and only one Mesh where to split the Track") );
    msgBox.exec();
    return oInfo;
    }

  unsigned int CheckedMesh = ListCheckedMeshesBelongingToTrackID.front();
  oInfo.first = CheckedMesh;
  std::list< unsigned int > MeshesIDs;
  MeshesIDs.push_back(CheckedMesh);
  std::list< unsigned int > ListTimePoints =
    this->m_CollectionOfTraces->GetTimePointsForTraceIDs(iDatabaseConnector, MeshesIDs);

  if ( ListTimePoints.size() != 1 )
    {
    std::cout << "more than one timepoint";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return oInfo;
    }
  oInfo.second = ListTimePoints.front();
  return oInfo;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< QGoDBTraceManager::NameWithColorData >
QGoDBMeshManager::GetAllTraceIDsWithColor(
  vtkMySQLDatabase *iDatabaseConnector, std::string & ioIDToSelect)
{
  ioIDToSelect = this->m_LastSelectedTraceAsCollection;
  return this->m_CollectionOfTraces->GetTracesIDsWithColorForATimePoint(
           iDatabaseConnector, *this->m_CurrentTimePoint);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< std::pair<unsigned int, double> >
QGoDBMeshManager::
GetListVolumes()
{
  std::list< std::pair<unsigned int, double> > oList;

  QGoTableWidget* tableWidget = this->GetTableWidget();

  std::list<unsigned int> list = this->GetListHighlightedIDs();
  std::list<unsigned int>::iterator it = list.begin();

  while(it!=list.end())
    {
    double volume = (tableWidget->GetValue( *it, "mesh", "Volume" )).toDouble();
    int trackID = (tableWidget->GetValue( *it, "mesh", "trackID" )).toInt();

    std::pair<unsigned int, double> trackAndVolume(trackID, volume);
    oList.push_back(trackAndVolume);
    ++it;
    }

  return oList;

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< std::pair<unsigned int, double> >
QGoDBMeshManager::
GetListVolumes(std::list<unsigned int> iMeshIDs)
{
  std::list< std::pair<unsigned int, double> > oList;
  QGoTableWidget* tableWidget = this->GetTableWidget();

  std::list<unsigned int>::iterator it = iMeshIDs.begin();

  while(it!=iMeshIDs.end())
    {
    double volume = (tableWidget->GetValue( *it, "mesh", "Volume" )).toDouble();
    int trackID = (tableWidget->GetValue( *it, "mesh", "trackID" )).toInt();

    std::pair<unsigned int, double> trackAndVolume(trackID, volume);
    oList.push_back(trackAndVolume);
    ++it;
    }

  return oList;

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
double
QGoDBMeshManager::
GetVolume(unsigned int iMeshID)
{
  return (this->GetTableWidget()->GetValue( iMeshID, "mesh", "Volume" )).toDouble();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoDBMeshManager::
CleanTWAndContainerForGivenTimePoint(vtkMySQLDatabase *iDatabaseConnector,
                                     const std::list<unsigned int>& iTimePoints)
{
  this->RemoveTracesFromTWAndContainerForVisuForSpecificTPsTemplate<MeshContainer>(
          iDatabaseConnector, this->m_MeshContainerInfoForVisu, iTimePoints);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoDBMeshManager::
ModifyTrackIDInVisuContainer(unsigned int iTrackID,
                                  std::list< unsigned int > iToTrack,
                                  std::list< unsigned int > iToNull)
{
  m_MeshContainerInfoForVisu->AssignToGivenCollection(iTrackID, iToTrack);
  m_MeshContainerInfoForVisu->AssignToGivenCollection(0, iToNull);
}
