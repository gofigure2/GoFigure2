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
#include <QStatusBar>

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
#include "QGoTrackEditingWidget.h"

//--------------------------------------------------------------------------
QGoPrintDatabase::QGoPrintDatabase(QWidget *iParent) :
  QWidget(iParent),
  m_ContoursManager(NULL),
  m_MeshesManager(NULL),
  m_TracksManager(NULL),
  m_LineagesManager(NULL),
  m_DatabaseConnector(NULL),
  m_IsDatabaseUsed(false),
  m_ReeditMode(false),
  m_MeshGenerationMode(false)
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

  this->m_TraceSettingsDockWidget =
    new QGoTraceSettingsDockWidget(this);

  this->m_TraceWidget =
    this->m_TraceSettingsDockWidget->m_TraceWidget;

  this->m_CellTypeManager = new QGoDBCellTypeManager(this);

  this->m_SubCellTypeManager = new QGoDBSubCellTypeManager(this);

  this->m_ColorManager = new QGoDBColorManager(this);
  this->m_SelectedTimePoint = new int;

  this->CreateConnectionsForTraceSettingsWidget();

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
  if ( m_SelectedTimePoint )
    {
    delete m_SelectedTimePoint;
    }
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
  if (m_LineagesManager)
    {
    delete m_LineagesManager;
    }
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
  this->SetTSListCollectionID();
  emit DBVariablesSet();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::OpenDBConnection()
{
  if ( m_DatabaseConnector == NULL )
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
    this->m_DatabaseConnector = NULL;
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
  this->DBTabWidget->addTab(this->m_LineagesManager->GetTableWidget(), "lineage");
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
    ContourID = this->m_ContoursManager->SaveNewContourFromVisu(iXCoordMin,
                                                                iYCoordMin,
                                                                iZCoordMin,
                                                                iXCoordMax,
                                                                iYCoordMax,
                                                                iZCoordMax,
                                                                iContourNodes,
                                                                this->m_DatabaseConnector,
                                                                1);
    }
  else
    {
    ContourID =  this->m_ContoursManager->SaveReeditedContourFromVisu(
        iXCoordMin, iYCoordMin, iZCoordMin, iTCoord, iXCoordMax, iYCoordMax, iZCoordMax,
        iContourNodes, this->m_DatabaseConnector);
    this->m_ReeditMode = false;
    this->m_TraceSettingsDockWidget->setEnabled(true);
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
                                       int iTShift,
                                       vtkPolyData *iMeshNodes,
                                       GoFigureMeshAttributes *iMeshAttributes)
{
  OpenDBConnection();
  if ( !this->m_MeshGenerationMode )
    {
    unsigned int TrackID = this->m_TraceWidget->GetCurrentSelectedCollectionID();
    //check that there isn't an existing mesh with the same timepoint in the
    // track,if so, set its trackID to 0:
    /** \todo print a different message if several meshes are created at the
      same timepoint*/
    QString MessageToPrint = this->m_MeshesManager->CheckExistingMeshesForTheTrack(TrackID,
                                                                                   this->m_DatabaseConnector, iTShift);
    if ( MessageToPrint != "" )
      {
      emit PrintMessage(MessageToPrint);
      }
    unsigned int NewMeshID = this->m_MeshesManager->SaveNewMeshFromVisu(iXCoordMin,
                                                                        iYCoordMin,
                                                                        iZCoordMin,
                                                                        iXCoordMax,
                                                                        iYCoordMax,
                                                                        iZCoordMax,
                                                                        iTShift,
                                                                        iMeshNodes,
                                                                        this->m_DatabaseConnector,
                                                                        iMeshAttributes);
    std::list< unsigned int > ListNewMeshes;
    ListNewMeshes.push_back(NewMeshID);
    //here update the CurrentElement for trackContainer with the data from the
    // database corresponding to the selected trackID:
    // this->m_TracksManager->UpdateCurrentElementTrackContainer(TrackID);
    //update the bounding box and the visu for the tracks:
    std::list< unsigned int > trackIDs = this->m_MeshesManager->GetListCollectionIDs(this->m_DatabaseConnector,
                                                                                     ListNewMeshes);
    this->m_TracksManager->UpdateBoundingBoxes(this->m_DatabaseConnector,
                                               trackIDs);
    }
  else //for mesh generated from contours:
    {
    this->m_MeshesManager->SaveGeneratedMeshFromVisu(iXCoordMin,
                                                     iYCoordMin,
                                                     iZCoordMin,
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
  unsigned int TrackID = this->m_TraceWidget->GetCurrentSelectedCollectionID();
  QString      MessageToPrint =  this->m_MeshesManager->CheckExistingMeshesForTheTrack(TrackID,
                                                                                       this->m_DatabaseConnector);
  if ( MessageToPrint != "" )
    {
    emit PrintMessage(MessageToPrint);
    }

  unsigned int MeshID = this->m_MeshesManager->CreateNewMeshWithNoContourNoPoints(
      this->m_DatabaseConnector);
  std::list< unsigned int > ListLastCreatedContours =
    this->m_ContoursManager->GetLastCreatedTracesIDs(this->m_DatabaseConnector, iNumberOfContours);
  this->AddCheckedTracesToCollection< QGoDBContourManager, QGoDBMeshManager >(
    this->m_ContoursManager, this->m_MeshesManager, MeshID, ListLastCreatedContours);

  std::list< unsigned int > ListNewMeshes;
  ListNewMeshes.push_back(MeshID);
  std::list< unsigned int > trackIDs = this->m_MeshesManager->GetListCollectionIDs(this->m_DatabaseConnector,
                                                                                   ListNewMeshes);
  //here update the CurrentElement for trackContainer with the data from the
  // database corresponding to the selected trackID:
  //update the bounding box and the visu for the tracks:
  this->m_TracksManager->UpdateBoundingBoxes(
    this->m_DatabaseConnector,
    trackIDs);
  this->SetTSListCollectionID(); // need to reload the list of trackIDs as
                                 // normally, the new added mesh from create
                                 // mesh with no
  //contour will add the new meshid in the TSList...
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
      iXCoordMax, iYCoordMax, iZCoordMax, iTraceNodes,
      this->m_DatabaseConnector, 0);
  this->CloseDBConnection();
  return ContourID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoPrintDatabase::IsDatabaseUsed()
{
  return m_IsDatabaseUsed;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< QGoPrintDatabase::ItemColorComboboxData >
QGoPrintDatabase::GetListCollectionIDFromDB(vtkMySQLDatabase *iDatabaseConnector,
                                            std::string & ioIDToSelect)
{
  std::list< ItemColorComboboxData > EmptyList = std::list< ItemColorComboboxData >();
  std::string                        TraceName = this->m_TraceWidget->GetTraceName();
  if ( TraceName == "contour" )
    {
    return this->m_MeshesManager->GetAllTraceIDsWithColor(iDatabaseConnector, ioIDToSelect);
    }
  if ( TraceName == "mesh" )
    {
    return this->m_TracksManager->GetAllTraceIDsWithColor(iDatabaseConnector, ioIDToSelect);
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
          this->m_DatabaseConnector);
      }
    if ( TraceName == "mesh" )
      {
      NewCollectionID = this->m_TracksManager->CreateNewTrackWithNoMesh(
          this->m_DatabaseConnector);
      }
    ItemColorComboboxData NewCollectionData;
    NewCollectionData.first = ConvertToString< unsigned int >(NewCollectionID);
    NewCollectionData.second = this->m_TraceWidget->GetPointerColorData()->second;
    }

  this->CloseDBConnection();
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
  *this->m_SelectedTimePoint = iTimePoint;
  if ( !this->m_Server.empty() ) //if empty, the database variables are not
  //been set up yet.
    {
    if ( this->m_TraceWidget->GetTraceName() == "contour" )
      {
      //if we change the timepoint, the list of meshes will be different from
      // the previous one, so, initialize the list is needed (no pre-selected
      // collection)
      this->SetTSListCollectionID();
      this->m_ContoursManager->CheckShowRows();
      }
    if ( this->m_TraceWidget->GetTraceName() == "mesh" )
      {
      this->m_MeshesManager->CheckShowRows();
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::TheTabIsChanged(int iIndex)
{
  this->m_TraceSettingsDockWidget->show();

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
void QGoPrintDatabase::UpdateSelectedCollectionForTableWidget(std::string iTableName)
{
  if ( iTableName == "contour" )
    {
    this->m_MeshesManager->UpdateLastSelectedOneAsCollection();
    }
  if ( iTableName == "mesh" )
    {
    this->m_TracksManager->UpdateLastSelectedOneAsCollection();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::ExportContours()
{
  QString p = QFileDialog::getSaveFileName( this,
                                            tr("Save Contour Export File"), "", tr("TextFile (*.txt)") );

  if ( !p.isNull() )
    {
    if ( !p.endsWith(".txt") )
      {
      p.append( QString(".txt") );
      }
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
    if ( !p.endsWith(".txt") )
      {
      p.append( QString(".txt") );
      }

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
                                            tr("Open Contours Export File"), "",
                                            tr("TextFile (*.txt)") );

  //refactoring
  if ( !p.isNull() )
    {
    emit        PrintMessage( tr("Warning: Close and reopen your imagingsession once the import is done !!") );
    QFileInfo   pathInfo(p);
    std::string filename = p.toStdString();
    //import into the database:
    GoDBImport ImportHelper(this->m_Server, this->m_User,
                            this->m_Password, this->m_ImgSessionID, filename,
                            *this->m_SelectedTimePoint);
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
                                            tr("Open Meshes Export File"), "",
                                            tr("TextFile (*.txt)") );

  //refactoring

  if ( !p.isNull() )
    {
    emit        PrintMessage( tr("Warning: Close and reopen your imagingsession once the import is done !!") );
    QFileInfo   pathInfo(p);
    std::string filename = p.toStdString();
    //import into the database:
    GoDBImport ImportHelper(this->m_Server, this->m_User,
                            this->m_Password, this->m_ImgSessionID, filename,
                            *this->m_SelectedTimePoint);
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
std::vector< int > QGoPrintDatabase::ImportTracks()
{
  QString p = QFileDialog::getOpenFileName( this,
                                            tr("Open Tracks Export File"), "",
                                            tr("TextFile (*.txt)") );

  std::vector< int > NewTrackIDs = std::vector< int >();

  if ( !p.isNull() )
    {
    emit        PrintMessage( tr("Warning: Close and reopen your imagingsession once the import is done !!") );
    QFileInfo   pathInfo(p);
    std::string filename = p.toStdString();
    //import into the database:
    GoDBImport ImportHelper(this->m_Server, this->m_User,
                            this->m_Password, this->m_ImgSessionID, filename,
                            *this->m_SelectedTimePoint);
    ImportHelper.ImportTracks();

    std::vector< int > NewMeshIDs = ImportHelper.GetVectorNewMeshIDs();
    NewTrackIDs = ImportHelper.GetVectorNewTracksIDs();

    this->OpenDBConnection();
    this->m_MeshesManager->UpdateTWAndContainerForImportedTraces(NewMeshIDs,
                                                                 this->m_DatabaseConnector);
    this->m_TracksManager->UpdateTWAndContainerForImportedTraces(NewTrackIDs,
                                                                 this->m_DatabaseConnector);
    this->CloseDBConnection();
    this->InitializeTheComboboxesNotTraceRelated();
    }
  return NewTrackIDs;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//******related to TraceManualEditingDockWidget:****************************
QGoTraceSettingsDockWidget * QGoPrintDatabase::GetTraceSettingsDockWidget()
{
  return this->m_TraceSettingsDockWidget;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::CreateConnectionsForTraceSettingsWidget()
{
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
  std::string PreviousTraceName = this->m_TraceWidget->GetTraceName();

  if ( UpdateTableWidget )
    {
    std::string CurrentTableName = this->InWhichTableAreWe();
    if ( CurrentTableName != iTraceName )
      {
      this->UpdateSelectedCollectionForTableWidget(PreviousTraceName);
      this->m_TraceWidget->UpdateTraceAndCollection(iTraceName, iCollectionName);
      this->SetTSListCollectionID();
      // show the updated widget
      this->m_TraceSettingsDockWidget->show();
      }
    //if the TableWidget has to be set to match the trace name, no need for the
    //signal TabHasChanged to be emitted, it would results in the Segmentation
    //widgets to be hidden
    this->blockSignals(true);
    this->SetTable(iTraceName);
    this->blockSignals(false);
    return;
    }
  this->UpdateSelectedCollectionForTableWidget(PreviousTraceName);
  this->m_TraceWidget->UpdateTraceAndCollection(iTraceName, iCollectionName);
  this->SetTSListCollectionID();
  // show the updated widget
  this->m_TraceSettingsDockWidget->show();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::InitializeTheComboboxesNotTraceRelated()
{
  this->SetTSListColors();
  this->SetTSListCellTypes();
  this->SetTSListSubCellTypes();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTSListColors(std::string iNewColorToSelect)
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListColors(
    this->m_ColorManager->GetListExistingColors(this->m_DatabaseConnector),
    iNewColorToSelect);
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTSListColorsWithPreviousSelectedOne()
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListColorsWithSelectedOne(
    this->m_ColorManager->GetListExistingColors(this->m_DatabaseConnector) );
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTSListCollectionID()
{
  this->OpenDBConnection();
  std::string                        IDToSelect;
  std::list< ItemColorComboboxData > ListCollectionID =
    this->GetListCollectionIDFromDB(this->m_DatabaseConnector, IDToSelect);
  this->m_TraceWidget->SetListCollectionID(ListCollectionID, IDToSelect);
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTSListCellTypes(std::string iCellTypeToSelect)
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListCellTypes(
    this->m_CellTypeManager->GetListExistingEntities(this->m_DatabaseConnector),
    iCellTypeToSelect);
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTSListCellTypesWithPreviousSelectedOne()
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListCellTypeWithSelectedOne(
    this->m_CellTypeManager->GetListExistingEntities(this->m_DatabaseConnector) );
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTSListSubCellTypes(std::string iSubCellTypeToSelect)
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListSubCellTypes(
    this->m_SubCellTypeManager->GetListExistingEntities(this->m_DatabaseConnector),
    iSubCellTypeToSelect);
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTSListSubCellTypesWithPreviousSelectedOne()
{
  this->OpenDBConnection();
  this->m_TraceWidget->SetListSubCellTypeWithSelectedOne(
    this->m_SubCellTypeManager->GetListExistingEntities(this->m_DatabaseConnector) );
  this->CloseDBConnection();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddNewCellType()
{
  this->OpenDBConnection();
  std::string NewCellType = this->m_CellTypeManager->AddAnEntity(
      this->m_DatabaseConnector);
  if ( !NewCellType.empty() )
    {
    this->SetTSListCellTypes(NewCellType);
    }
  else //if the NewCellType is empty, go to the last selected one:
    {
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
    this->SetTSListSubCellTypes(NewSubCellType);
    }
  else //if the NewSubCellType is empty, go to the last selected one:
    {
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
    //*this->m_SelectedColorData = NewColor;
    this->SetTSListColors(NewColor.first);
    }
  else //if the NewColor name is empty, go to the last selected one:
    {
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
    this->SetTSListCellTypesWithPreviousSelectedOne();
    }
  else //if the user cancelled, go to the last selected one:
    {
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
    this->SetTSListColorsWithPreviousSelectedOne();
    }
  else //if the user cancelled, go to the last selected one:
    {
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
    this->SetTSListSubCellTypesWithPreviousSelectedOne();
    }
  else //if the user cancelled, go to the last selected one:
    {
    this->m_TraceWidget->SetCurrentSubCellTypeToSelectedOne();
    }
  this->CloseDBConnection();
}

//*********************RELATED TO QGODBTRACEMANAGER**************************

void QGoPrintDatabase::GetContentAndDisplayAllTracesInfo(
  vtkMySQLDatabase *iDatabaseConnector)
{
  this->m_ContoursManager->DisplayInfoAndLoadVisuContainerForAllContours(
    iDatabaseConnector);
  this->m_MeshesManager->DisplayInfoAndLoadVisuContainerForAllMeshes(
    iDatabaseConnector);
  this->m_TracksManager->DisplayInfoAndLoadVisuContainerForAllTracks(
    iDatabaseConnector);
  this->m_LineagesManager->DisplayInfoAndLoadVisuContainerForAllLineages(
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
void QGoPrintDatabase::DeleteCheckedContours()
{
  this->DeleteCheckedTraces< QGoDBContourManager, QGoDBMeshManager, QGoDBContourManager >(
    this->m_ContoursManager, this->m_MeshesManager, this->m_ContoursManager);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteCheckedMeshes()
{
  this->DeleteCheckedTraces< QGoDBMeshManager, QGoDBTrackManager, QGoDBContourManager >(
    this->m_MeshesManager, this->m_TracksManager, this->m_ContoursManager);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteCheckedTracks()
{
  this->DeleteCheckedTraces< QGoDBTrackManager, QGoDBMeshManager, QGoDBMeshManager >(
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
void QGoPrintDatabase::PrintCalculatedValuesForTrack(GoFigureTrackAttributes *
                                                     iTrackAttributes, unsigned int iTrackID)
{
  this->m_TracksManager->DisplayOnlyCalculatedValuesForExistingTrack(
    iTrackAttributes, iTrackID);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoPrintDatabase::SetContoursContainer(ContourContainer *iContainer)
{
  this->m_ContoursManager->SetContoursInfoContainerForVisu(iContainer);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoPrintDatabase::SetMeshesContainer(MeshContainer *iContainer)
{
  this->m_MeshesManager->SetMeshesInfoContainerForVisu(iContainer);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoPrintDatabase::SetTracksContainer(TrackContainer *iContainer)
{
  this->m_TracksManager->SetTracksInfoContainerForVisu(iContainer);
  QObject::connect( this->m_TracksManager,
                    SIGNAL ( NeedMeshesInfoForImportedTrack(unsigned int) ),
                    this,
                    SLOT ( PassMeshesInfoForImportedTrack(unsigned int) ) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoPrintDatabase::SetLineagesContainer(LineageContainer *iContainer)
{
  this->m_LineagesManager->SetLineagesInfoContainerForVisu(iContainer);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetTracesManager()
{
  this->SetContoursManager();
  this->SetMeshesManager();
  this->SetTracksManager();
  this->SetLineagesManager();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetContoursManager()
{
  this->m_ContoursManager = new QGoDBContourManager(m_ImgSessionID, this);
  this->m_ContoursManager->SetSelectedCollection (
    this->m_TraceWidget->GetPointerCollectionData() );

  QObject::connect( this->m_ContoursManager, SIGNAL( NeedToGetDatabaseConnection() ),
                    this, SLOT( PassDBConnectionToContoursManager() ) );
  QObject::connect( this->m_ContoursManager, SIGNAL( CheckedTracesToDelete() ),
                    this, SLOT( DeleteCheckedContours() ) );
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
                    SIGNAL( DBConnectionNotNeededAnymore() ),
                    this,
                    SLOT( CloseDBConnection() ) );
  QObject::connect( this->m_ContoursManager,
                    SIGNAL( PrintMessage(QString, int) ),
                    this,
                    SIGNAL( PrintMessage(QString, int) ) );

  this->m_ContoursManager->SetSelectedCollection(
    this->m_TraceWidget->GetPointerCollectionData() );
  this->m_ContoursManager->SetCurrentTimePoint(this->m_SelectedTimePoint);
  this->m_ContoursManager->SetSelectedColor( this->m_TraceWidget->GetPointerColorData() );
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
  QObject::connect( this->m_MeshesManager, SIGNAL( CheckedTracesToDelete() ),
                    this, SLOT( DeleteCheckedMeshes() ) );
  QObject::connect( this->m_MeshesManager,
                    SIGNAL( NeedToGoToTheLocation(int, int, int, int) ),
                    this, SIGNAL( NeedToGoToTheLocation(int, int, int, int) ) );
  QObject::connect( this->m_MeshesManager,
                    SIGNAL ( NewCollectionFromCheckedTraces(std::list< unsigned int > ) ),
                    this,
                    SLOT( CreateNewTrackFromListMeshes(std::list< unsigned int > ) ) );
  QObject::connect( this->m_MeshesManager,
                    SIGNAL( CheckedTracesToAddToSelectedCollection(
                              std::list< unsigned int > ) ), this,
                    SLOT( AddCheckedMeshesToSelectedTrack(std::list< unsigned int > ) ) );
  QObject::connect( this->m_MeshesManager,
                    SIGNAL( DBConnectionNotNeededAnymore() ),
                    this,
                    SLOT( CloseDBConnection() ) );
  QObject::connect( this->m_MeshesManager,
                    SIGNAL( AddNewTraceIDInTS(std::pair< std::string, QColor > ) ),
                    this->m_TraceWidget,
                    SLOT( AddANewCollectionID(std::pair< std::string, QColor > ) ) );
  QObject::connect( this->m_MeshesManager,
                    SIGNAL( PrintMessage(QString, int) ),
                    this,
                    SIGNAL( PrintMessage(QString, int) ) );

  //related to traceEditingWidget and meshes_manager (celltype + subcelltype +
  // collectionData + colordata):
  this->m_MeshesManager->SetSelectedCollection (
    this->m_TraceWidget->GetPointerCollectionData() );
  this->m_MeshesManager->SetSelectedCellType(
    this->m_TraceWidget->GetPointerSelectedCellType() );
  this->m_MeshesManager->SetSelectedSubCellType(
    this->m_TraceWidget->GetPointerSelectedSubCellType() );
  this->m_MeshesManager->SetCurrentTimePoint(this->m_SelectedTimePoint);
  this->m_MeshesManager->SetSelectedColor( this->m_TraceWidget->GetPointerColorData() );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetTracksManager()
{
  this->m_TracksManager = new QGoDBTrackManager(m_ImgSessionID, this);

  QObject::connect( this->m_TracksManager,
                    SIGNAL( TraceColorToChange() ),
                    this, SLOT( ChangeTrackColor() ) );
  QObject::connect( this->m_TracksManager, SIGNAL( CheckedTracesToDelete() ),
                    this, SLOT( DeleteCheckedTracks() ) );

  QObject::connect( this->m_TracksManager,
                    SIGNAL ( NewCollectionFromCheckedTraces(std::list< unsigned int > ) ),
                    this,
                    SLOT( CreateNewLineageFromCheckedTracks(std::list< unsigned int > ) ) );

  QObject::connect( this->m_TracksManager, SIGNAL( NeedToGetDatabaseConnection() ),
                    this, SLOT( PassDBConnectionToTracksManager() ) );

  QObject::connect( this->m_TracksManager,
                    SIGNAL( DBConnectionNotNeededAnymore() ),
                    this,
                    SLOT( CloseDBConnection() ) );
  QObject::connect( this->m_TracksManager, SIGNAL( NeedToGetDatabaseConnection() ),
                    this, SLOT( PassDBConnectionToTracksManager() ) );
  QObject::connect( this->m_TracksManager,
                    SIGNAL( DBConnectionNotNeededAnymore() ),
                    this,
                    SLOT( CloseDBConnection() ) );
  QObject::connect( this->m_TracksManager,
                    SIGNAL( TrackToSplit(unsigned int, std::list< unsigned int > ) ),
                    this,
                    SLOT( SplitTheTrack(unsigned int,
                                        std::list< unsigned int > ) ) );
  QObject::connect( this->m_TracksManager,
                    SIGNAL( TrackIDToBeModifiedWithWidget(std::list< unsigned int > ) ),
                    this,
                    SLOT( SplitMergeTracksWithWidget(std::list< unsigned int > ) ) );
  QObject::connect( this->m_TracksManager,
                    SIGNAL( MeshesToAddToTrack(std::list< unsigned int >, unsigned int) ),
                    this,
                    SLOT( AddListMeshesToATrack(std::list< unsigned int >, unsigned int) ) );
  QObject::connect( this->m_TracksManager,
                    SIGNAL( AddNewTraceIDInTS(std::pair< std::string, QColor > ) ),
                    this->m_TraceWidget,
                    SLOT( AddANewCollectionID(std::pair< std::string, QColor > ) ) );
  QObject::connect( this->m_TracksManager,
                    SIGNAL( PrintMessage(QString, int) ),
                    this,
                    SIGNAL( PrintMessage(QString, int) ) );

  this->m_TracksManager->SetSelectedCollection(
    this->m_TraceWidget->GetPointerCollectionData() );
  this->m_TracksManager->SetSelectedColor( this->m_TraceWidget->GetPointerColorData() );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetLineagesManager()
{
  this->m_LineagesManager = new QGoDBLineageManager(m_ImgSessionID, this);

  QObject::connect( this->m_TracksManager, 
                    SIGNAL (TrackRootLastCreatedLineageToUpdate(unsigned int) ),
                    this->m_LineagesManager, 
                    SLOT( UpdateTrackRootLastCreatedLineage(unsigned int) ) );

  QObject::connect( this->m_LineagesManager, 
                    SIGNAL( NeedToGetDatabaseConnection() ),
                    this, 
                    SLOT( PassDBConnectionToLineagesManager() ) );
  QObject::connect( this->m_LineagesManager,
                    SIGNAL( DBConnectionNotNeededAnymore() ),
                    this,
                    SLOT( CloseDBConnection() ) );

  QObject::connect(this->m_TracksManager,
                   SIGNAL(NewTrackFamilySavedInDB(double*, double*, double*) ),
                   this->m_LineagesManager,
                   SLOT(CreateBasicLineageInVisuFromCurrentElement(double*, double*, double*) ) );

  this->m_LineagesManager->SetSelectedColor( this->m_TraceWidget->GetPointerColorData() );
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
void QGoPrintDatabase::PassDBConnectionToLineagesManager()
{
  this->OpenDBConnection();
  this->m_LineagesManager->SetDatabaseConnection(this->m_DatabaseConnector);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::ReEditTrace(unsigned int iTraceID)
{
  this->m_ReeditMode = true;
  this->m_TraceSettingsDockWidget->setEnabled(false);
  emit TraceToReEdit(iTraceID);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::PassMeshesInfoForImportedTrack(unsigned int iTrackID)
{
  std::list< unsigned int > TrackIDs;
  TrackIDs.push_back(iTrackID);
  this->OpenDBConnection();
  //get the meshesID that have iTrackID as a collectionID:
  std::list< unsigned int > ListMeshesIDs =
    this->m_TracksManager->GetListTracesIDsFromThisCollectionOf(
      this->m_DatabaseConnector, TrackIDs);

  if ( !ListMeshesIDs.empty() )
    {
    //get the coordinate info from the meshes:
    std::map< unsigned int, double * > MeshesInfo = this->m_MeshesManager->
      GetMeshesInfoForImportedMesh(ListMeshesIDs);
    if ( !MeshesInfo.empty() )
      {
      //pass the coordinate info from the meshes in order to calculate the
      //points/string of the track:
      this->m_TracksManager->UpdatePointsOfCurrentElementForImportedTrack(
        MeshesInfo, this->m_DatabaseConnector);
      }
    }
  this->CloseDBConnection();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SplitTheTrack(unsigned int iTrackID,
                                     std::list< unsigned int > iListMeshIDs)
{
  this->OpenDBConnection();
  std::list< unsigned int > ListMeshesForNewTrack =
    this->m_MeshesManager->GetMeshesWithTimePointInfToTheCheckedOne(
      iTrackID, this->m_DatabaseConnector, iListMeshIDs);
  this->CreateNewTrackFromListMeshes(ListMeshesForNewTrack);
  this->CloseDBConnection();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateNewTrackFromListMeshes(
  std::list< unsigned int > iListCheckedMeshes)
{
  this->OpenDBConnection();
  unsigned int NewTrackID =
    this->m_TracksManager->CreateNewTrackWithNoMesh(
      this->m_DatabaseConnector);

  std::list< unsigned int > ListMeshToBelongToTheTrack;
  std::list< unsigned int > ListMeshToReassign;

  std::string MessageToPrint =
    this->m_MeshesManager->CheckListMeshesFromDifferentTimePoints(
      this->m_DatabaseConnector, iListCheckedMeshes,
      ListMeshToBelongToTheTrack, ListMeshToReassign);

  //at that moment, do nothing for the checked meshes not selected to be part of
  // the track
  if ( MessageToPrint != "" )
    {
    emit PrintMessage( MessageToPrint.c_str() );
    }
  this->AddCheckedTracesToCollection< QGoDBMeshManager, QGoDBTrackManager >(
    this->m_MeshesManager, this->m_TracksManager,
    NewTrackID, ListMeshToBelongToTheTrack);
  this->CloseDBConnection();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateNewTrackFromListMeshes(
  std::list< std::list< unsigned int > > iListsCheckedMeshes)
{
  std::list< std::list< unsigned int > >::iterator iter = iListsCheckedMeshes.begin();
  while ( iter != iListsCheckedMeshes.end() )
    {
    CreateNewTrackFromListMeshes(*iter);
    ++iter;
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

void QGoPrintDatabase::CreateNewMeshFromCheckedContours(
  std::list< unsigned int > iListCheckedContours)
{
  this->OpenDBConnection();
  unsigned int NewMeshID =
    this->m_MeshesManager->CreateNewMeshWithNoContourNoPoints(
      this->m_DatabaseConnector);

  this->AddCheckedTracesToCollection< QGoDBContourManager, QGoDBMeshManager >(
    this->m_ContoursManager, this->m_MeshesManager,
    NewMeshID, iListCheckedContours);

  this->m_MeshGenerationMode = true;
  emit NewMeshToGenerate(iListCheckedContours, NewMeshID);
  this->CloseDBConnection();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateNewLineageFromCheckedTracks(
  std::list< unsigned int > iListCheckedTracks )
{
  this->OpenDBConnection();
  unsigned int NewLineageID =
    this->m_LineagesManager->CreateNewLineageWithNoTrack(
      this->m_DatabaseConnector);

  this->AddCheckedTracesToCollection< QGoDBTrackManager, QGoDBLineageManager >(
    this->m_TracksManager, this->m_LineagesManager,
    NewLineageID, iListCheckedTracks);

  this->CloseDBConnection();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::AddCheckedContoursToSelectedMesh(std::list< unsigned int > iListCheckedContours)
{
  this->OpenDBConnection();
  this->AddCheckedTracesToCollection< QGoDBContourManager, QGoDBMeshManager >
    (this->m_ContoursManager, this->m_MeshesManager,
    this->m_TraceWidget->GetCurrentSelectedCollectionID(), iListCheckedContours);
  this->CloseDBConnection();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::AddListMeshesToATrack(std::list< unsigned int > iListMeshes, unsigned int iTrackID)
{
  this->OpenDBConnection();
  std::list< unsigned int > ListMeshToBelongToTheTrack;
  if ( iTrackID == 0 )
    {
    ListMeshToBelongToTheTrack = iListMeshes;
    }
  else
    {
    std::list< unsigned int > ListMeshToReassign;
    //at that moment, do nothing for the checked meshes not selected to be part
    // of the track
    std::string MessageToPrint =
      this->m_MeshesManager->CheckListMeshesFromDifferentTimePoints(
        this->m_DatabaseConnector, iListMeshes,
        ListMeshToBelongToTheTrack, ListMeshToReassign);

    //check for the existing ones:
    MessageToPrint +=
      this->m_MeshesManager->CheckExistingMeshesForTheTrack(
        iTrackID, this->m_DatabaseConnector,
        ListMeshToBelongToTheTrack).toStdString();

    if ( MessageToPrint != "" )
      {
      emit PrintMessage( MessageToPrint.c_str() );
      }
    }

  this->AddCheckedTracesToCollection< QGoDBMeshManager, QGoDBTrackManager >(
    this->m_MeshesManager, this->m_TracksManager,
    iTrackID, ListMeshToBelongToTheTrack);
  this->CloseDBConnection();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::AddListMeshesToATrack(
  std::map< unsigned int, std::list< unsigned int > > iListMeshesWithTracks)
{
  std::map< unsigned int, std::list< unsigned int > >::iterator iter =
    iListMeshesWithTracks.begin();
  while ( iter != iListMeshesWithTracks.end() )
    {
    AddListMeshesToATrack(iter->second, iter->first);
    ++iter;
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::AddCheckedMeshesToSelectedTrack(std::list< unsigned int > iListCheckedMeshes)
{
  unsigned int SelectedTrackID =
    this->m_TraceWidget->GetCurrentSelectedCollectionID();

  this->AddListMeshesToATrack(iListCheckedMeshes, SelectedTrackID);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SplitMergeTracksWithWidget(
  std::list< unsigned int > iTrackIDs)
{
  this->OpenDBConnection();
  MeshContainer *MeshContainerTemp = this->m_MeshesManager->
    GetMeshesInfoFromDBAndCreateContainerForVisu(
      this->m_DatabaseConnector, iTrackIDs);

  QGoTrackEditingWidget *win = new QGoTrackEditingWidget(MeshContainerTemp);
  win->init();

  if ( win->exec() )
    {
    std::list< std::list< unsigned int > >              ListTracksToCreate = win->GetListOfTracksToBeCreated();
    std::map< unsigned int, std::list< unsigned int > > ListTracksToUpdate =
      win->GetListOfTracksToBeUpdated();
    std::list< unsigned int > ListTracksToDelete = win->GetListOfTracksToBeDeleted();
    if ( !ListTracksToCreate.empty() )
      {
      this->CreateNewTrackFromListMeshes(ListTracksToCreate);
      }
    if ( !ListTracksToUpdate.empty() )
      {
      this->AddListMeshesToATrack(ListTracksToUpdate);
      }
    if ( !ListTracksToDelete.empty() )
      {
      this->DeleteListTraces< QGoDBTrackManager, QGoDBMeshManager, QGoDBMeshManager >(
        this->m_TracksManager, this->m_MeshesManager, this->m_MeshesManager,
        ListTracksToDelete, true);
      }
    }
  delete win;
}