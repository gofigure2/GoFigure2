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
QGoPrintDatabase::QGoPrintDatabase(QWidget *iParent):
  QWidget(iParent),
  m_ContoursManager(NULL),
  m_MeshesManager(NULL),
  m_TracksManager(NULL),
  m_DatabaseConnector(NULL),
  m_SelectedCollectionData(NULL),
  m_SelectedColorData(NULL),
  m_IsDatabaseUsed(false)
{
  this->setupUi(this);
  DBTabWidget->setTabShape(QTabWidget::Triangular);
  DBTabWidget->removeTab(0);

  this->setContextMenuPolicy(Qt::CustomContextMenu);

  m_VisibilityAction = new QAction(tr("Show/hide the table widget"), this);
  QIcon TableWidgetIcon;
  TableWidgetIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/TableWidget.png") ),
                            QIcon::Normal, QIcon::Off);
  m_VisibilityAction->setIcon(TableWidgetIcon);
  m_VisibilityAction->setCheckable(true);

  this->m_TraceManualEditingDockWidget =
    new QGoTraceManualEditingDockWidget(this);

  this->m_TraceWidget =
    this->m_TraceManualEditingDockWidget->m_TraceWidget;
  this->SetPointerCollectionData (this->m_TraceWidget->GetPointerCollectionData());
  this->SetPointerSelectedColorData (this->m_TraceWidget->GetPointerColorData());

  this->m_CellTypeManager = new QGoDBCellTypeManager(this);

  this->m_SubCellTypeManager = new QGoDBSubCellTypeManager(this);

  this->m_ColorManager = new QGoDBColorManager(this);

  this->CreateConnectionsForTraceManualEditingWidget();
  this->m_ReeditMode = false;
  this->m_MeshGenerationMode = false;

  QObject::connect( m_VisibilityAction, SIGNAL( toggled(bool) ),
                    this, SLOT( setVisible(bool) ) );

  QObject::connect( this, SIGNAL( customContextMenuRequested(const QPoint &) ),
                    this, SLOT( CreateContextMenu(const QPoint &) ) );
  QObject::connect( this->DBTabWidget, SIGNAL( currentChanged(int) ),
                    this, SLOT( TheTabIsChanged(int) ) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoPrintDatabase::~QGoPrintDatabase()
{
  // Need to release memory allocated for contours, meshes, tracks and lineages
  // contours
  if ( m_ContoursManager )
    {
    delete m_ContoursManager;
    }
  // meshes
  if ( m_MeshesManager )
    {
    delete m_MeshesManager;
    }
  // tracks
  if ( m_TracksManager )
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
QAction *
QGoPrintDatabase::toggleViewAction()
{
  return m_VisibilityAction;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetDatabaseVariables(
  const std::string & iNameDB, const std::string & iServer,
  const std::string & iUser, const std::string & iPassword,
  const unsigned int & iImgSessionID, const std::string & iImgSessionName)
{
  m_Server         = iServer;
  m_User           = iUser;
  m_Password       = iPassword;
  m_DBName         = iNameDB;
  m_ImgSessionID   = iImgSessionID;
  m_ImgSessionName = iImgSessionName;

  this->SetTracesManager();
  this->m_BookmarkManager = new QGoDBBookmarkManager(this, this->m_ImgSessionID);
  QObject::connect( this->m_BookmarkManager, SIGNAL( ListBookmarksChanged() ),
                    this, SIGNAL( OpenBookmarksToUpdate() ) );

  this->InitializeTheComboboxesNotTraceRelated();
  this->SetTMListCollectionID();
  emit DBVariablesSet();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::OpenDBConnection()
{
  if ( m_DatabaseConnector == 0 )
    {
    this->m_DatabaseConnector = OpenDatabaseConnection(m_Server, m_User,
                                                       m_Password, m_DBName);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CloseDBConnection()
{
  if ( CloseDatabaseConnection(m_DatabaseConnector) )
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

  QString title = QString("Imaging Session: %1 ").arg( m_ImgSessionName.c_str() );
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
void QGoPrintDatabase::closeEvent(QCloseEvent *iEvent)
{
  (void)iEvent;
  m_VisibilityAction->setChecked(false);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string QGoPrintDatabase::InWhichTableAreWe()
{
  int CurrentIndex = this->DBTabWidget->currentIndex();

  return this->DBTabWidget->tabText(CurrentIndex).toStdString();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoPrintDatabase::AddContoursFromDBForAGivenTimePoint(std::vector< int > iListIDs)
{
  this->OpenDBConnection();
  std::list< unsigned int > ListIDs( iListIDs.begin(), iListIDs.end() );
  this->m_ContoursManager->UpdateTWAndContainerForExistingTraces(
    this->m_DatabaseConnector, ListIDs);

  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddMeshesFromDBForAGivenTimePoint(std::vector< int > iListIDs)
{
  this->OpenDBConnection();
  std::list< unsigned int > ListIDs( iListIDs.begin(), iListIDs.end() );
  this->m_MeshesManager->UpdateTWAndContainerForExistingTraces(
    this->m_DatabaseConnector, ListIDs);

  this->CloseDBConnection();
}
*/
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
                                           vtkPolyData *iContourNodes)
{
  OpenDBConnection();
  unsigned int ContourID;
  if ( !this->m_ReeditMode )
    {
    unsigned int MeshID = 0;
    std::string  test = this->m_SelectedCollectionData->first;
    if ( this->m_SelectedCollectionData->first != "Add a new mesh ..." )
      {
      MeshID = ss_atoi< unsigned int >(this->m_SelectedCollectionData->first);
      }
    ContourID = this->m_ContoursManager->SaveNewContourFromVisu(iXCoordMin,
                                                                iYCoordMin,
                                                                iZCoordMin,
                                                                iTCoord,
                                                                iXCoordMax,
                                                                iYCoordMax,
                                                                iZCoordMax,
                                                                iContourNodes,
                                                                this->m_DatabaseConnector,
                                                                *this->m_SelectedColorData,
                                                                MeshID);
    }
  else
    {
    ContourID =  this->m_ContoursManager->SaveReeditedContourFromVisu(
      iXCoordMin, iYCoordMin, iZCoordMin, iTCoord, iXCoordMax, iYCoordMax, iZCoordMax,
      iContourNodes, this->m_DatabaseConnector);
    this->m_ReeditMode = false;
    this->m_TraceManualEditingDockWidget->setEnabled(true);
    }

  std::list< unsigned int > ListContours;
  ListContours.push_back(ContourID);
  std::list< unsigned int > ListMeshes =
    this->m_ContoursManager->GetListCollectionIDs(this->m_DatabaseConnector, ListContours);
  if ( !ListMeshes.empty() )
    {
    this->m_MeshesManager->UpdateBoundingBoxes(this->m_DatabaseConnector, ListMeshes);
    }
  CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::SaveMeshFromVisuInDB(unsigned int iXCoordMin,
                                       unsigned int iYCoordMin,
                                       unsigned int iZCoordMin,
                                       unsigned int iXCoordMax,
                                       unsigned int iYCoordMax,
                                       unsigned int iZCoordMax,
                                       vtkPolyData *iMeshNodes,
                                       GoFigureMeshAttributes *iMeshAttributes)
{
  OpenDBConnection();
  if ( !this->m_MeshGenerationMode )
    {
    unsigned int NewMeshID = this->m_MeshesManager->SaveNewMeshFromVisu(iXCoordMin,
                                                                        iYCoordMin,
                                                                        iZCoordMin,
                                                                        this->m_SelectedTimePoint,
                                                                        iXCoordMax,
                                                                        iYCoordMax,
                                                                        iZCoordMax,
                                                                        iMeshNodes,
                                                                        this->m_DatabaseConnector,
                                                                        *this->m_SelectedColorData,
                                                                        ss_atoi< unsigned int >(this->
                                                                        m_SelectedCollectionData->first),
                                                                        iMeshAttributes);
                                                                        //this->m_SelectedCellType,
                                                                        //this->m_SelectedSubCellType);
    std::list< unsigned int > ListNewMeshes;
    ListNewMeshes.push_back(NewMeshID);
    this->m_TracksManager->UpdateBoundingBoxes( this->m_DatabaseConnector,
                                                this->m_MeshesManager->GetListCollectionIDs(this->m_DatabaseConnector,
                                                                                            ListNewMeshes) );
    //here update the CurrentElement for trackContainer with the data from the database corresponding to the selected trackID:
    this->m_TracksManager->UpdateCurrentElementTrackContainer(atoi(this->m_SelectedCollectionData->first.c_str()));
    }
  else //for mesh generated from contours:
    {
    this->m_MeshesManager->SaveGeneratedMeshFromVisu(iXCoordMin,
                                                     iYCoordMin,
                                                     iZCoordMin,
                                                     this->m_SelectedTimePoint,
                                                     iXCoordMax,
                                                     iYCoordMax,
                                                     iZCoordMax,
                                                     iMeshNodes,
                                                     this->m_DatabaseConnector,
                                                     iMeshAttributes);
    //as the mesh is generated from contours, there is no TrackID associated.
    this->m_MeshGenerationMode = false;
    }
  CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SaveNewMeshForMeshToContours(int iNumberOfContours)
{
  this->OpenDBConnection();
  unsigned int MeshID = this->m_MeshesManager->CreateNewMeshWithNoContourNoPoints(
    this->m_DatabaseConnector, *this->m_SelectedColorData, this->m_SelectedTimePoint,
    //this->m_SelectedCellType, this->m_SelectedSubCellType,
    ss_atoi< unsigned int >(this->m_SelectedCollectionData->first) );
  std::list< unsigned int > ListLastCreatedContours =
    this->m_ContoursManager->GetLastCreatedTracesIDs(this->m_DatabaseConnector, iNumberOfContours);
  this->AddCheckedTracesToCollection< QGoDBContourManager, QGoDBMeshManager >(
    this->m_ContoursManager, this->m_MeshesManager, MeshID, ListLastCreatedContours);
  //need to update the trackid ?? points ??
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int QGoPrintDatabase::SaveNewContourForMeshToContours(
  unsigned int iXCoordMin,
  unsigned int iYCoordMin,
  unsigned int iZCoordMin,
  unsigned int iXCoordMax,
  unsigned int iYCoordMax,
  unsigned int iZCoordMax,
  vtkPolyData *iTraceNodes)
{
  this->OpenDBConnection();
  unsigned int ContourID = this->m_ContoursManager->SaveNewContourFromVisu(
    iXCoordMin, iYCoordMin, iZCoordMin,
    this->m_SelectedTimePoint, iXCoordMax, iYCoordMax, iZCoordMax, iTraceNodes,
    this->m_DatabaseConnector, *this->m_SelectedColorData, 0);
  this->CloseDBConnection();
  return ContourID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoPrintDatabase::SaveTrackCurrentElement()
{
  this->OpenDBConnection();
  this->m_TracksManager->SaveTrackCurrentElement(this->m_DatabaseConnector);
  this->CloseDBConnection();
}

std::vector< ContourMeshStructure > QGoPrintDatabase::GetTracesForAGivenTimepoint(
  ContourMeshStructureMultiIndexContainer iAllTraces,
  unsigned int iTimePoint)
{
  std::vector< ContourMeshStructure >               SelectedTraces;
  ContourMeshStructureMultiIndexContainer::iterator iter = iAllTraces.begin();

  while ( iter != iAllTraces.end() )
    {
    if ( iter->TCoord == iTimePoint )
      {
      SelectedTraces.push_back(*iter);
      }
    ++iter;
    }
  return SelectedTraces;
}
*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*std::list< unsigned int > QGoPrintDatabase::GetContoursForAGivenZCoord(unsigned int iZCoord)
{
  this->OpenDBConnection();
  std::list< unsigned int > ContourIDs = this->m_ContoursManager->GetAllTracesIDForAGivenZCoord(
    this->m_DatabaseConnector, iZCoord);
  this->CloseDBConnection();
  return ContourIDs;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > QGoPrintDatabase::GetMeshesForAGivenZCoord(unsigned int iZCoord)
{
  this->OpenDBConnection();
  std::list< unsigned int > MeshesIDs = this->m_MeshesManager->GetAllTracesIDForAGivenZCoord(
    this->m_DatabaseConnector, iZCoord);
  this->CloseDBConnection();
  return MeshesIDs;
}*/

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoPrintDatabase::IsDatabaseUsed()
{
  return m_IsDatabaseUsed;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< QGoPrintDatabase::ItemColorComboboxData >
QGoPrintDatabase::GetListCollectionIDFromDB(vtkMySQLDatabase *iDatabaseConnector)
{
  std::list< ItemColorComboboxData > EmptyList = std::list< ItemColorComboboxData >();
  std::string                        TraceName = this->m_TraceWidget->GetTraceName();
  if ( TraceName == "contour" )
    {
    return this->m_MeshesManager->GetAllTraceIDsWithColor(iDatabaseConnector,
                                                          this->m_SelectedTimePoint);
    }
  if ( TraceName == "mesh" )
    {
    return this->m_TracksManager->GetAllTraceIDsWithColor(iDatabaseConnector);
    }
  return EmptyList;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SaveNewCollectionFromTraceWidgetInDBAndTW()
{
  this->OpenDBConnection();
  std::string  TraceName = this->m_TraceWidget->GetTraceName();
  unsigned int NewCollectionID = 0;
  if ( TraceName != "contour" && TraceName != "mesh" )
    {
    return;
    }
  else
    {
    if ( TraceName == "contour" )
      {
      NewCollectionID = this->m_MeshesManager->CreateNewMeshWithNoContourNoPoints(
        this->m_DatabaseConnector,
        *this->m_SelectedColorData, this->m_SelectedTimePoint);//, this->m_SelectedCellType,
        //this->m_SelectedSubCellType);
      }
    if ( TraceName == "mesh" )
      {
      NewCollectionID = this->m_TracksManager->CreateNewTrackWithNoMesh(
        this->m_DatabaseConnector, *this->m_SelectedColorData);
      }
    ItemColorComboboxData NewCollectionData;
    NewCollectionData.first = ConvertToString< unsigned int >(NewCollectionID);
    NewCollectionData.second = this->m_SelectedColorData->second;
    this->m_TraceWidget->AddANewCollectionID(NewCollectionData);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::pair< std::string, QColor > QGoPrintDatabase::GetSelectedCollectionData()
{
  return *this->m_SelectedCollectionData;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddBookmark(int iXCoord, int iYCoord,
                                   int iZCoord, int iTCoord)
{
  GoDBCoordinateRow BookmarkCoord;

  BookmarkCoord.SetField< int >("XCoord", iXCoord);
  BookmarkCoord.SetField< int >("YCoord", iYCoord);
  BookmarkCoord.SetField< int >("ZCoord", iZCoord);
  BookmarkCoord.SetField< int >("TCoord", iTCoord);

  this->OpenDBConnection();
  int BookmarkCoordID = BookmarkCoord.SaveInDB(this->m_DatabaseConnector);
  this->m_BookmarkManager->AddABookmark(BookmarkCoordID, this->m_DatabaseConnector);
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoPrintDatabase::NamesDescrContainerType QGoPrintDatabase::GetListBookmarks()
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
  if ( !this->m_Server.empty() ) //if empty, the database variables are not
  //been set up yet.
    {
    if ( this->m_TraceWidget->GetTraceName() == "contour" )
      {
      //if we change the timepoint, the list of meshes will be different from
      // the
      //previous one, so, initialize the list is needed (no pre-selected
      // collection)
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
  switch ( iIndex )
    {
    case 1:
      this->UpdateWidgetsForCorrespondingTrace("mesh", "track", false);
      break;
    case 2:
      this->UpdateWidgetsForCorrespondingTrace("track", "lineage", false);
      break;
    case 3:
      this->UpdateWidgetsForCorrespondingTrace("lineage", "None", false);
      break;
    default:
      this->UpdateWidgetsForCorrespondingTrace("contour", "mesh", false);
      break;
    }
  emit TableWidgetTabChanged();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTable(std::string iTablename)
{
  int Index = 0;

  if ( iTablename == "mesh" )
    {
    Index = 1;
    }
  if ( iTablename == "track" )
    {
    Index = 2;
    }
  if ( iTablename == "lineage" )
    {
    Index = 3;
    }
  this->DBTabWidget->setCurrentIndex(Index);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::ExportContours()
{
  QString p = QFileDialog::getSaveFileName( this,
                                            tr("Save Contour Export File"), "", tr("TextFile (*.txt)") );

  if ( !p.isNull() )
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
  QString p = QFileDialog::getSaveFileName( this,
                                            tr("Save Contour Export File"), "", tr("TextFile (*.txt)") );

  if ( !p.isNull() )
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
void QGoPrintDatabase::ImportContours()
{
  QString p = QFileDialog::getOpenFileName( this,
                                            tr("Open Contour Export File"), "",
                                            tr("TextFile (*.txt)") );

  //refactoring
  if ( !p.isNull() )
    {
    QFileInfo   pathInfo(p);
    std::string filename = p.toStdString();
    //import into the database:
    GoDBImport ImportHelper(this->m_Server, this->m_User,
                            this->m_Password, this->m_ImgSessionID, filename,
                            this->m_SelectedTimePoint);
    ImportHelper.ImportContours();

    std::vector< int > NewContourIDs = ImportHelper.GetVectorNewContourIDs();
    std::vector< int > NewMeshIDs = ImportHelper.GetVectorNewMeshIDs();
    std::vector< int > NewTrackIDs = ImportHelper.GetVectorNewTracksIDs();

    this->OpenDBConnection();
    this->m_ContoursManager->UpdateTWAndContainerForImportedTraces(NewContourIDs,
                                                                   this->m_DatabaseConnector);
    this->m_MeshesManager->UpdateTWAndContainerForImportedTraces(NewMeshIDs,
                                                                 this->m_DatabaseConnector);
    this->m_TracksManager->UpdateTWAndContainerForImportedTraces(NewTrackIDs,
                                                                 this->m_DatabaseConnector);
    this->CloseDBConnection();
    //as in the import contours file, there are data such as colors,celltype
    //and subcelltype, the lists may have been updated in the database:
    this->InitializeTheComboboxesNotTraceRelated();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::ImportMeshes()
{
  QString p = QFileDialog::getOpenFileName( this,
                                            tr("Open Contour Export File"), "",
                                            tr("TextFile (*.txt)") );

  //refactoring

  if ( !p.isNull() )
    {
    QFileInfo   pathInfo(p);
    std::string filename = p.toStdString();
    //import into the database:
    GoDBImport ImportHelper(this->m_Server, this->m_User,
                            this->m_Password, this->m_ImgSessionID, filename,
                            this->m_SelectedTimePoint);
    ImportHelper.ImportMeshes();

    std::vector< int > NewMeshIDs = ImportHelper.GetVectorNewMeshIDs();
    std::vector< int > NewTrackIDs = ImportHelper.GetVectorNewTracksIDs();

    this->OpenDBConnection();
    this->m_MeshesManager->UpdateTWAndContainerForImportedTraces(NewMeshIDs,
                                                                 this->m_DatabaseConnector);
    this->m_TracksManager->UpdateTWAndContainerForImportedTraces(NewTrackIDs,
                                                                 this->m_DatabaseConnector);
    this->CloseDBConnection();
    this->InitializeTheComboboxesNotTraceRelated();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//******related to TraceManualEditingDockWidget:****************************
QGoTraceManualEditingDockWidget * QGoPrintDatabase::GetTraceManualEditingDockWidget()
{
  return this->m_TraceManualEditingDockWidget;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::CreateConnectionsForTraceManualEditingWidget()
{
  //QObject::connect( this->m_TraceWidget,
    //                SIGNAL( NewSelectedColorActivated(ItemColorComboboxData) ),
      //              this,
                    //SLOT( UpdateSelectedColorData(ItemColorComboboxData) ) );

  //QObject::connect( this->m_TraceWidget,
    //                SIGNAL( NewCollectionActivated(ItemColorComboboxData) ),
      //              this,
        //            SLOT( UpdateSelectedCollectionID(ItemColorComboboxData) ) );

  QObject::connect( this->m_TraceWidget,
                    SIGNAL( AddNewColor() ),
                    this,
                    SLOT( AddNewColor() ) );

  QObject::connect( this->m_TraceWidget,
                    SIGNAL( DeleteColor() ),
                    this,
                    SLOT( DeleteColor() ) );

  QObject::connect( this->m_TraceWidget,
                    SIGNAL( NewCollectionToBeCreated() ),
                    this,
                    SLOT( SaveNewCollectionFromTraceWidgetInDBAndTW() ) );

  QObject::connect( this->m_TraceWidget,
                    SIGNAL( AddANewCellType() ),
                    this,
                    SLOT( AddNewCellType() ) );
  QObject::connect( this->m_TraceWidget,
                    SIGNAL( AddANewSubCellType() ),
                    this,
                    SLOT( AddNewSubCellType() ) );

  QObject::connect( this->m_TraceWidget,
                    SIGNAL( DeleteCellType() ),
                    this,
                    SLOT( DeleteCellType() ) );

  QObject::connect( this->m_TraceWidget,
                    SIGNAL( DeleteSubCellType() ),
                    this,
                    SLOT( DeleteSubCellType() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateWidgetsForCorrespondingTrace(std::string iTraceName,
                                                          std::string iCollectionName, bool UpdateTableWidget)
{
  if ( UpdateTableWidget )
    {
    //if the TableWidget has to be set to match the trace name, no need for the
    //signal TabHasChanged to be emitted, it would results in the Segmentation
    //widgets to be hidden
    this->blockSignals(true);
    this->SetTable(iTraceName);
    this->blockSignals(false);
    }
  this->m_TraceWidget->UpdateTraceAndCollection(iTraceName, iCollectionName);
  this->SetTMListCollectionID();
  // show the updated widget
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
void QGoPrintDatabase::SetTMListColors(std::string iNewColorToSelect)
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListColors(
    this->m_ColorManager->GetListExistingColors(this->m_DatabaseConnector),
    iNewColorToSelect);
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTMListColorsWithPreviousSelectedOne()
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListColorsWithSelectedOne(
    this->m_ColorManager->GetListExistingColors(this->m_DatabaseConnector) );
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTMListCollectionID(std::string iIDToSelect)
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
void QGoPrintDatabase::SetTMListCellTypesWithPreviousSelectedOne()
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListCellTypeWithSelectedOne(
    this->m_CellTypeManager->GetListExistingEntities(this->m_DatabaseConnector));
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
void QGoPrintDatabase::SetTMListSubCellTypesWithPreviousSelectedOne()
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListSubCellTypeWithSelectedOne(
    this->m_CellTypeManager->GetListExistingEntities(this->m_DatabaseConnector));
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoPrintDatabase::UpdateSelectedColorData(
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
*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddNewCellType()
{
  this->OpenDBConnection();
  std::string NewCellType = this->m_CellTypeManager->AddAnEntity(
    this->m_DatabaseConnector);
  if ( !NewCellType.empty() )
    {
    //this->m_MeshesManager->UpdateSelectedCellType(NewCellType);
    this->SetTMListCellTypes(NewCellType);
    }
  else //if the NewCellType is empty, go to the last selected one:
    {
    //this->m_TraceWidget->SetCurrentCellType(this->m_SelectedCellType);
    this->m_TraceWidget->SetCurrentCellTypeToSelectedOne();
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
  if ( !NewSubCellType.empty() )
    {
    //this->m_MeshesManager->UpdateSelectedSubCellType(NewSubCellType);
    this->SetTMListSubCellTypes(NewSubCellType);
    }
  else //if the NewSubCellType is empty, go to the last selected one:
    {
    //this->m_TraceWidget->SetCurrentSubCellType(this->m_SelectedSubCellType);
    this->m_TraceWidget->SetCurrentSubCellTypeToSelectedOne();
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
  if ( !NewColor.first.empty() )
    {
    //this->UpdateSelectedColorData(NewColor);
    this->SetTMListColors(NewColor.first);
    }
  else //if the NewColor name is empty, go to the last selected one:
    {
    //this->m_TraceWidget->SetCurrentColor(this->m_SelectedColorData->first);
    this->m_TraceWidget->SetCurrentColorToSelectedOne();
    }
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteCellType()
{
  this->OpenDBConnection();
  if ( this->m_CellTypeManager->DeleteEntity(this->m_DatabaseConnector) )
    {
    this->SetTMListCellTypesWithPreviousSelectedOne();
    }
  else //if the user cancelled, go to the last selected one:
    {
    //this->m_TraceWidget->SetCurrentCellType(this->m_SelectedCellType);
    this->m_TraceWidget->SetCurrentCellTypeToSelectedOne();
    }
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteColor()
{
  this->OpenDBConnection();
  if ( this->m_ColorManager->DeleteEntity(this->m_DatabaseConnector) )
    {
    //this->SetTMListColors(this->m_SelectedColorData->first);
    this->SetTMListColorsWithPreviousSelectedOne();
    }
  else //if the user cancelled, go to the last selected one:
    {
    //this->m_TraceWidget->SetCurrentColor(this->m_SelectedColorData->first);
    this->m_TraceWidget->SetCurrentColorToSelectedOne();
    }
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteSubCellType()
{
  this->OpenDBConnection();
  if ( this->m_SubCellTypeManager->DeleteEntity(this->m_DatabaseConnector) )
    {
    //this->SetTMListSubCellTypes(this->m_SelectedSubCellType);
    this->SetTMListSubCellTypesWithPreviousSelectedOne();
    }
  else //if the user cancelled, go to the last selected one:
    {
    //this->m_TraceWidget->SetCurrentSubCellType(this->m_SelectedSubCellType);
    this->m_TraceWidget->SetCurrentSubCellTypeToSelectedOne();
    }
  this->CloseDBConnection();
}

//*********************RELATED TO QGODBTRACEMANAGER**************************

void QGoPrintDatabase::GetContentAndDisplayAllTracesInfo(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->m_ContoursManager->DisplayInfoAndLoadVisuContainerForAllContours(
    iDatabaseConnector,this->m_SelectedTimePoint);
  this->m_MeshesManager->DisplayInfoAndLoadVisuContainerForAllMeshes(
    iDatabaseConnector,this->m_SelectedTimePoint);
  this->m_TracksManager->DisplayInfoAndLoadVisuContainerForAllTracks(
    iDatabaseConnector);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::CreateContextMenu(const QPoint & iPos)
{
  std::string TraceName = this->m_TraceWidget->GetTraceName();

  if ( TraceName == "contour" )
    {
    this->m_ContoursManager->CreateContextMenu(iPos);
    }
  if ( TraceName == "mesh" )
    {
    this->m_MeshesManager->CreateContextMenu(iPos);
    }
  if ( TraceName == "track" )
    {
    this->m_TracksManager->CreateContextMenu(iPos);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteContours()
{
  this->DeleteTraces< QGoDBContourManager, QGoDBMeshManager, QGoDBContourManager >(
    this->m_ContoursManager, this->m_MeshesManager, this->m_ContoursManager);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteMeshes()
{
  this->DeleteTraces< QGoDBMeshManager, QGoDBTrackManager, QGoDBContourManager >(
    this->m_MeshesManager, this->m_TracksManager, this->m_ContoursManager);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTracks()
{
  this->DeleteTraces< QGoDBTrackManager, QGoDBMeshManager, QGoDBMeshManager >(
    this->m_TracksManager, this->m_MeshesManager, this->m_MeshesManager, true);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::ChangeMeshColor()
{
  this->ChangeTraceColor< QGoDBMeshManager, QGoDBContourManager >(
    this->m_MeshesManager, this->m_ContoursManager);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::ChangeTrackColor()
{
  this->ChangeTraceColor< QGoDBTrackManager, QGoDBMeshManager >(
    this->m_TracksManager, this->m_MeshesManager);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::PrintVolumeAreaForMesh(GoFigureMeshAttributes *
                                              iMeshAttributes, unsigned int iMeshID)
{
  this->m_MeshesManager->DisplayOnlyVolumeAreaForExistingMesh(
    iMeshAttributes, iMeshID);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoPrintDatabase::SetContoursContainer(ContourMeshContainer *iContainer)
{
  this->m_ContoursManager->SetContoursInfoContainerForVisu(iContainer);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoPrintDatabase::SetMeshesContainer(ContourMeshContainer *iContainer)
{
  this->m_MeshesManager->SetMeshesInfoContainerForVisu(iContainer);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoPrintDatabase::SetTracksContainer(TrackContainer *iContainer)
{
  this->m_TracksManager->SetTracksInfoContainerForVisu(iContainer);
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
  this->m_ContoursManager = new QGoDBContourManager(m_ImgSessionID, this);
  this->m_ContoursManager->SetSelectedCollection (
    this->m_TraceWidget->GetPointerCollectionData());

  QObject::connect( this->m_ContoursManager, SIGNAL( NeedToGetDatabaseConnection() ),
                    this, SLOT( PassDBConnectionToContoursManager() ) );
  QObject::connect( this->m_ContoursManager, SIGNAL( NeedToGetCurrentSelectedColor() ),
                    this, SLOT( PassSelectedColorToContoursManager() ) );
  QObject::connect( this->m_ContoursManager, SIGNAL( TracesToDelete() ),
                    this, SLOT( DeleteContours() ) );
  QObject::connect( this->m_ContoursManager,
                    SIGNAL( NeedToGoToTheLocation(int, int, int, int) ),
                    this, SIGNAL( NeedToGoToTheLocation(int, int, int, int) ) );
  QObject::connect( this->m_ContoursManager,
                    SIGNAL( TraceToReEdit(unsigned int) ),
                    this,
                    SLOT( ReEditTrace(unsigned int) ) );
  QObject::connect( this->m_ContoursManager,
                    SIGNAL( NewCollectionFromCheckedTraces(std::list< unsigned int > ) ),
                    this,
                    SLOT( CreateNewMeshFromCheckedContours(std::list< unsigned int > ) ) );
  QObject::connect( this->m_ContoursManager,
                    SIGNAL( CheckedTracesToAddToSelectedCollection(
                              std::list< unsigned int > ) ), this,
                    SLOT( AddCheckedContoursToSelectedMesh(std::list< unsigned int > ) ) );
  QObject::connect( this->m_ContoursManager,
                    SIGNAL(DBConnectionNotNeededAnymore() ),
                    this,
                    SLOT(CloseDBConnection() ) );

  //QObject::connect( this->m_TraceWidget, 
   //                 SIGNAL( NewCollectionActivated(ItemColorComboboxData) ),
    //                this->m_ContoursManager, 
     //               SLOT(SetSelectedCollection(ItemColorComboboxData) ) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetMeshesManager()
{
  this->m_MeshesManager = new QGoDBMeshManager(m_ImgSessionID, this);
 
  QObject::connect( this->m_MeshesManager, SIGNAL( NeedToGetDatabaseConnection() ),
                    this, SLOT( PassDBConnectionToMeshesManager() ) );
  QObject::connect( this->m_MeshesManager,
                    SIGNAL( TraceColorToChange() ),
                    this, SLOT( ChangeMeshColor() ) );
  QObject::connect( this->m_MeshesManager, SIGNAL( TracesToDelete() ),
                    this, SLOT( DeleteMeshes() ) );
  QObject::connect( this->m_MeshesManager,
                    SIGNAL( NeedToGoToTheLocation(int, int, int, int) ),
                    this, SIGNAL( NeedToGoToTheLocation(int, int, int, int) ) );
  QObject::connect( this->m_MeshesManager,
                    SIGNAL ( NewCollectionFromCheckedTraces(std::list< unsigned int > ) ),
                    this,
                    SLOT( CreateNewTrackFromCheckedMeshes(std::list< unsigned int > ) ) );
  QObject::connect( this->m_MeshesManager,
                    SIGNAL( CheckedTracesToAddToSelectedCollection(
                              std::list< unsigned int > ) ), this,
                    SLOT( AddCheckedMeshesToSelectedTrack(std::list< unsigned int > ) ) );
  QObject::connect( this->m_MeshesManager,
                    SIGNAL(DBConnectionNotNeededAnymore() ),
                    this,
                    SLOT(CloseDBConnection() ) );

  //related to traceEditingWidget and meshes_manager (celltype + subcelltype + collectionData):
  this->m_MeshesManager->SetSelectedCollection (
    this->m_TraceWidget->GetPointerCollectionData());
  this->m_MeshesManager->SetSelectedCellType(
    this->m_TraceWidget->GetPointerSelectedCellType());
  this->m_MeshesManager->SetSelectedSubCellType(
    this->m_TraceWidget->GetPointerSelectedSubCellType());

  //QObject::connect( this->m_TraceWidget,
  //                  SIGNAL( NewCellTypeActivated(std::string) ),
   //                 this->m_MeshesManager,
   //                 SLOT( UpdateSelectedCellType(std::string) ) );
  //QObject::connect( this->m_TraceWidget,
  //                  SIGNAL( NewSubCellTypeActivated(std::string) ),
  //                  this->m_MeshesManager,
  //                  SLOT( UpdateSelectedSubCellType(std::string) ) );
  //QObject::connect( this->m_TraceWidget, 
    //                SIGNAL( NewCollectionActivated(ItemColorComboboxData) ),
      //              this->m_MeshesManager, 
        //            SLOT(SetSelectedCollection(ItemColorComboboxData) ) );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetTracksManager()
{
  this->m_TracksManager = new QGoDBTrackManager(m_ImgSessionID, this);
  QObject::connect( this->m_TracksManager,
                    SIGNAL( TraceColorToChange() ),
                    this, SLOT( ChangeTrackColor() ) );
  QObject::connect( this->m_TracksManager, SIGNAL( TracesToDelete() ),
                    this, SLOT( DeleteTracks() ) );

  QObject::connect( this->m_TracksManager,
                    SIGNAL(DBConnectionNotNeededAnymore() ),
                    this,
                    SLOT(CloseDBConnection() ) );

  QObject::connect( this->m_TracksManager, SIGNAL( NeedToGetDatabaseConnection() ),
                    this, SLOT( PassDBConnectionToTracksManager() ) );

  QObject::connect( this->m_TracksManager,
                    SIGNAL(DBConnectionNotNeededAnymore() ),
                    this,
                    SLOT(CloseDBConnection() ) );
  //QObject::connect( this->m_TraceWidget, 
    //                SIGNAL( NewCollectionActivated(ItemColorComboboxData) ),
      //              this->m_TracksManager, 
        //            SLOT(SetSelectedCollection(ItemColorComboboxData) ) );
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
void QGoPrintDatabase::PassDBConnectionToMeshesManager()
{
  this->OpenDBConnection();
  this->m_MeshesManager->SetDatabaseConnection(this->m_DatabaseConnector);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::PassDBConnectionToTracksManager()
{
  this->OpenDBConnection();
  this->m_TracksManager->SetDatabaseConnection(this->m_DatabaseConnector);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::PassSelectedColorToContoursManager()
{
  this->m_ContoursManager->SetSelectedColor(*this->m_SelectedColorData);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::ReEditTrace(unsigned int iTraceID)
{
  this->m_ReeditMode = true;
  this->m_TraceManualEditingDockWidget->setEnabled(false);
  emit TraceToReEdit(iTraceID);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateNewTrackFromCheckedMeshes(
  std::list< unsigned int > iListCheckedMeshes)
{
  this->OpenDBConnection();
  unsigned int NewTrackID = this->m_TracksManager->CreateNewTrackWithNoMesh(
    this->m_DatabaseConnector, *this->m_SelectedColorData);
  this->AddCheckedTracesToCollection< QGoDBMeshManager, QGoDBTrackManager >
    (this->m_MeshesManager, this->m_TracksManager,
    NewTrackID, iListCheckedMeshes);
  this->CloseDBConnection();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateNewMeshFromCheckedContours(
  std::list< unsigned int > iListCheckedContours)
{
  this->OpenDBConnection();
  unsigned int NewMeshID =
    this->m_MeshesManager->CreateNewMeshWithNoContourNoPoints(
      this->m_DatabaseConnector, *this->m_SelectedColorData,
      this->m_SelectedTimePoint);//, this->m_SelectedCellType,
      //this->m_SelectedSubCellType);

  this->AddCheckedTracesToCollection< QGoDBContourManager, QGoDBMeshManager >
    (this->m_ContoursManager, this->m_MeshesManager,
    NewMeshID, iListCheckedContours);

  this->m_MeshGenerationMode = true;
  emit NewMeshToGenerate(iListCheckedContours, NewMeshID);
  this->CloseDBConnection();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::AddCheckedContoursToSelectedMesh(std::list< unsigned int > iListCheckedContours)
{
  this->OpenDBConnection();
  this->AddCheckedTracesToCollection< QGoDBContourManager, QGoDBMeshManager >
    (this->m_ContoursManager, this->m_MeshesManager,
    ss_atoi< unsigned int >(this->m_SelectedCollectionData->first), iListCheckedContours);
  this->CloseDBConnection();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::AddCheckedMeshesToSelectedTrack(std::list< unsigned int > iListCheckedMeshes)
{
  this->OpenDBConnection();
  this->AddCheckedTracesToCollection< QGoDBMeshManager, QGoDBTrackManager >
    (this->m_MeshesManager, this->m_TracksManager,
    ss_atoi< unsigned int >(this->m_SelectedCollectionData->first), iListCheckedMeshes);

  this->CloseDBConnection();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetPointerCollectionData (ItemColorComboboxData* iCollectionData)
{
  this->m_SelectedCollectionData = iCollectionData;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetPointerSelectedColorData(ItemColorComboboxData* iColorData)
{
  this->m_SelectedColorData = iColorData;
}