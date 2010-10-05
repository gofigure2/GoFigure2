/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
  Date: $Date$  // Date of last commit
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
#include "QGoPrintDatabase.h"

#include <QStringList>
#include <QWidget>
#include <QDialog>
#include <QInputDialog>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QMenu>
#include <QCloseEvent>
#include <QPixmap>

#include <iostream>

#include "vtkActor.h"
#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStringArray.h"
#include "vtkStdString.h"

#include "QGoTableWidget.h"
#include "GoDBRecordSet.h"
#include "GoDBRecordSetHelper.h"
#include "GoDBContourRow.h"
#include "GoDBMeshRow.h"
#include "GoDBTrackRow.h"
#include "GoDBLineageRow.h"
#include "GoDBCoordinateRow.h"
#include "GoDBColorRow.h"
#include "QueryDataBaseHelper.h"
#include "ConvertToStringHelper.h"
#include "GoDBTraceInfoForTableWidget.h"
#include "GoDBExport.h"
#include "GoDBImport.h"

#include "ContourMeshContainer.h"

//--------------------------------------------------------------------------
QGoPrintDatabase::
QGoPrintDatabase(QWidget* iParent) :
  QWidget(iParent),
  m_ContoursData(NULL),
  m_MeshesData(NULL),
  m_TracksData(NULL),
  //m_LineagesData(NULL),
  m_DatabaseConnector(NULL),
  m_IsDatabaseUsed(false)
  {
  this->setupUi(this);
  DBTabWidget->setTabShape(QTabWidget::Triangular);
  DBTabWidget->removeTab(0);

  this->setContextMenuPolicy(Qt::CustomContextMenu);

  m_VisibilityAction = new QAction(tr("Show/hide the table widget"), this);
  QIcon TableWidgetIcon;
  TableWidgetIcon.addPixmap(QPixmap(QString::fromUtf8(":/fig/TableWidget.png")),
                            QIcon::Normal, QIcon::Off);
  m_VisibilityAction->setIcon(TableWidgetIcon);
  m_VisibilityAction->setCheckable(true);

  this->m_TraceManualEditingDockWidget =
    new QGoTraceManualEditingDockWidget(this);

  this->m_TraceWidget =
    this->m_TraceManualEditingDockWidget->m_TraceWidget;

  this->m_CellTypeManager = new QGoDBCellTypeManager(this);

  this->m_SubCellTypeManager = new QGoDBSubCellTypeManager(this);

  this->m_ColorManager = new QGoDBColorManager(this);

  this->CreateConnectionsForTraceManualEditingWidget();
  this->m_ReeditMode = false;

  QObject::connect(m_VisibilityAction, SIGNAL(toggled(bool)),
                   this, SLOT(setVisible(bool)));

  QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
                   this, SLOT(CreateContextMenu(const QPoint &)));
  QObject::connect(this->DBTabWidget, SIGNAL(currentChanged(int)),
                   this, SLOT(TheTabIsChanged(int)));

  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoPrintDatabase::~QGoPrintDatabase()
  {
  // Need to release memory allocated for contours, meshes, tracks and lineages
    // contours
  if (m_ContoursManager)
    {
    delete m_ContoursManager;
    }
  // meshes
  if (m_MeshesManager)
    {
    delete m_MeshesManager;
    }
  // tracks
  if (m_TracksManager)
    {
    delete m_TracksManager;
    }
  // lineages
 // if (m_LineagesManager)
 //   {
 //   delete m_LineagesManager;
 //   }
  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QAction*
QGoPrintDatabase::
toggleViewAction()
{
  return m_VisibilityAction;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetDatabaseVariables(
  const std::string& iNameDB, const std::string& iServer,
  const std::string& iUser, const std::string& iPassword,
  const unsigned int& iImgSessionID, const std::string& iImgSessionName)
{
  m_Server         = iServer;
  m_User           = iUser;
  m_Password       = iPassword;
  m_DBName         = iNameDB;
  m_ImgSessionID   = iImgSessionID;
  m_ImgSessionName = iImgSessionName;

  this->SetTracesManager();
  this->m_BookmarkManager = new QGoDBBookmarkManager(this, this->m_ImgSessionID);
  QObject::connect(this->m_BookmarkManager, SIGNAL(ListBookmarksChanged()),
                   this, SIGNAL(OpenBookmarksToUpdate()));

  this->InitializeTheComboboxesNotTraceRelated();
  this->SetTMListCollectionID();
  emit DBVariablesSet();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::OpenDBConnection()
{
  if (m_DatabaseConnector == 0)
    {
    this->m_DatabaseConnector = OpenDatabaseConnection(m_Server, m_User,
                                                 m_Password, m_DBName);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CloseDBConnection()
{
  if (CloseDatabaseConnection(m_DatabaseConnector))
    {
    this->m_DatabaseConnector = 0;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::FillTableFromDatabase()
{
  OpenDBConnection();
  this->GetContentAndDisplayAllTracesInfo(this->m_DatabaseConnector);
  CloseDBConnection();

  QString title = QString("Imaging Session: %1 ").arg(m_ImgSessionName.c_str());
  this->setWindowTitle(title);
  this->DBTabWidget->blockSignals(true);
  this->DBTabWidget->removeTab(0);
  this->DBTabWidget->addTab(this->m_ContoursManager->GetTableWidget(), "contour");
  this->DBTabWidget->addTab(this->m_MeshesManager->GetTableWidget(), "mesh");
  this->DBTabWidget->addTab(this->m_TracksManager->GetTableWidget(), "track");
  this->DBTabWidget->blockSignals(false);

  m_IsDatabaseUsed = true;
  emit PrintDBReady();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::closeEvent(QCloseEvent* iEvent)
{
  (void) iEvent;
  m_VisibilityAction->setChecked(false);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*void QGoPrintDatabase::CreateContextMenu(const QPoint& iPos)
{
  QMenu* ContextMenu = new QMenu;
  //NeedCurrentSelectedCollectionID();
  std::string TraceName = this->InWhichTableAreWe();

  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);

  /** \todo when using lineages, remove the following
  if (TraceName != "track") //for the time being, as we don't use lineages
    {
    ContextMenu->addAction(tr("Go to this %1")
                           .arg(CurrentlyUsedTraceData->TraceName.c_str()),
                           this, SLOT(GoToTheTrace()));
    // ContextMenu->addAction(tr("Show only the checked %1s")
    //   .arg(CurrentlyUsedTraceData->TraceName.c_str()),
    // this,SIGNAL(ShowCheckedTracesActivated()));
    ContextMenu->addAction(tr("Create a new %1 from checked %2s")
                           .arg(CurrentlyUsedTraceData->CollectionName.c_str())
                           .arg(CurrentlyUsedTraceData->TraceName.c_str()),
                           this, SLOT(CreateCorrespondingCollection()));
    ContextMenu->addAction(
      tr("Add to selected %1 : %2").arg(CurrentlyUsedTraceData->CollectionName.c_str())
      .arg(this->m_SelectedCollectionData.first.c_str()), this, SLOT(AddToSelectedCollection()));
    ContextMenu->addAction(tr("ReEdit the checked %1").arg(TraceName.c_str()),
                           this, SLOT(ReEditTrace()));
    }

  ContextMenu->addAction(tr("Check the selected %1s")
                         .arg(CurrentlyUsedTraceData->TraceName.c_str()), this, SLOT(CheckSelectedRows()));
  ContextMenu->addAction(tr("Uncheck the selected %1s")
                         .arg(CurrentlyUsedTraceData->TraceName.c_str()), this, SLOT(UncheckSelectedRows()));
  ContextMenu->addAction(tr("Show the selected %1s")
                         .arg(CurrentlyUsedTraceData->TraceName.c_str()), this, SLOT(ShowSelectedRows()));
  ContextMenu->addAction(tr("Hide the selected %1s")
                         .arg(CurrentlyUsedTraceData->TraceName.c_str()), this, SLOT(HideSelectedRows()));

  ContextMenu->addAction(tr("Change the color for the checked %1 to the selected color").arg(TraceName.c_str()),
                         this, SLOT(ChangeTraceColor()));
  ContextMenu->addAction(tr("Delete checked %1s").arg(TraceName.c_str()),
                         this, SLOT(DeleteTraces()));
  ContextMenu->addAction(tr("Copy Selection"),
                         CurrentlyUsedTraceData->Table, SLOT(CopySelection()));
  ContextMenu->addAction(tr("Copy table"), CurrentlyUsedTraceData->Table, SLOT(CopyTable()));
  ContextMenu->exec(this->mapToGlobal(iPos));
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string QGoPrintDatabase::InWhichTableAreWe()
{
  int CurrentIndex = this->DBTabWidget->currentIndex();
  return this->DBTabWidget->tabText(CurrentIndex).toStdString();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*void QGoPrintDatabase::AddToSelectedCollection()
{
  /*std::string         TraceName = this->InWhichTableAreWe();
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);
  std::list<int>      ListSelectedTraces = CurrentlyUsedTraceData->Table->GetListCheckedTraceID();
  //emit                NeedCurrentSelectedCollectionID();

  if (ListSelectedTraces.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1 to be part of the %2 %3")
      .arg(CurrentlyUsedTraceData->TraceName.c_str())
      .arg(CurrentlyUsedTraceData->CollectionName.c_str())
      .arg(this->m_SelectedCollectionData.first.c_str()));
    msgBox.exec();
    }
  else
    {
    OpenDBConnection();
    //refactoring
   // this->AddListTracesToACollection(ListSelectedTraces,
                               //      this->m_SelectedCollectionData, TraceName, false);
    if (CurrentlyUsedTraceData->TraceName == "contour")
      {
      std::list<int> ListIDs = CurrentlyUsedTraceData->CollectionOfTraces->GetTracesIDPartOfTheCollection(
        this->m_DatabaseConnector,
        atoi(this->m_SelectedCollectionData.first.c_str()));
      //emit MeshGenerationToUpdate (ListIDs);
      }
    CloseDBConnection();
    }
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::
AddContoursFromDBForAGivenTimePoint(std::vector<int> iListIDs)
{
  this->OpenDBConnection();

  //ContourMeshStructureMultiIndexContainer* ContoursInfoToAdd =
   // GetTracesInfoFromDB(this->m_DatabaseConnector, "contour", "mesh",
                      //  this->m_ImgSessionID, iTimePoint, iListIDs);
  /*GetTracesInfoFromDB(this->m_ContoursContainer,this->m_DatabaseConnector,
    "contour","mesh",this->m_ImgSessionID,iTimePoint,iListIDs);

  for (unsigned int i = 0; i < iListIDs.size(); i++)
    {
    this->AddATraceToContourMeshInfo("contour", iListIDs.at(i));
    }*/
  std::list<unsigned int> ListIDs(iListIDs.begin(),iListIDs.end());
  this->m_ContoursManager->UpdateTWAndContainerForExistingTraces(
    this->m_DatabaseConnector,ListIDs);

  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*ContourMeshStructureMultiIndexContainer*
QGoPrintDatabase::
GetContoursMultiIndexFromDBForAGivenTimePoint(int iTimePoint,
                                              std::vector<int> iListIDs)
{
  this->OpenDBConnection();
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure("contour");

  ContourMeshStructureMultiIndexContainer* ContoursInfo =
    GetTracesInfoFromDBMultiIndex(this->m_DatabaseConnector,
                                  CurrentlyUsedTraceData->TraceName, CurrentlyUsedTraceData->CollectionName,
                                  "ImagingSessionID", this->m_ImgSessionID, iTimePoint, iListIDs);

  this->CloseDBConnection();
  return ContoursInfo;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*ContourMeshStructureMultiIndexContainer* QGoPrintDatabase::
GetMeshesMultiIndexFromDBForAGivenTimePoint(int iTimePoint,
                                            std::vector<int> iListIDs)
{
  this->OpenDBConnection();
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(
    "mesh");
  ContourMeshStructureMultiIndexContainer* MeshesInfo =
    GetTracesInfoFromDBMultiIndex(this->m_DatabaseConnector,
                                  CurrentlyUsedTraceData->TraceName, CurrentlyUsedTraceData->CollectionName,
                                  "ImagingSessionID", this->m_ImgSessionID, iTimePoint, iListIDs);

  this->CloseDBConnection();

  return MeshesInfo;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::
AddMeshesFromDBForAGivenTimePoint(std::vector<int> iListIDs)
{
  this->OpenDBConnection();

  //ContourMeshStructureMultiIndexContainer* MeshesInfoToAdd =
    //GetTracesInfoFromDB(this->m_DatabaseConnector, "mesh", "track",
      //                  this->m_ImgSessionID, iTimePoint, iListIDs);
  /*GetTracesInfoFromDB(this->m_MeshesContainer,this->m_DatabaseConnector,
    "mesh","track",this->m_ImgSessionID,iTimePoint,iListIDs);

  for (unsigned int i = 0; i < iListIDs.size(); i++)
    {
    this->AddATraceToContourMeshInfo("mesh", iListIDs.at(i));
    }*/
  std::list<unsigned int> ListIDs(iListIDs.begin(),iListIDs.end());
  this->m_MeshesManager->UpdateTWAndContainerForExistingTraces(
    this->m_DatabaseConnector,ListIDs);

  this->CloseDBConnection();

 // return MeshesInfoToAdd;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*ContourMeshStructureMultiIndexContainer* QGoPrintDatabase::
GetMeshesFromDBForAGivenTimePoint(int iTimePoint)
{
  this->OpenDBConnection();
  //ContourMeshStructureMultiIndexContainer* MeshesInfo = GetTracesInfoFromDB(
   // this->m_DatabaseConnector, "mesh", "track", m_ImgSessionID, iTimePoint);
  GetTracesInfoFromDB(this->m_MeshesContainer,this->m_DatabasConnector,
    "mesh","track",this->m_ImgSessionID,iTimePoint);
  this->CloseDBConnection();
  return MeshesInfo;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::SaveContoursFromVisuInDB(unsigned int iXCoordMin,
                                           unsigned int iYCoordMin,
                                           unsigned int iZCoordMin,
                                           unsigned int iTCoord,
                                           unsigned int iXCoordMax,
                                           unsigned int iYCoordMax,
                                           unsigned int iZCoordMax,
                                           vtkPolyData* iContourNodes)                                       
{
  OpenDBConnection();
  unsigned int ContourID;
  if(!this->m_ReeditMode)
    {
    unsigned int MeshID = 0;
    std::string test = this->m_SelectedCollectionData.first;
    if (this->m_SelectedCollectionData.first != "Add a new mesh ...")
    {
      MeshID = ss_atoi<unsigned int>(this->m_SelectedCollectionData.first);
    }
    ContourID = this->m_ContoursManager->SaveNewContourFromVisu(iXCoordMin,iYCoordMin,
      iZCoordMin,iTCoord,iXCoordMax,iYCoordMax,iZCoordMax,iContourNodes,this->m_DatabaseConnector,
      this->m_SelectedColorData,MeshID);
    }
  else
    {
    ContourID =  this->m_ContoursManager->SaveReeditedContourFromVisu(
      iXCoordMin,iYCoordMin,iZCoordMin,iTCoord,iXCoordMax,iYCoordMax,iZCoordMax,
      iContourNodes,this->m_DatabaseConnector);
    this->m_ReeditMode = false;
    this->m_TraceManualEditingDockWidget->setEnabled(true);
    }

   std::list<unsigned int> ListContours;
   ListContours.push_back(ContourID);   
   std::list<unsigned int> ListMeshes = 
     this->m_ContoursManager->GetListCollectionIDs(this->m_DatabaseConnector,ListContours);
   if (!ListMeshes.empty())
   {
   this->m_MeshesManager->UpdateBoundingBoxes(this->m_DatabaseConnector,ListMeshes);
   }

  CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoPrintDatabase::AddATraceToCurrentCollection(int iTraceID)
{

}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoPrintDatabase::IDWithColorData QGoPrintDatabase::
  UpdateContourFromVisuInDB(unsigned int iXCoordMin,
                            unsigned int iYCoordMin,
                            unsigned int iZCoordMin,
                            unsigned int iTCoord,
                            unsigned int iXCoordMax,
                            unsigned int iYCoordMax,
                            unsigned int iZCoordMax,
                            vtkPolyData* iContourNodes,
                            int ContourID)
{
  OpenDBConnection();
  GoDBContourRow contour_row(this->m_ImgSessionID);
  contour_row.SetValuesForSpecificID(ContourID,this->m_DatabaseConnector);
  contour_row.SaveInDB(this->m_DatabaseConnector);

  int CollectionID = FindOneID(this->m_DatabaseConnector,
                               "contour", "meshID", "contourID", ConvertToString<int>(ContourID));
  this->m_ContoursData->CollectionOfTraces->RecalculateDBBoundingBox(
    this->m_DatabaseConnector, CollectionID);
  this->UpdateTableWidgetForAnExistingTrace("mesh", CollectionID);
  this->UpdateTableWidgetForAnExistingTrace("contour", ContourID);
  IDWithColorData UpdatedContourData;
  UpdatedContourData.first = ContourID;
  QColor Color = this->GetQColorFromTraceRow<GoDBContourRow>(contour_row,
    this->m_DatabaseConnector);
  CloseDBConnection();
  //useful ?
  UpdatedContourData.second = Color;
  return UpdatedContourData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::
SaveMeshFromVisuInDB( unsigned int iXCoordMin,
                      unsigned int iYCoordMin,
                      unsigned int iZCoordMin,
                      unsigned int iTCoord,
                      unsigned int iXCoordMax,
                      unsigned int iYCoordMax,
                      unsigned int iZCoordMax,
                      vtkPolyData* iMeshNodes,
                      GoFigureMeshAttributes* iMeshAttributes)
                      //bool NewMesh,
                      //int iMeshID)
{
  OpenDBConnection();
  unsigned int NewMeshID = this->m_MeshesManager->SaveNewMeshFromVisu(iXCoordMin,iYCoordMin,
    iZCoordMin,iTCoord,iXCoordMax,iYCoordMax,iZCoordMax,iMeshNodes,this->m_DatabaseConnector,
    this->m_SelectedColorData,ss_atoi<unsigned int>(this->m_SelectedCollectionData.first),
    iMeshAttributes,this->m_SelectedCellType,this->m_SelectedSubCellType);

  std::list<unsigned int> ListNewMeshes;
  ListNewMeshes.push_back(NewMeshID);
  this->m_TracksManager->UpdateBoundingBoxes(this->m_DatabaseConnector,
    this->m_MeshesManager->GetListCollectionIDs(this->m_DatabaseConnector,ListNewMeshes));

  //emit NeedToGetCurrentSelectedColor();
  //emit NeedCurrentSelectedCollectionID();

  /*GoDBMeshRow                    mesh_row(this->m_ImgSessionID);
  std::pair<std::string, QColor> CollectionData;
  int                            SavedMeshID;

  if (!NewMesh)
    {
    int tempid = iMeshID;
    if (tempid == 0)
      {
      tempid = atoi(this->m_SelectedCollectionData.first.c_str());

      CollectionData.first = mesh_row.GetMapValue("TrackID");
      }
    mesh_row.SetValuesForSpecificID( tempid, this->m_DatabaseConnector );

    GoDBTrackRow Collection;
    Collection.SetValuesForSpecificID( atoi(CollectionData.first.c_str()),
                                       this->m_DatabaseConnector );

    /*QColor ColorCollection(atoi(Collection.GetMapValue("Red").c_str()),
                           atoi(Collection.GetMapValue("Green").c_str()),
                           atoi(Collection.GetMapValue("Blue").c_str()),
<<<<<<< .working
                           atoi(Collection.GetMapValue("Alpha").c_str()));
    CollectionData.second =
      this->GetQColorFromTraceRow<GoDBTrackRow>(
        Collection,
        this->m_DatabaseConnector);
=======
                           atoi(Collection.GetMapValue("Alpha").c_str()));*/
  /*  CollectionData.second = this->GetQColorFromTraceRow<GoDBTrackRow>(Collection,
      this->m_DatabaseConnector);
<<<<<<< .working
>>>>>>> .merge-right.r2033
=======
    CollectionData.second =
        this->GetQColorFromTraceRow<GoDBTrackRow>(Collection, this->m_DatabaseConnector );
>>>>>>> .merge-right.r2032
=======
>>>>>>> .merge-right.r2136
*/
      /*QColor ColorMesh(atoi(mesh_row.GetMapValue("Red").c_str()),
                           atoi(mesh_row.GetMapValue("Green").c_str()),
                           atoi(mesh_row.GetMapValue("Blue").c_str()),
                           atoi(mesh_row.GetMapValue("Alpha").c_str()));

    MeshData.first = atoi(mesh_row.GetMapValue("MeshID").c_str());

    //MeshData.second = this->GetQColorFromTraceRow<GoDBMeshRow>(mesh_row);
    MeshData.second = this->m_SelectedColorData.second;
    }*/
//refactoring:
  /*this->GetTraceRowFromVisu<GoDBMeshRow>( iXCoordMin,
                                          iYCoordMin,
                                          iZCoordMin,
                                          iTCoord,
                                          iXCoordMax,
                                          iYCoordMax,
                                          iZCoordMax,
                                          iMeshNodes,
                                          this->m_DatabaseConnector,
                                          mesh_row,
                                          0,
                                          iMeshAttributes);

  if (NewMesh)
    {
    //if the new mesh is created from a collection of contours,
    //there will be no collectionID from the tracemanualediting widget as
    //the collection in the widget will be mesh and not track
    mesh_row.SetColor( m_SelectedColorData.second.red(),
                       m_SelectedColorData.second.green(),
                       m_SelectedColorData.second.blue(),
                       m_SelectedColorData.second.alpha(),
                       m_SelectedColorData.first,
                       this->m_DatabaseConnector);

    if (this->InWhichTableAreWe() == "contour")
      {
      mesh_row.SetCollectionID(0);
      CollectionData.first = "0";
      QColor WhiteColor(255, 255, 255, 255);
      CollectionData.second = WhiteColor;
      }
    else
      {
      mesh_row.SetCollectionID(atoi(m_SelectedCollectionData.first.c_str()));
      CollectionData = this->m_SelectedCollectionData;
      }
    mesh_row.SetField("CellType", this->m_SelectedCellType);
    mesh_row.SetField("SubCellType", this->m_SelectedSubCellType);
    SavedMeshID = mesh_row.SaveInDB(this->m_DatabaseConnector);
    this->UpdateTableWidgetAndDBWithNewCreatedTrace("mesh",
                                                    iMeshAttributes);
    this->AddATraceToContourMeshInfo("mesh", SavedMeshID);
    MeshData.first = SavedMeshID;
    MeshData.second = this->m_SelectedColorData.second;
    }

  if (!NewMesh)
    {
    SavedMeshID = mesh_row.SaveInDB(this->m_DatabaseConnector);
    this->UpdateTableWidgetForAnExistingTrace(
      "mesh", atoi(mesh_row.GetMapValue("MeshID").c_str()),iMeshAttributes);
    }

  /** \todo check if it is needed for an updated mesh
  std::list<int> ListSelectedTraces;
  ListSelectedTraces.push_back(SavedMeshID);
  //refactoring
  //this->AddListTracesToACollection(
    //ListSelectedTraces, CollectionData, "mesh", false);*/

  CloseDBConnection();

  //return SavedMeshID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/** \todo check this method for the sphere/contours from one-click*/
int QGoPrintDatabase::CreateMeshFromOneClickSegmentation(
  std::list<int> iListContoursIDs)
{
  int                 NewMeshID = -1;
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure("contour");
  if (!iListContoursIDs.empty())
    {
    OpenDBConnection();
    //set the color for the new collection:
    GoDBMeshRow NewMesh;
    //emit NeedToGetCurrentSelectedColor();
    //emit NeedCurrentSelectedCollectionID();

    NewMesh.SetColor(this->m_SelectedColorData.second.red(), this->m_SelectedColorData.second.green(),
                     this->m_SelectedColorData.second.blue(), this->m_SelectedColorData.second.alpha(),
                     this->m_SelectedColorData.first, this->m_DatabaseConnector);
    NewMesh.SetCollectionID(atoi(this->m_SelectedCollectionData.first.c_str()));
    NewMesh.SetField<int>("CellTypeID",
                          this->m_CellTypeManager->GetTheEntityID(
                          this->m_SelectedCellType,this->m_DatabaseConnector));
    NewMesh.SetField<int>("SubCellularID",
                          this->m_SubCellTypeManager->GetTheEntityID(
                          this->m_SelectedSubCellType,this->m_DatabaseConnector));

    //create the collection in the database and get the corresponding ID:
    //refactoring:
      //NewMeshID = CurrentlyUsedTraceData->CollectionOfTraces->
      //            CreateNewCollectionFromSelection<GoDBMeshRow>(
      //  iListContoursIDs, this->m_DatabaseConnector, NewMesh);
    //end refactoring
    std::pair<std::string, QColor> NewCollectionInfo(
      ConvertToString(NewMeshID), this->m_SelectedColorData.second);
    //refactoring
    //this->AddListTracesToACollection(iListContoursIDs, NewCollectionInfo, "contour", true);

    CloseDBConnection();
    QString CollectionIDQString = ConvertToString<int>(NewMeshID).c_str();
    //emit NewCreatedCollection(this->m_CurrentColorData.second,CollectionIDQString);
    }
  return NewMeshID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure> QGoPrintDatabase::
GetContoursForAGivenTimepoint(unsigned int iTimePoint)
{
  return GetTracesForAGivenTimepoint(
           *this->m_ContoursData->ListTracesInfoForVisu, iTimePoint);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure> QGoPrintDatabase::
GetMeshesForAGivenTimepoint(unsigned int iTimePoint)
{
  return GetTracesForAGivenTimepoint(
           *this->m_MeshesData->ListTracesInfoForVisu, iTimePoint);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure> QGoPrintDatabase::
GetTracesForAGivenTimepoint(ContourMeshStructureMultiIndexContainer iAllTraces,
                            unsigned int iTimePoint)
{
  std::vector<ContourMeshStructure>                 SelectedTraces;
  ContourMeshStructureMultiIndexContainer::iterator iter = iAllTraces.begin();

  while (iter != iAllTraces.end())
    {
    if (iter->TCoord == iTimePoint)
      {
      SelectedTraces.push_back(*iter);
      }
    ++iter;
    }
  return SelectedTraces;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*std::vector<ContourMeshStructure> QGoPrintDatabase::
GetContoursForAGivenZCoord(unsigned int iZCoord)
{
  return this->GetTracesForAGivenZCoord(*this->m_ContoursData->ListTracesInfoForVisu, iZCoord,
                                        this->m_ContoursData->CollectionOfTraces);
}*/

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<unsigned int> QGoPrintDatabase::GetContoursForAGivenZCoord(unsigned int iZCoord)
{
  this->OpenDBConnection();
  std::list<unsigned int> ContourIDs = this->m_ContoursManager->GetAllTracesIDForAGivenZCoord(
    this->m_DatabaseConnector,iZCoord);
  this->CloseDBConnection();
  return ContourIDs;
}
/*ContourMeshStructureMultiIndexContainer* QGoPrintDatabase::
  GetContoursMultiIndexFromDBForAGivenZCoord(unsigned int iZCoord,
  std::vector<int> iListIDs)
{
  this->OpenDBConnection();
  ContourMeshStructureMultiIndexContainer* ContoursInfo;
  //get the IDs for the contours with the zcoord inside the bounding box
  //then use the GetTraceInfoStructure
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(
    "contour");
  ContourMeshStructureMultiIndexContainer* ContoursInfo =
    GetTracesInfoFromDBMultiIndex(this->m_DatabaseConnector,
    CurrentlyUsedTraceData->TraceName,CurrentlyUsedTraceData->CollectionName,
    this->m_ImgSessionID,iTimePoint,iListIDs);
  this->CloseDBConnection();
  return ContoursInfo;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*std::vector<ContourMeshStructure> QGoPrintDatabase::
GetMeshesForAGivenZCoord(unsigned int iZCoord)
{
  return this->GetTracesForAGivenZCoord(
           *this->m_MeshesData->ListTracesInfoForVisu, iZCoord,
           this->m_MeshesData->CollectionOfTraces);
}*/
std::list<unsigned int> QGoPrintDatabase::GetMeshesForAGivenZCoord(unsigned int iZCoord)
{
  this->OpenDBConnection();
  std::list<unsigned int> MeshesIDs = this->m_MeshesManager->GetAllTracesIDForAGivenZCoord(
    this->m_DatabaseConnector,iZCoord);
  this->CloseDBConnection();
  return MeshesIDs;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*std::vector<ContourMeshStructure> QGoPrintDatabase::
GetTracesForAGivenZCoord(ContourMeshStructureMultiIndexContainer iAllTraces,
                         unsigned int iZCoord, GoDBCollectionOfTraces* iCollectionOfTraces)
{
  std::vector<ContourMeshStructure> oSelectedTraces;
  GoDBTableWidgetContainer*         LinkToRowContainer =
    iCollectionOfTraces->GetLinkToRowContainer();
  std::list<std::string> ListOfSelectedTracesID =
    LinkToRowContainer->GetTracesIDForAGivenZCoord(iZCoord);

  ContourMeshStructureMultiIndexContainer::iterator iterAllTraces = iAllTraces.begin();
  std::list<std::string>::iterator                  iterTracesID = ListOfSelectedTracesID.begin();

  while (iterTracesID != ListOfSelectedTracesID.end())
    {
    std::string  TraceIDToFindStrg = *iterTracesID;
    unsigned int TraceIDToFind =
      static_cast<unsigned int>(atoi(TraceIDToFindStrg.c_str()));
    bool found = false;
    while ((iterAllTraces != iAllTraces.end()) && (found == false))
      {
      if (TraceIDToFind == iterAllTraces->TraceID)
        {
        oSelectedTraces.push_back(*iterAllTraces);
        found = true;
        }
      ++iterAllTraces;
      }
    ++iterTracesID;
    }
  return oSelectedTraces;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoPrintDatabase::IsDatabaseUsed()
{
  return m_IsDatabaseUsed;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<QGoPrintDatabase::ItemColorComboboxData>
QGoPrintDatabase::GetListCollectionIDFromDB(vtkMySQLDatabase* iDatabaseConnector)
{
  //OpenDBConnection();
  std::list<ItemColorComboboxData> EmptyList = std::list<ItemColorComboboxData>();
  std::string TraceName = this->m_TraceWidget->GetTraceName();
  if (TraceName == "contour")
    {
    return this->m_MeshesManager->GetAllTraceIDsWithColor(iDatabaseConnector,
      this->m_SelectedTimePoint);
    }
  if (TraceName == "mesh")
    {
    return this->m_TracksManager->GetAllTraceIDsWithColor(iDatabaseConnector);
    }
  //CloseDBConnection();
  return EmptyList;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SaveNewCollectionFromTraceWidgetInDBAndTW()
{
  this->OpenDBConnection();
  std::string TraceName = this->m_TraceWidget->GetTraceName();
  unsigned int NewCollectionID;
  if (TraceName == "contour")
  {
    NewCollectionID = this->m_MeshesManager->CreateNewMeshWithNoContourNoPoints(
      this->m_DatabaseConnector,
      this->m_SelectedColorData,this->m_SelectedTimePoint,this->m_SelectedCellType,
      this->m_SelectedSubCellType);
  }
  if (TraceName == "mesh")
  {
    NewCollectionID = this->m_TracksManager->CreateNewTrackWithNoMesh(
      this->m_DatabaseConnector,this->m_SelectedColorData);
  }
  ItemColorComboboxData NewCollectionData;
  NewCollectionData.first = ConvertToString<unsigned int>(NewCollectionID);
  NewCollectionData.second = this->m_SelectedColorData.second;
  this->m_TraceWidget->AddANewCollectionID(NewCollectionData);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::pair<std::string, QColor> QGoPrintDatabase::GetSelectedCollectionData()
{
  return this->m_SelectedCollectionData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateTableWidgetForAnExistingTrace(
  std::string iTraceName, int iTraceID,GoFigureMeshAttributes* iMeshAttributes)
{
  /*TraceInfoStructure*       CurrentlyUsedTraceData = this->GetTraceInfoStructure(iTraceName);
  GoDBTableWidgetContainer* LinkToUpdatedRow = CurrentlyUsedTraceData->CollectionOfTraces->
                                               GetLinkToUpdatedTraceContainer(this->m_DatabaseConnector, iTraceID);
  CurrentlyUsedTraceData->Table->UpdateRow(LinkToUpdatedRow, iTraceID, iTraceName,
                                           CurrentlyUsedTraceData->CollectionName);
  if (iMeshAttributes != 0)
    {
    this->PrintVolumeAreaForMesh(iMeshAttributes->m_Volume,
                                 iMeshAttributes->m_Area, iTraceID);
    }*/
  if (iTraceName == "contour")
    {
      //refactoring:
    //this->m_ContoursManager->DisplayInfoForExistingTrace(this->m_DatabaseConnector,
      //iTraceID);
    }
  if (iTraceName == "mesh")
    {
    this->m_MeshesManager->DisplayInfoForExistingTraceForMesh(this->m_DatabaseConnector,
      iTraceID,iMeshAttributes);
    }
  if (iTraceName == "track")
    {
    this->m_TracksManager->DisplayInfoForExistingTrace(this->m_DatabaseConnector,
      iTraceID);
    }

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
TraceInfoStructure* QGoPrintDatabase::
GetTraceInfoStructure(std::string iTraceName)
{
  if (iTraceName == "contour")
    {
    return this->m_ContoursData;
    }
  if (iTraceName == "mesh")
    {
    return this->m_MeshesData;
    }
  if (iTraceName == "track")
    {
    return this->m_TracksData;
    }
  if (iTraceName == "lineage")
    {
    return this->m_LineagesData;
    }
  std::cout << "error, this tracename is not related to any TracesData" << std::endl;

  return 0;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoPrintDatabase::AddListTracesToACollection(
  std::list<unsigned int> iListSelectedTraces, std::pair<std::string, QColor> iCollection,
  std::string iTraceName, bool IsANewCollection, int iCollectionID)
{
  if (iCollectionID == -1)
    {
    iCollectionID = atoi(iCollection.first.c_str());
    }
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure(iTraceName);
  //update the corresponding database data:
  //refactoring
      //std::list<int> ListCollectionsToUpdateInTableWidget =
      //  CurrentlyUsedTraceData->CollectionOfTraces->
      //  UpdateDBDataForAddedTracesToExistingCollection(iListSelectedTraces,
      //                                                 iCollectionID, this->m_DatabaseConnector);
  //end refactoring

  //update the RowContainer for traces with the new ID for the selected traces:
  //GoDBTableWidgetContainer* LinkToRowContainerForTraces =
  //CurrentlyUsedTraceData->CollectionOfTraces->GetLinkToRowContainer();
  //LinkToRowContainerForTraces->UpdateIDs(ListSelectedTraces,CollectionID);

  //update the Table Widget Display:
  //CurrentlyUsedTraceData->Table->UpdateIDs(iCollectionID,
                                        //   CurrentlyUsedTraceData->CollectionNameID, iCollection.second,
                                        //   CurrentlyUsedTraceData->TraceNameID, iListSelectedTraces);
  if (CurrentlyUsedTraceData->TraceName == "contour")
      {
        this->m_ContoursManager->DisplayInfoForExistingTraces(this->m_DatabaseConnector,iListSelectedTraces);
      }
    if (CurrentlyUsedTraceData->TraceName == "mesh")
      {
      this->m_MeshesManager->DisplayInfoForExistingTraces(this->m_DatabaseConnector,iListSelectedTraces);
      }
    if (CurrentlyUsedTraceData->TraceName == "track")
      {
      this->m_TracksManager->DisplayInfoForExistingTraces(this->m_DatabaseConnector,iListSelectedTraces);
      }
  //update the Collection Table with the new bounding box:
  CurrentlyUsedTraceData = this->GetTraceInfoStructure(CurrentlyUsedTraceData->CollectionName);
  if (IsANewCollection)
    {
    //this->UpdateTableWidgetAndDBWithNewCreatedTrace(
     // CurrentlyUsedTraceData->TraceName);
    }
  else
    {
    this->UpdateTableWidgetForAnExistingTrace(CurrentlyUsedTraceData->TraceName, iCollectionID);
    }

  //update the Collection Table with the old updated bounding box:
  //refactoring
  /*if (!ListCollectionsToUpdateInTableWidget.empty())
    {
    std::list<int>::iterator iter = ListCollectionsToUpdateInTableWidget.begin();
    while (iter != ListCollectionsToUpdateInTableWidget.end())
      {
      int TraceID = *iter;
      this->UpdateTableWidgetForAnExistingTrace(CurrentlyUsedTraceData->TraceName, TraceID);
      iter++;
      }
    }
  //end refactoring
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddBookmark(int iXCoord, int iYCoord,
                                   int iZCoord, int iTCoord)
{
  GoDBCoordinateRow BookmarkCoord;
  BookmarkCoord.SetField<int>("XCoord", iXCoord);
  BookmarkCoord.SetField<int>("YCoord", iYCoord);
  BookmarkCoord.SetField<int>("ZCoord", iZCoord);
  BookmarkCoord.SetField<int>("TCoord", iTCoord);

  this->OpenDBConnection();
  int BookmarkCoordID = BookmarkCoord.SaveInDB(this->m_DatabaseConnector);
  this->m_BookmarkManager->AddABookmark(BookmarkCoordID, this->m_DatabaseConnector);
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoPrintDatabase::NamesDescrContainerType QGoPrintDatabase::
GetListBookmarks()
{
  this->OpenDBConnection();
  NamesDescrContainerType ListBookmarks =
    this->m_BookmarkManager->GetListExistingEntities(this->m_DatabaseConnector);
  this->CloseDBConnection();
  return ListBookmarks;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBCoordinateRow QGoPrintDatabase::GetCoordinateForBookmark(
  std::string iName)
{
  this->OpenDBConnection();
  GoDBCoordinateRow Coord = this->m_BookmarkManager->
                            GetCoordinatesForBookmark(this->m_DatabaseConnector, iName);
  this->CloseDBConnection();
  return Coord;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteBookmarks()
{
  this->OpenDBConnection();
  this->m_BookmarkManager->DeleteBookmark(this->m_DatabaseConnector);
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateSelectedTimePoint(int iTimePoint)
{

  this->m_SelectedTimePoint = iTimePoint;
  if (!this->m_Server.empty()) //if empty, the database variables are not
    //been set up yet.
  {
  if (this->m_TraceWidget->GetTraceName() == "contour")
    {
    //if we change the timepoint, the list of meshes will be different from the
    //previous one, so, initialize the list is needed (no pre-selected collection)
    this->SetTMListCollectionID();
    }
  }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::TheTabIsChanged(int iIndex)
{
  this->m_TraceManualEditingDockWidget->show();
  //this->m_TraceManualEditingDockWidget->setEnabled(true);
  switch (iIndex)
    {
    case 1:
      this->UpdateWidgetsForCorrespondingTrace("mesh","track",false);
      break;
    case 2:
      this->UpdateWidgetsForCorrespondingTrace("track","lineage",false);
      break;
    case 3:
      this->UpdateWidgetsForCorrespondingTrace("lineage","None",false);
      break;
    default:
      this->UpdateWidgetsForCorrespondingTrace("contour","mesh",false);
      break;
    }
  emit TableWidgetTabChanged();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTable(std::string iTablename)
{
  int Index = 0;
  if (iTablename == "mesh")
    {
    Index = 2;
    }
  if (iTablename == "track")
    {
    Index = 3;
    }
  if (iTablename == "lineage")
    {
    Index = 4;
    }
  this->DBTabWidget->setCurrentIndex(Index);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::ExportContours()
{
  QString p = QFileDialog::getSaveFileName(this,
                                           tr("Save Contour Export File"), "", tr("TextFile (*.txt)"));
  if (!p.isNull())
    {
    QFileInfo   pathInfo(p);
    std::string filename = p.toStdString();

    GoDBExport ExportHelper(this->m_Server, this->m_User,
                            this->m_Password, this->m_ImgSessionID, filename);
    ExportHelper.ExportContours();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::ExportMeshes()
{
  QString p = QFileDialog::getSaveFileName(this,
                                           tr("Save Contour Export File"), "", tr("TextFile (*.txt)"));
  if (!p.isNull())
    {
    QFileInfo   pathInfo(p);
    std::string filename = p.toStdString();

    GoDBExport ExportHelper(this->m_Server, this->m_User,
                            this->m_Password, this->m_ImgSessionID, filename);
    ExportHelper.ExportMeshes();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::ImportContours(int iTimePoint)
{
  //ContourMeshStructureMultiIndexContainer* ContoursForVisu = NULL;
  QString                                  p = QFileDialog::getOpenFileName(this,
                                                                            tr("Open Contour Export File"), "",
                                                                            tr("TextFile (*.txt)"));
   //refactoring
 /* if (!p.isNull())
    {
    QFileInfo   pathInfo(p);
    std::string filename = p.toStdString();
    //import into the database:
    GoDBImport ImportHelper(this->m_Server, this->m_User,
                            this->m_Password, this->m_ImgSessionID, filename);
    ImportHelper.ImportContours();

    std::vector<int> NewContourIDs = ImportHelper.GetVectorNewContourIDs();
    std::vector<int> NewMeshIDs = ImportHelper.GetVectorNewMeshIDs();
    std::vector<int> NewTrackIDs = ImportHelper.GetVectorNewTracksIDs();

    //std::vector<int> ContourToAddTW = ImportHelper.GetVectorNewContourIDs();
    //add the imported traces in the table widget:

    this->AddTracesInTableWidgetFromDB(NewContourIDs, "contour");
    this->AddTracesInTableWidgetFromDB(NewMeshIDs, "mesh");
    this->AddTracesInTableWidgetFromDB(NewTrackIDs, "track");

    ContoursForVisu = this->AddContoursFromDBForAGivenTimePoint(iTimePoint,
                                                                NewContourIDs);
    }
  return ContoursForVisu;*/
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//ContourMeshStructureMultiIndexContainer* QGoPrintDatabase::
void QGoPrintDatabase::ImportMeshes(ContourMeshContainer* iContainer,int iTimePoint)
{
  //ContourMeshStructureMultiIndexContainer* MeshesForVisu = NULL;
  QString                                  p = QFileDialog::getOpenFileName(this,
                                                                            tr("Open Contour Export File"), "",
                                                                            tr("TextFile (*.txt)"));
  //refactoring
  /*
  if (!p.isNull())
    {
    QFileInfo   pathInfo(p);
    std::string filename = p.toStdString();
    //import into the database:
    GoDBImport ImportHelper(this->m_Server, this->m_User,
                            this->m_Password, this->m_ImgSessionID, filename);
    ImportHelper.ImportMeshes();

    std::vector<int> NewMeshIDs = ImportHelper.GetVectorNewMeshIDs();
    std::vector<int> NewTrackIDs = ImportHelper.GetVectorNewTracksIDs();

    //std::vector<int> ContourToAddTW = ImportHelper.GetVectorNewContourIDs();
    //add the imported traces in the table widget:

    this->AddTracesInTableWidgetFromDB(NewMeshIDs, "mesh");
    this->AddTracesInTableWidgetFromDB(NewTrackIDs, "track");

    MeshesForVisu = this->AddMeshesFromDBForAGivenTimePoint(iTimePoint,
                                                            NewMeshIDs);
    }
  return MeshesForVisu;*/
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//******related to TraceManualEditingDockWidget:****************************
QGoTraceManualEditingDockWidget* QGoPrintDatabase::
GetTraceManualEditingDockWidget()
{
  return this->m_TraceManualEditingDockWidget;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::CreateConnectionsForTraceManualEditingWidget()
{
  QObject::connect(this->m_TraceWidget,
                   SIGNAL(NewSelectedColorActivated(ItemColorComboboxData)),
                   this,
                   SLOT(UpdateSelectedColorData(ItemColorComboboxData)));

  QObject::connect(this->m_TraceWidget,
                   SIGNAL(NewCollectionActivated(ItemColorComboboxData)),
                   this,
                   SLOT(UpdateSelectedCollectionID(ItemColorComboboxData)));

  QObject::connect(this->m_TraceWidget,
                   SIGNAL(NewCellTypeActivated(std::string)),
                   this,
                   SLOT(UpdateSelectedCellType(std::string)));

  QObject::connect(this->m_TraceWidget,
                   SIGNAL(NewSubCellTypeActivated(std::string)),
                   this,
                   SLOT(UpdateSelectedSubCellType(std::string)));

  QObject::connect(this->m_TraceWidget,
                   SIGNAL(AddNewColor()),
                   this,
                   SLOT(AddNewColor()));

  QObject::connect(this->m_TraceWidget,
                   SIGNAL(DeleteColor()),
                   this,
                   SLOT(DeleteColor()));

  QObject::connect(this->m_TraceWidget,
                   SIGNAL(NewCollectionToBeCreated()),
                   this,
                   SLOT(SaveNewCollectionFromTraceWidgetInDBAndTW()));

  QObject::connect(this->m_TraceWidget,
                   SIGNAL(AddANewCellType()),
                   this,
                   SLOT(AddNewCellType()));
  QObject::connect(this->m_TraceWidget,
                   SIGNAL(AddANewSubCellType()),
                   this,
                   SLOT(AddNewSubCellType()));

  QObject::connect(this->m_TraceWidget,
                   SIGNAL(DeleteCellType()),
                   this,
                   SLOT(DeleteCellType()));

  QObject::connect(this->m_TraceWidget,
                   SIGNAL(DeleteSubCellType()),
                   this,
                   SLOT(DeleteSubCellType()));

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateWidgetsForCorrespondingTrace(std::string iTraceName,
  std::string iCollectionName,bool UpdateTableWidget)
{
   this->m_TraceWidget->UpdateTraceAndCollection(iTraceName, iCollectionName);
   this->SetTMListCollectionID();
   if(UpdateTableWidget)
     {
     //if the TableWidget has to be set to match the trace name, no need for the
     //signal TabHasChanged to be emitted, it would results in the Segmentation
     //widgets to be hidden
     this->blockSignals(true);
     this->SetTable(iTraceName);
     this->blockSignals(false);
     }
   this->m_TraceManualEditingDockWidget->show();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::InitializeTheComboboxesNotTraceRelated()
{
  this->SetTMListColors();
  this->SetTMListCellTypes();
  this->SetTMListSubCellTypes();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::
SetTMListColors(std::string iNewColorToSelect)
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListColors(
    this->m_ColorManager->GetListExistingColors(this->m_DatabaseConnector),
    iNewColorToSelect);
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::
SetTMListCollectionID(std::string iIDToSelect)
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListCollectionID(
    this->GetListCollectionIDFromDB(this->m_DatabaseConnector),
    iIDToSelect);
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTMListCellTypes(std::string iCellTypeToSelect)
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListCellTypes(
    this->m_CellTypeManager->GetListExistingEntities(this->m_DatabaseConnector),
    iCellTypeToSelect);
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTMListSubCellTypes(std::string iSubCellTypeToSelect)
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListSubCellTypes(
    this->m_SubCellTypeManager->GetListExistingEntities(this->m_DatabaseConnector),
    iSubCellTypeToSelect);
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateSelectedColorData(
  ItemColorComboboxData iSelectedColorData)
{
  this->m_SelectedColorData = iSelectedColorData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateSelectedCollectionID(
  ItemColorComboboxData iSelectedCollectionData)
{
  this->m_SelectedCollectionData = iSelectedCollectionData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateSelectedCellType(std::string iSelectedCellType)
{
  this->m_SelectedCellType = iSelectedCellType;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateSelectedSubCellType(std::string iSelectedSubCellType)
{
  this->m_SelectedSubCellType = iSelectedSubCellType;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddNewCellType()
{
  this->OpenDBConnection();
  std::string NewCellType = this->m_CellTypeManager->AddAnEntity(
    this->m_DatabaseConnector);
  if (!NewCellType.empty())
    {
    this->UpdateSelectedCellType(NewCellType);
    this->SetTMListCellTypes(NewCellType);
    }
  else //if the NewCellType is empty, go to the last selected one:
    {
    this->m_TraceWidget->SetCurrentCellType(this->m_SelectedCellType);
    }
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddNewSubCellType()
{
  this->OpenDBConnection();
  std::string NewSubCellType =
    this->m_SubCellTypeManager->AddAnEntity(this->m_DatabaseConnector);
  if(!NewSubCellType.empty())
    {
    this->UpdateSelectedSubCellType(NewSubCellType);
    this->SetTMListSubCellTypes(NewSubCellType);
    }
  else //if the NewSubCellType is empty, go to the last selected one:
    {
    this->m_TraceWidget->SetCurrentSubCellType(this->m_SelectedSubCellType);
    }
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddNewColor()
{
  this->OpenDBConnection();
  ItemColorComboboxData NewColor = this->m_ColorManager->AddANewColor(
    this->m_DatabaseConnector);
  if (!NewColor.first.empty())
    {
    this->UpdateSelectedColorData(NewColor);
    this->SetTMListColors(NewColor.first);
    }
  else //if the NewColor name is empty, go to the last selected one:
    {
    this->m_TraceWidget->SetCurrentColor(this->m_SelectedColorData.first);
    }
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteCellType()
{
  this->OpenDBConnection();
  if(this->m_CellTypeManager->DeleteEntity(this->m_DatabaseConnector))
    {
    this->SetTMListCellTypes(this->m_SelectedCellType);
    }
  else //if the user cancelled, go to the last selected one:
    {
    this->m_TraceWidget->SetCurrentCellType(this->m_SelectedCellType);
    }
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteColor()
{
  this->OpenDBConnection();
  if(this->m_ColorManager->DeleteEntity(this->m_DatabaseConnector))
    {
    this->SetTMListColors(this->m_SelectedColorData.first);
    }
  else //if the user cancelled, go to the last selected one:
    {
    this->m_TraceWidget->SetCurrentColor(this->m_SelectedColorData.first);
    }
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteSubCellType()
{
  this->OpenDBConnection();
  if (this->m_SubCellTypeManager->DeleteEntity(this->m_DatabaseConnector))
    {
    this->SetTMListSubCellTypes(this->m_SelectedSubCellType);
    }
  else //if the user cancelled, go to the last selected one:
    {
    this->m_TraceWidget->SetCurrentSubCellType(this->m_SelectedSubCellType);
    }
  this->CloseDBConnection();
}
//*********************RELATED TO QGODBTRACEMANAGER**************************

void QGoPrintDatabase::GetContentAndDisplayAllTracesInfo(
  vtkMySQLDatabase* iDatabaseConnector)
{
  this->m_ContoursManager->DisplayInfoAndLoadVisuContainerForAllContours(
    iDatabaseConnector);
  this->m_MeshesManager->DisplayInfoAndLoadVisuContainerForAllMeshes(
    iDatabaseConnector);
  this->m_TracksManager->DisplayInfoAndLoadVisuContainerForAllTracks(
    iDatabaseConnector);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::CreateContextMenu(const QPoint& iPos)
{
  std::string TraceName = this->m_TraceWidget->GetTraceName();
  if ( TraceName == "contour")
    {
    this->m_ContoursManager->CreateContextMenu(iPos);
    }
  if ( TraceName == "mesh")
    {
    this->m_MeshesManager->CreateContextMenu(iPos);
    }
  if ( TraceName == "track")
    {
    this->m_TracksManager->CreateContextMenu(iPos);
    }

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteContours()
{
  this->DeleteTraces<QGoDBContourManager,QGoDBMeshManager,QGoDBContourManager>(
    this->m_ContoursManager,this->m_MeshesManager,this->m_ContoursManager);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteMeshes()
{
  this->DeleteTraces<QGoDBMeshManager,QGoDBTrackManager,QGoDBContourManager>(
      this->m_MeshesManager,this->m_TracksManager,this->m_ContoursManager);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTracks()
{
  this->DeleteTraces<QGoDBTrackManager,QGoDBMeshManager,QGoDBMeshManager>(
        this->m_TracksManager,this->m_MeshesManager,this->m_MeshesManager,true);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetTracksManager()
{
  this->m_TracksManager = new QGoDBTrackManager(m_ImgSessionID,this);
  QObject::connect(this->m_TracksManager,
                   SIGNAL(TraceColorToChange()),
                   this,SLOT(ChangeTrackColor()));
  QObject::connect(this->m_TracksManager,SIGNAL(TracesToDelete()),
                   this,SLOT(DeleteTracks()));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::ChangeMeshColor()
{
  this->ChangeTraceColor<QGoDBMeshManager,QGoDBContourManager>(
    this->m_MeshesManager,this->m_ContoursManager);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::ChangeTrackColor()
{
  this->ChangeTraceColor<QGoDBTrackManager,QGoDBMeshManager>(
    this->m_TracksManager,this->m_MeshesManager);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::PrintVolumeAreaForMesh(GoFigureMeshAttributes*
     iMeshAttributes,unsigned int iMeshID)
{
  this->m_MeshesManager->UpdateOnlyVolumeAreaForExistingMesh(
    iMeshAttributes,iMeshID);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoPrintDatabase::
SetContoursContainer( ContourMeshContainer* iContainer )
{
  this->m_ContoursManager->SetTracesInfoContainerForVisu(iContainer);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoPrintDatabase::
SetMeshesContainer( ContourMeshContainer* iContainer )
{
  this->m_MeshesManager->SetTracesInfoContainerForVisu(iContainer);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoPrintDatabase::
SetTracksContainer( ContourMeshContainer* iContainer )
{
  this->m_TracksManager->SetTracesInfoContainerForVisu(iContainer);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetTracesManager()
{
  this->SetContoursManager();
  this->SetMeshesManager();
  this->SetTracksManager();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetContoursManager()
{
  this->m_ContoursManager = new QGoDBContourManager(m_ImgSessionID,this);

  QObject::connect(this->m_ContoursManager,SIGNAL(NeedToGetDatabaseConnection()),
                   this,SLOT(PassDBConnectionToContoursManager()));
  QObject::connect(this->m_ContoursManager,SIGNAL(NeedToGetCurrentSelectedColor()),
                   this,SLOT(PassSelectedColorToContoursManager()));
  //QObject::connect(this->m_ContoursManager,SIGNAL(NeedToGetCurrentSelectedCollection()).
                //   this,SLOT(PassSelectedCollectionToContoursManager()));
  QObject::connect(this->m_ContoursManager,SIGNAL(TracesToDelete()),
                   this,SLOT(DeleteContours()));
  QObject::connect(this->m_ContoursManager,
                   SIGNAL(NeedToGoToTheLocation(int,int,int,int)),
                   this,SIGNAL(NeedToGoToTheLocation(int,int,int,int)));
  QObject::connect(this->m_ContoursManager,
                   SIGNAL(TraceToReEdit(unsigned int)),
                   this,
                   SLOT(ReEditTrace(unsigned int)));
  QObject::connect(this->m_ContoursManager,
                   SIGNAL(NewCollectionFromCheckedTraces(std::list<unsigned int>)),
                   this,
                   SLOT(CreateNewMeshFromCheckedContours(std::list<unsigned int>)));
  QObject::connect(this->m_ContoursManager,
                   SIGNAL(CheckedTracesToAddToSelectedCollection(
                   std::list<unsigned int>)),this,
                   SLOT(AddCheckedContoursToSelectedMesh(std::list<unsigned int>)));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetMeshesManager()
{
  this->m_MeshesManager = new QGoDBMeshManager(m_ImgSessionID,this);
  QObject::connect(this->m_MeshesManager,
                   SIGNAL(TraceColorToChange()),
                   this,SLOT(ChangeMeshColor()));
  QObject::connect(this->m_MeshesManager,SIGNAL(TracesToDelete()),
                   this,SLOT(DeleteMeshes()));
  QObject::connect(this->m_MeshesManager,
                   SIGNAL(NeedToGoToTheLocation(int,int,int,int)),
                   this,SIGNAL(NeedToGoToTheLocation(int,int,int,int)));
  QObject::connect(this->m_MeshesManager, 
                   SIGNAL (NewCollectionFromCheckedTraces(std::list<unsigned int>)),
                   this,
                   SLOT(CreateNewTrackFromCheckedMeshes(std::list<unsigned int>)));
  QObject::connect(this->m_MeshesManager,
                   SIGNAL(CheckedTracesToAddToSelectedCollection(
                   std::list<unsigned int>)),this,
                   SLOT(AddCheckedMeshesToSelectedTrack(std::list<unsigned int>)));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::PassDBConnectionToContoursManager()
{
  this->OpenDBConnection();
  this->m_ContoursManager->SetDatabaseConnection(this->m_DatabaseConnector);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::PassSelectedColorToContoursManager()
{
  this->m_ContoursManager->SetSelectedColor(this->m_SelectedColorData);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//void QGoPrintDatabase::PassSelectedCollectionToContoursManager()
void QGoPrintDatabase::ReEditTrace(unsigned int iTraceID)
{
 this->m_ReeditMode = true;
 this->m_TraceManualEditingDockWidget->setEnabled(false);
 emit TraceToReEdit(iTraceID);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateNewTrackFromCheckedMeshes(
  std::list<unsigned int> iListCheckedMeshes)
{
  this->OpenDBConnection();
  unsigned int NewTrackID = this->m_TracksManager->CreateNewTrackWithNoMesh(
    this->m_DatabaseConnector,this->m_SelectedColorData);
  this->AddCheckedTracesToCollection<QGoDBMeshManager,QGoDBTrackManager>
    (this->m_MeshesManager,this->m_TracksManager,
    NewTrackID,iListCheckedMeshes);
  this->CloseDBConnection();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateNewMeshFromCheckedContours(
  std::list<unsigned int> iListCheckedContours)
{
  this->OpenDBConnection();

  unsigned int NewMeshID = 
    this->m_MeshesManager->CreateNewMeshWithNoContourNoPoints(
      this->m_DatabaseConnector,this->m_SelectedColorData,
      this->m_SelectedTimePoint,this->m_SelectedCellType,
      this->m_SelectedSubCellType);

  this->AddCheckedTracesToCollection<QGoDBContourManager,QGoDBMeshManager>
    (this->m_ContoursManager,this->m_MeshesManager,
     NewMeshID,iListCheckedContours);

  emit NewMeshToGenerate(iListCheckedContours,NewMeshID);

  this->CloseDBConnection();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::
AddCheckedContoursToSelectedMesh(std::list<unsigned int> iListCheckedContours)
{
  this->OpenDBConnection();
  this->AddCheckedTracesToCollection<QGoDBContourManager,QGoDBMeshManager>
    (this->m_ContoursManager,this->m_MeshesManager,
    ss_atoi<unsigned int>(this->m_SelectedCollectionData.first),iListCheckedContours);
  this->CloseDBConnection();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::
AddCheckedMeshesToSelectedTrack(std::list<unsigned int> iListCheckedMeshes)
{
  this->OpenDBConnection();
  this->AddCheckedTracesToCollection<QGoDBMeshManager,QGoDBTrackManager>
    (this->m_MeshesManager,this->m_TracksManager,
    ss_atoi<unsigned int>(this->m_SelectedCollectionData.first),iListCheckedMeshes);
  this->CloseDBConnection();
}