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

#include "QTableWidgetChild.h"
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

//--------------------------------------------------------------------------
QGoPrintDatabase::
QGoPrintDatabase(QWidget* iParent) :
  QWidget(iParent),
  m_ContoursData(NULL),
  m_MeshesData(NULL),
  m_TracksData(NULL),
  m_LineagesData(NULL),
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
  this->CreateConnectionsForTraceManualEditingWidget();

  QObject::connect(m_VisibilityAction, SIGNAL(toggled(bool)),
                   this, SLOT(setVisible(bool)));

  QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
                   this, SLOT(CreateContextMenu(const QPoint &)));
  QObject::connect(this->DBTabWidget, SIGNAL(currentChanged(int)),
                   this, SLOT(TheTabIsChanged(int)));

  // QObject::connect( this->ContourTable, SIGNAL(itemSelectionChanged()),
  //   this, SLOT(ChangeTracesToHighLightInfoFromTableWidget()));

  // QObject::connect( this->MeshTable, SIGNAL(itemSelectionChanged()),
  //   this, SLOT(ChangeTracesToHighLightInfoFromTableWidget()));

  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoPrintDatabase::~QGoPrintDatabase()
  {
  // Need to release memory allocated for contours, meshes, tracks and lineages

  // contours
  if (m_ContoursData)
    {
    if (this->m_ContoursData->ListTracesInfoForVisu != 0)
      {
      DeleteContourMeshStructureElement(
        *this->m_ContoursData->ListTracesInfoForVisu);
      }
    delete m_ContoursData;
    }
  // meshes
  if (m_MeshesData)
    {
    if (this->m_MeshesData->ListTracesInfoForVisu != 0)
      {
      DeleteContourMeshStructureElement(
        *this->m_MeshesData->ListTracesInfoForVisu);
      }
    delete m_MeshesData;
    }

  // tracks
  if (m_TracksData)
    {
    delete m_TracksData;
    }

  // lineages
  if (m_LineagesData)
    {
    delete m_LineagesData;
    }
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

  this->SetTraceInfoStructures();

  this->m_BookmarkManager = new QGoDBBookmarkManager(this, this->m_ImgSessionID);
  QObject::connect(this->m_BookmarkManager, SIGNAL(ListBookmarksChanged()),
                   this, SIGNAL(OpenBookmarksToUpdate()));
  //to check: set the list directly ?
  this->m_CellTypeManager = new QGoDBCellTypeManager(this);
  //to check: set the list directly ?
  this->m_SubCellTypeManager = new QGoDBSubCellTypeManager(this);
  this->InitializeTheComboboxesNotTraceRelated();
  this->SetListExistingCollectionIDFromDB();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::SetTraceInfoStructures()
{
  m_ContoursData = new TraceInfoStructure("contour", this);
  m_TracksData = new TraceInfoStructure("track", this);
  m_LineagesData = new TraceInfoStructure("lineage", this);
  m_MeshesData = new TraceInfoStructure("mesh", this);

  QObject::connect(this->m_ContoursData->Table, SIGNAL (CheckedRowsChanged()),
                   this, SLOT(ChangeTracesToHighLightInfoFromTableWidget()));

  QObject::connect(this->m_MeshesData->Table, SIGNAL (CheckedRowsChanged()),
                   this, SLOT(ChangeTracesToHighLightInfoFromTableWidget()));

  QObject::connect(this->m_ContoursData->Table, SIGNAL (VisibleRowsChanged()),
                   this, SLOT(ChangeTracesToShowInfoFromTableWidget()));

  QObject::connect(this->m_MeshesData->Table, SIGNAL (VisibleRowsChanged()),
                   this, SLOT(ChangeTracesToShowInfoFromTableWidget()));

  this->m_ContoursData->CollectionOfTraces->SetImgSessionID(m_ImgSessionID);
  this->m_MeshesData->CollectionOfTraces->SetImgSessionID(m_ImgSessionID);
  this->OpenDBConnection();
  this->m_MeshesData->CollectionOfTraces->SetChannelsInfo(this->m_DatabaseConnector);
  this->CloseDBConnection();
  this->m_TracksData->CollectionOfTraces->SetImgSessionID(m_ImgSessionID);
  this->m_LineagesData->CollectionOfTraces->SetImgSessionID(m_ImgSessionID);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::OpenDBConnection()
{
  if (m_DatabaseConnector == 0)
    {
    m_DatabaseConnector = OpenDatabaseConnection(m_Server, m_User,
                                                 m_Password, m_DBName);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CloseDBConnection()
{
  if (CloseDatabaseConnection(m_DatabaseConnector))
    {
    m_DatabaseConnector = 0;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::FillTableFromDatabase(int iTimePoint)
{
  OpenDBConnection();

  QString title = QString("Imaging Session: %1 ").arg(m_ImgSessionName.c_str());
  this->setWindowTitle(title);

  GetContentAndDisplayFromDB("contour");
  GetContentAndDisplayFromDB("mesh");

  this->m_LineagesData->Table->hide();
  this->m_TracksData->Table->hide();

  GetContentAndDisplayFromDB("track");
  //GetContentAndDisplayFromDB( "lineage" );

  LoadContoursAndMeshesFromDB(m_DatabaseConnector);

  m_IsDatabaseUsed = true;
  //std::list<std::pair<std::string, std::vector<int> > > test =
    //this->GetColorComboBoxInfofromDB(); //for test
  //emit PrintExistingColorsFromDB(this->GetColorComboBoxInfofromDB());


  /** \todo get the trace name from the visudockwidget*/
  //PrintExistingCollectionIDsFromDB(
    //this->GetListExistingCollectionIDFromDB("contour", iTimePoint));
  CloseDBConnection();
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
void QGoPrintDatabase::CreateContextMenu(const QPoint& iPos)
{
  QMenu* ContextMenu = new QMenu;
  //NeedCurrentSelectedCollectionID();
  std::string TraceName = this->InWhichTableAreWe();

  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);

  /** \todo when using lineages, remove the following*/
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
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTraces()
{
  std::string         TraceName = this->InWhichTableAreWe();
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(
    TraceName);

  std::list<int> SelectedTraces =
    CurrentlyUsedTraceData->Table->GetListCheckedTraceID();

  if (SelectedTraces.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1 to be deleted")
      .arg(TraceName.c_str()));
    msgBox.exec();
    }
  else
    {
    int r = QMessageBox::warning(this, tr(""),
                                 tr("Are you sure you want to delete\n"
                                    "permanently the selected %1s?").arg(TraceName.c_str()),
                                 QMessageBox::Yes,
                                 QMessageBox::No | QMessageBox::Default);

    if (r == QMessageBox::Yes)
      {
      OpenDBConnection();

      std::vector<std::string> VectorDeletedValues;
      std::list<int>::iterator iter = SelectedTraces.begin();

      while (iter != SelectedTraces.end())
        {
        int ID = *iter;
        //delete the trace in the vector<ContourMeshStructure>:
        this->DeleteTraceInContourMeshStructure(ID,
                                                CurrentlyUsedTraceData->ListTracesInfoForVisu);

        VectorDeletedValues.push_back(ConvertToString<int>(ID));

        ++iter;
        }
      this->UpdateInfoForTracesToBeDeletedAsCollectionOf(TraceName,
                                                         VectorDeletedValues);
      //update the table widget for the delete traces:
      CurrentlyUsedTraceData->Table->DeleteSelectedRows(CurrentlyUsedTraceData->TraceNameID);

      //delete traces in database + update bounding box for collections and update table widget:
      this->DeleteTracesAsPartOfACollection(TraceName, SelectedTraces);
      CloseDBConnection();

      emit TracesToDeleteInVisu(SelectedTraces);
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::GoToTheTrace()
{
  std::string         TraceName = this->InWhichTableAreWe();
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure(TraceName);
  if (CurrentlyUsedTraceData->Table->GetListCheckedTraceID().size() != 1)
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select one and only one %1 to go to")
      .arg(TraceName.c_str()));
    msgBox.exec();
    return;
    }
  GoDBCoordinateRow CoordCenter =
    CurrentlyUsedTraceData->Table->GetCoordinateCenterBoundingBox();
  emit NeedToGoToTheLocation(atoi(CoordCenter.GetMapValue("XCoord").c_str()),
                             atoi(CoordCenter.GetMapValue("YCoord").c_str()),
                             atoi(CoordCenter.GetMapValue("ZCoord").c_str()),
                             atoi(CoordCenter.GetMapValue("TCoord").c_str()));
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateCorrespondingCollection()
{
  std::string         TraceName = InWhichTableAreWe();
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);
  std::list<int>      ListSelectedTraces = CurrentlyUsedTraceData->Table->GetListCheckedTraceID();
  if (ListSelectedTraces.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1 to be part of the new %2")
      .arg(CurrentlyUsedTraceData->TraceName.c_str())
      .arg(CurrentlyUsedTraceData->CollectionName.c_str()));
    msgBox.exec();
    }
  else
    {
    OpenDBConnection();
    //set the color for the new collection:
    GoDBTraceRow NewCollection;
    //emit         NeedToGetCurrentSelectedColor();
    NewCollection.SetColor(this->m_SelectedColorData.second.red(), this->m_SelectedColorData.second.green(),
                           this->m_SelectedColorData.second.blue(), this->m_SelectedColorData.second.alpha(),
                           this->m_SelectedColorData.first, this->m_DatabaseConnector);
    //create the collection in the database and get the corresponding ID:
    int NewCollectionID = CurrentlyUsedTraceData->CollectionOfTraces->
                          CreateNewCollectionFromSelection<GoDBTraceRow>(
      ListSelectedTraces, this->m_DatabaseConnector, NewCollection);

    std::pair<std::string, QColor> NewCollectionInfo(
      ConvertToString(NewCollectionID), this->m_SelectedColorData.second);
    this->AddListTracesToACollection(ListSelectedTraces, NewCollectionInfo, TraceName, true);

    CloseDBConnection();
    QString CollectionIDQString = ConvertToString<int>(NewCollectionID).c_str();

    //emit    NewCreatedCollection(this->m_SelectedColorData.second, CollectionIDQString);
    if (CurrentlyUsedTraceData->TraceName == "contour")
      {
      emit NewMeshToGenerate(ListSelectedTraces,NewCollectionID);
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string QGoPrintDatabase::InWhichTableAreWe()
{
  int CurrentIndex = this->DBTabWidget->currentIndex();
  return this->DBTabWidget->tabText(CurrentIndex).toStdString();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::AddToSelectedCollection()
{
  std::string         TraceName = this->InWhichTableAreWe();
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
    this->AddListTracesToACollection(ListSelectedTraces,
                                     this->m_SelectedCollectionData, TraceName, false);
    if (CurrentlyUsedTraceData->TraceName == "contour")
      {
      std::list<int> ListIDs = CurrentlyUsedTraceData->CollectionOfTraces->GetTracesIDPartOfTheCollection(
        this->m_DatabaseConnector,
        atoi(this->m_SelectedCollectionData.first.c_str()));
      //emit MeshGenerationToUpdate (ListIDs);
      }
    CloseDBConnection();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::ChangeTraceColor()
{
  std::string         TraceName = this->InWhichTableAreWe();
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure(TraceName);
  std::list<int> ListSelectedTraces =
    CurrentlyUsedTraceData->Table->GetListCheckedTraceID();
  //emit NeedToGetCurrentSelectedColor();
  if (ListSelectedTraces.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1 for the color to be changed")
      .arg(CurrentlyUsedTraceData->TraceName.c_str()));
    msgBox.exec();
    }
  else
    {
    OpenDBConnection();
    int ColorID = FindOneID(
      this->m_DatabaseConnector, "color", "ColorID", "Name",
      this->m_SelectedColorData.first);

    std::list<int>::iterator iter = ListSelectedTraces.begin();
    while (iter != ListSelectedTraces.end())
      {
      UpdateValueInDB(this->m_DatabaseConnector,
                      CurrentlyUsedTraceData->TraceName, "ColorID",
                      ConvertToString<int>(ColorID), CurrentlyUsedTraceData->TraceNameID,
                      ConvertToString<int>(*iter));

      this->UpdateTableWidgetForAnExistingTrace(
        CurrentlyUsedTraceData->TraceName, *iter);

      if (CurrentlyUsedTraceData->CollectionOf != "None")
        {
        std::vector<std::string> ListTracesFromThisCollectionOf =
          ListSpecificValuesForOneColumn(this->m_DatabaseConnector,
                                         CurrentlyUsedTraceData->CollectionOf,
                                         CurrentlyUsedTraceData->CollectionOfID,
                                         CurrentlyUsedTraceData->TraceNameID, ConvertToString<int>(*iter));
        for (unsigned int i = 0; i < ListTracesFromThisCollectionOf.size(); i++)
          {
          this->UpdateTableWidgetForAnExistingTrace(
            CurrentlyUsedTraceData->CollectionOf,
            atoi(ListTracesFromThisCollectionOf[i].c_str()));
          }
        }
      ++iter;
      std::pair<std::list<int>, QColor> ListIDsWithColor;
      ListIDsWithColor.first = ListSelectedTraces;
      ListIDsWithColor.second = this->m_SelectedColorData.second;
      emit ColorChangedForSelectedTraces(ListIDsWithColor);
      }
    CloseDBConnection();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::LoadContoursAndMeshesFromDB(
  vtkMySQLDatabase* DatabaseConnector)
{
  /** \todo put it in the TraceInfoStructure and the GetTracesInforFromDB
  in the collection of traces?*/
  this->m_ContoursData->ListTracesInfoForVisu =
    GetTracesInfoFromDB(DatabaseConnector, "contour", "mesh", m_ImgSessionID);
  this->m_MeshesData->ListTracesInfoForVisu =
    GetTracesInfoFromDB(DatabaseConnector, "mesh", "track", m_ImgSessionID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
ContourMeshStructureMultiIndexContainer* QGoPrintDatabase::
AddContoursFromDBForAGivenTimePoint(int iTimePoint, std::vector<int> iListIDs)
{
  this->OpenDBConnection();

  ContourMeshStructureMultiIndexContainer* ContoursInfoToAdd =
    GetTracesInfoFromDB(this->m_DatabaseConnector, "contour", "mesh",
                        this->m_ImgSessionID, iTimePoint, iListIDs);

  for (unsigned int i = 0; i < iListIDs.size(); i++)
    {
    this->AddATraceToContourMeshInfo("contour", iListIDs.at(i));
    }

  this->CloseDBConnection();

  return ContoursInfoToAdd;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
ContourMeshStructureMultiIndexContainer*
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
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
ContourMeshStructureMultiIndexContainer* QGoPrintDatabase::
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
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
ContourMeshStructureMultiIndexContainer* QGoPrintDatabase::
AddMeshesFromDBForAGivenTimePoint(int iTimePoint, std::vector<int> iListIDs)
{
  this->OpenDBConnection();

  ContourMeshStructureMultiIndexContainer* MeshesInfoToAdd =
    GetTracesInfoFromDB(this->m_DatabaseConnector, "mesh", "track",
                        this->m_ImgSessionID, iTimePoint, iListIDs);

  for (unsigned int i = 0; i < iListIDs.size(); i++)
    {
    this->AddATraceToContourMeshInfo("mesh", iListIDs.at(i));
    }

  this->CloseDBConnection();

  return MeshesInfoToAdd;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
ContourMeshStructureMultiIndexContainer* QGoPrintDatabase::
GetMeshesFromDBForAGivenTimePoint(int iTimePoint)
{
  this->OpenDBConnection();
  ContourMeshStructureMultiIndexContainer* MeshesInfo = GetTracesInfoFromDB(
    this->m_DatabaseConnector, "mesh", "track", m_ImgSessionID, iTimePoint);
  this->CloseDBConnection();
  return MeshesInfo;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::
ChangeTracesToHighLightInfoFromTableWidget()
{
  std::string         TraceName = this->InWhichTableAreWe();
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(
    TraceName);
  bool HasBeenModified =
    CurrentlyUsedTraceData->Table->TracesToHighlight(
      CurrentlyUsedTraceData->ListTracesInfoForVisu);

  if (HasBeenModified)
    {
    if (TraceName.compare("contour") == 0)
      {
      emit SelectionContoursToHighLightChanged();
      }
    else if (TraceName.compare("mesh") == 0)
      {
      emit SelectionMeshesToHighLightChanged();
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::
ChangeTracesToShowInfoFromTableWidget()
{
  std::string         TraceName = this->InWhichTableAreWe();
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(
    TraceName);
  bool HasBeenModified =
    CurrentlyUsedTraceData->Table->TracesToShow(
      CurrentlyUsedTraceData->ListTracesInfoForVisu);

  if (HasBeenModified)
    {
    if (TraceName.compare("contour") == 0)
      {
      emit SelectionContoursToShowChanged();
      }
    else if (TraceName.compare("mesh") == 0)
      {
      emit SelectionMeshesToShowChanged();
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::
ChangeContoursToHighLightInfoFromVisu(
  std::list<int> iListContoursHighLightedInVisu, bool Reedit)
{
  /** \todo get the tracename from the visu dock widget*/
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure("contour");
  if (!Reedit)
    {
    if (!iListContoursHighLightedInVisu.empty())
      {
      std::list<int>::iterator it = iListContoursHighLightedInVisu.begin();

      while (it != iListContoursHighLightedInVisu.end())
        {
        //for( unsigned int j = 0 ; j < m_ContoursInfo.size(); j++ )
        ContourMeshStructureMultiIndexContainer::index<TraceID>::type::iterator
        traceid_it =
          CurrentlyUsedTraceData->ListTracesInfoForVisu->get<TraceID>().find(*it);

        // note here there is only one element with a given TraceID == *it
        if (traceid_it !=
            CurrentlyUsedTraceData->ListTracesInfoForVisu->get<TraceID>().end())
          {
          if (!traceid_it->Highlighted)
            {
            ContourMeshStructure temp(*traceid_it);
            temp.Highlighted = true;
            CurrentlyUsedTraceData->ListTracesInfoForVisu->get<TraceID>().replace(traceid_it, temp);
            CurrentlyUsedTraceData->Table->SetSelectRowTraceID(CurrentlyUsedTraceData->TraceName,
              *it, Qt::Checked);
            }
          else
            {
            ContourMeshStructure temp(*traceid_it);
            temp.Highlighted = false;
            CurrentlyUsedTraceData->ListTracesInfoForVisu->get<TraceID>().replace(traceid_it, temp);
            CurrentlyUsedTraceData->Table->SetSelectRowTraceID(CurrentlyUsedTraceData->TraceName,
              *it, Qt::Unchecked);
            }
          }

        /*std::list< ContourMeshStructure* > Trace = FindContourGivenTraceID(
          *CurrentlyUsedTraceData->ListTracesInfoForVisu,static_cast< unsigned int >( *it ) );
        Trace.front()
          bool temp_selected =
            ( static_cast< unsigned int >( *it ) == (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].TraceID );

          (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].Highlighted = temp_selected;
          CurrentlyUsedTraceData->Table->SetSelectRowTraceID( CurrentlyUsedTraceData->TraceName,
            (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].TraceID, temp_selected );
          }*/
        ++it;
        }
      }
    else
      {
      /// \todo fix this using multi index container
      ContourMeshStructureMultiIndexContainer::iterator trace_it =
        CurrentlyUsedTraceData->ListTracesInfoForVisu->begin();

      while (trace_it != CurrentlyUsedTraceData->ListTracesInfoForVisu->end())
        {
        if (trace_it->Highlighted)
          {
          ContourMeshStructure temp(*trace_it);
          temp.Highlighted = false;
          CurrentlyUsedTraceData->ListTracesInfoForVisu->replace(trace_it, temp);
          CurrentlyUsedTraceData->Table->SetSelectRowTraceID(
            CurrentlyUsedTraceData->TraceName, trace_it->TraceID, Qt::Unchecked);
          }
        ++trace_it;
        }
      /*for( unsigned int j = 0 ; j < CurrentlyUsedTraceData->ListTracesInfoForVisu->size(); j++ )
        {
        (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].Highlighted = false;

        CurrentlyUsedTraceData->Table->SetSelectRowTraceID( CurrentlyUsedTraceData->TraceName,
          (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].TraceID, false);
        }*/
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::
ChangeMeshesToHighLightInfoFromVisu(
  std::list<int> iListMeshesHighLightedInVisu)
{
  std::list<int>::iterator it = iListMeshesHighLightedInVisu.begin();
  /** \todo get the tracename from the visu dock widget*/
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure("mesh");

  while (it != iListMeshesHighLightedInVisu.end())
    {
    ContourMeshStructureMultiIndexContainer::index<TraceID>::type::iterator
    traceid_it =
      CurrentlyUsedTraceData->ListTracesInfoForVisu->get<TraceID>().find(*it);

    // note here there is only one element with a given TraceID == *it
    if (traceid_it !=
        CurrentlyUsedTraceData->ListTracesInfoForVisu->get<TraceID>().end())
      {
      ContourMeshStructure temp(*traceid_it);
      temp.Highlighted = traceid_it->Highlighted;
      CurrentlyUsedTraceData->ListTracesInfoForVisu->get<TraceID>().replace(traceid_it, temp);
      /*CurrentlyUsedTraceData->Table->SetSelectRowTraceID(CurrentlyUsedTraceData->TraceName,
                                                         *it, !traceid_it->Highlighted);*/
       if (!traceid_it->Highlighted)
         {
         CurrentlyUsedTraceData->Table->SetSelectRowTraceID( CurrentlyUsedTraceData->TraceName,
           *it, Qt::Checked );
         }
       else
         {
         CurrentlyUsedTraceData->Table->SetSelectRowTraceID( CurrentlyUsedTraceData->TraceName,
           *it, Qt::Unchecked );
         }
      }
    ++it;
    }

  /// \todo check it seems to be the same code as in contour
  /*for( unsigned int j = 0 ; j < CurrentlyUsedTraceData->ListTracesInfoForVisu->size(); j++ )
    {
    bool temp_selected =
      ( static_cast< unsigned int >( *it ) ==
      (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].TraceID );

    (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].Highlighted = temp_selected;

    CurrentlyUsedTraceData->Table->SetSelectRowTraceID( CurrentlyUsedTraceData->TraceName,
      (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].TraceID, temp_selected );
    }
    ++it;
  }*/
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoPrintDatabase::IDWithColorData 
QGoPrintDatabase::SaveContoursFromVisuInDB(unsigned int iXCoordMin,
                                           unsigned int iYCoordMin,
                                           unsigned int iZCoordMin,
                                           unsigned int iTCoord,
                                           unsigned int iXCoordMax,
                                           unsigned int iYCoordMax,
                                           unsigned int iZCoordMax,
                                           vtkPolyData* iContourNodes)
                                               //std::pair<std::string, QColor> iColorData,
                                               //unsigned int iMeshID)
{
  OpenDBConnection();
  GoDBContourRow contour_row(this->m_ImgSessionID);
  this->GetTraceRowFromVisu<GoDBContourRow>(iXCoordMin,
                                            iYCoordMin,
                                            iZCoordMin,
                                            iTCoord,
                                            iXCoordMax,
                                            iYCoordMax,
                                            iZCoordMax,
                                            iContourNodes,
                                            this->m_DatabaseConnector,
                                            contour_row);
                                            
  /*contour_row.SetColor(iColorData.second.red(), iColorData.second.green(),
                       iColorData.second.blue(), iColorData.second.alpha(), iColorData.first,
                       this->m_DatabaseConnector);*/
  contour_row.SetColor(this->m_SelectedColorData.second.red(), m_SelectedColorData.second.green(),
                       m_SelectedColorData.second.blue(), m_SelectedColorData.second.alpha(), m_SelectedColorData.first,
                       this->m_DatabaseConnector);

  contour_row.SetCollectionID(atoi(m_SelectedCollectionData.first.c_str()));
  int NewContourID = contour_row.SaveInDB(this->m_DatabaseConnector);
  this->UpdateTableWidgetAndDBWithNewCreatedTrace("contour");
  std::list<int> ListSelectedTraces;

  ListSelectedTraces.push_back(NewContourID);
  //if (iMeshID != 0)
  /** \todo check if there is no pb with no more iMeshID: */
  //if (iMeshID == 0)
  //  {
    //emit this->NeedCurrentSelectedCollectionID();
    this->AddListTracesToACollection(
      ListSelectedTraces, this->m_SelectedCollectionData, "contour", false);
  //  }
  this->AddATraceToContourMeshInfo("contour", NewContourID);

  CloseDBConnection();
  IDWithColorData NewContourData;
  NewContourData.first = NewContourID;
  NewContourData.second = this->m_SelectedColorData.second;
  return NewContourData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoPrintDatabase::IDWithColorData QGoPrintDatabase::UpdateContourFromVisuInDB(unsigned int iXCoordMin,
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
  this->GetTraceRowFromVisu<GoDBContourRow>(iXCoordMin,
                                            iYCoordMin, iZCoordMin, iTCoord, iXCoordMax, iYCoordMax, iZCoordMax,
                                            iContourNodes, this->m_DatabaseConnector, contour_row);
  contour_row.SetField<int>("ContourID", ContourID);
  UpdateContourInDB(this->m_DatabaseConnector, contour_row);

  int CollectionID = FindOneID(this->m_DatabaseConnector,
                               "contour", "meshID", "contourID", ConvertToString<int>(ContourID));
  this->m_ContoursData->CollectionOfTraces->RecalculateDBBoundingBox(
    this->m_DatabaseConnector, CollectionID);
  this->UpdateTableWidgetForAnExistingTrace("mesh", CollectionID);
  this->UpdateTableWidgetForAnExistingTrace("contour", ContourID);
  CloseDBConnection();
  IDWithColorData UpdatedContourData;
  UpdatedContourData.first = ContourID;
  QColor Color(atoi(contour_row.GetMapValue("red").c_str()),
               atoi(contour_row.GetMapValue("green").c_str()),
               atoi(contour_row.GetMapValue("blue").c_str()),
               atoi(contour_row.GetMapValue("alpha").c_str()));
  UpdatedContourData.second = Color;
  return UpdatedContourData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoPrintDatabase::IDWithColorData QGoPrintDatabase::SaveMeshFromVisuInDB(
                                           unsigned int iXCoordMin,
                                           unsigned int iYCoordMin,
                                           unsigned int iZCoordMin,
                                           unsigned int iTCoord,
                                           unsigned int iXCoordMax,
                                           unsigned int iYCoordMax,
                                           unsigned int iZCoordMax,
                                           vtkPolyData* iMeshNodes,
                                           GoFigureMeshAttributes* iMeshAttributes,
                                           bool NewMesh, int iMeshID)
{
  OpenDBConnection();
  IDWithColorData MeshData;
  //emit NeedToGetCurrentSelectedColor();
  //emit NeedCurrentSelectedCollectionID();

  GoDBMeshRow                    mesh_row(this->m_ImgSessionID);
  std::pair<std::string, QColor> CollectionData;
  int                            SavedMeshID;

  if (!NewMesh)
    {
    if (iMeshID == 0)
      {
      mesh_row.SetValuesForSpecificID(
        atoi(this->m_SelectedCollectionData.first.c_str()), this->m_DatabaseConnector);
      CollectionData.first = mesh_row.GetMapValue("TrackID");
      }
    else
      {
      mesh_row.SetValuesForSpecificID(iMeshID, this->m_DatabaseConnector);
      }
    GoDBTrackRow Collection;
    Collection.SetValuesForSpecificID(atoi(CollectionData.first.c_str()),
                                      this->m_DatabaseConnector);
    /*QColor ColorCollection(atoi(Collection.GetMapValue("Red").c_str()),
                           atoi(Collection.GetMapValue("Green").c_str()),
                           atoi(Collection.GetMapValue("Blue").c_str()),
                           atoi(Collection.GetMapValue("Alpha").c_str()));*/
    CollectionData.second = this->GetQColorFromTraceRow<GoDBTrackRow>(Collection);
    /*QColor ColorMesh(atoi(mesh_row.GetMapValue("Red").c_str()),
                           atoi(mesh_row.GetMapValue("Green").c_str()),
                           atoi(mesh_row.GetMapValue("Blue").c_str()),
                           atoi(mesh_row.GetMapValue("Alpha").c_str()));*/
    MeshData.first = atoi(mesh_row.GetMapValue("MeshID").c_str());
    MeshData.second = this->GetQColorFromTraceRow<GoDBMeshRow>(mesh_row);
    }

  this->GetTraceRowFromVisu<GoDBMeshRow>(iXCoordMin, iYCoordMin,
                                         iZCoordMin, iTCoord, iXCoordMax, iYCoordMax, iZCoordMax,
                                         iMeshNodes, this->m_DatabaseConnector, mesh_row, 0, iMeshAttributes);

  if (NewMesh)
    {
    //if the new mesh is created from a collection of contours,
    //there will be no collectionID from the tracemanualediting widget as
    //the collection in the widget will be mesh and not track
    mesh_row.SetColor(m_SelectedColorData.second.red(),
                      m_SelectedColorData.second.green(), m_SelectedColorData.second.blue(),
                      m_SelectedColorData.second.alpha(), m_SelectedColorData.first,
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
 
  /** \todo check if it is needed for an updated mesh*/
  std::list<int> ListSelectedTraces;
  ListSelectedTraces.push_back(SavedMeshID);
  this->AddListTracesToACollection(
    ListSelectedTraces, CollectionData, "mesh", false);

  CloseDBConnection();

  //return SavedMeshID;
  return MeshData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoPrintDatabase::GetSelectedCellTypeID()
{
  //emit NeedCurrentSelectedCellTypeAndSubCellType();
  return FindOneID(this->m_DatabaseConnector,
                   "celltype", "CellTypeID", "Name", this->m_SelectedCellType);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoPrintDatabase::GetSelectedSubCellTypeID()
{
  //emit NeedCurrentSelectedCellTypeAndSubCellType();
  return FindOneID(this->m_DatabaseConnector,
                   "subcellulartype", "SubCellularID", "Name", this->m_SelectedSubCellType);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
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
    //emit NeedCurrentSelectedCellTypeAndSubCellType();
    //emit NeedToGetCurrentSelectedColor();
    //emit NeedCurrentSelectedCollectionID();

    NewMesh.SetColor(this->m_SelectedColorData.second.red(), this->m_SelectedColorData.second.green(),
                     this->m_SelectedColorData.second.blue(), this->m_SelectedColorData.second.alpha(),
                     this->m_SelectedColorData.first, this->m_DatabaseConnector);
    NewMesh.SetCollectionID(atoi(this->m_SelectedCollectionData.first.c_str()));
    NewMesh.SetField<int>("CellTypeID", this->GetSelectedCellTypeID());  
    NewMesh.SetField<int>("SubCellularID", this->GetSelectedSubCellTypeID());

    //create the collection in the database and get the corresponding ID:
    NewMeshID = CurrentlyUsedTraceData->CollectionOfTraces->
                CreateNewCollectionFromSelection<GoDBMeshRow>(
      iListContoursIDs, this->m_DatabaseConnector, NewMesh);

    std::pair<std::string, QColor> NewCollectionInfo(
      ConvertToString(NewMeshID), this->m_SelectedColorData.second);
    this->AddListTracesToACollection(iListContoursIDs, NewCollectionInfo, "contour", true);

    CloseDBConnection();
    QString CollectionIDQString = ConvertToString<int>(NewMeshID).c_str();
    //emit NewCreatedCollection(this->m_CurrentColorData.second,CollectionIDQString);
    }
  return NewMeshID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::GetContentAndDisplayFromDB(std::string iTraceName)
{
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure(iTraceName);
  //Get the column names to be displayed in the table widget:
  std::list<std::string> ColumnsNames =
    CurrentlyUsedTraceData->CollectionOfTraces->GetListColumnsNamesForTableWidget();
  CurrentlyUsedTraceData->Table->DisplayColumnNames(
    iTraceName.c_str(), ColumnsNames);
  this->DBTabWidget->addTab(CurrentlyUsedTraceData->Table, iTraceName.c_str());
  DBTableWidgetContainerType Row_Container =
    CurrentlyUsedTraceData->CollectionOfTraces->GetRowContainer(m_DatabaseConnector);
  CurrentlyUsedTraceData->Table->DisplayContent(CurrentlyUsedTraceData->CollectionOfTraces->GetLinkToRowContainer(),
                                                iTraceName, CurrentlyUsedTraceData->CollectionName);
  CurrentlyUsedTraceData->Table->setSortingEnabled(true);

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
std::vector<ContourMeshStructure> QGoPrintDatabase::
GetContoursForAGivenZCoord(unsigned int iZCoord)
{
  return this->GetTracesForAGivenZCoord(*this->m_ContoursData->ListTracesInfoForVisu, iZCoord,
                                        this->m_ContoursData->CollectionOfTraces);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
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
std::vector<ContourMeshStructure> QGoPrintDatabase::
GetMeshesForAGivenZCoord(unsigned int iZCoord)
{
  return this->GetTracesForAGivenZCoord(
           *this->m_MeshesData->ListTracesInfoForVisu, iZCoord,
           this->m_MeshesData->CollectionOfTraces);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure> QGoPrintDatabase::
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
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::
SetColorComboBoxInfofromDB()
{
  this->OpenDBConnection();
  std::list<ItemColorComboboxData > InfoColors;
  std::vector<std::string> ResultsQuery  = ListAllValuesForOneColumn(
    m_DatabaseConnector, "*", "color","name");
  unsigned int i = 0;
  while (i < ResultsQuery.size())
    {
    ItemColorComboboxData temp;
    temp.first = ResultsQuery[i + 1];
    QColor tempColor(atoi(ResultsQuery[i + 2].c_str()),atoi(ResultsQuery[i + 3].c_str()),
      atoi(ResultsQuery[i + 4].c_str()),atoi(ResultsQuery[i + 5].c_str()));
    /*std::string Red = ResultsQuery[i + 2];

    temp.second.push_back(atoi(Red.c_str()));
    temp.second.push_back(atoi(ResultsQuery[i + 3].c_str()));
    temp.second.push_back(atoi(ResultsQuery[i + 4].c_str()));
    temp.second.push_back(atoi(ResultsQuery[i + 5].c_str()));*/
    temp.second = tempColor;
    InfoColors.push_back(temp);
    i = i + 7;
    }
  this->CloseDBConnection();
  this->m_TraceWidget->SetListColors(InfoColors);  
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::
SaveNewColorInDB(ItemColorComboboxData iDataNewColor)
{
  this->OpenDBConnection();
  GoDBColorRow NewColor;
  /*if (iDataNewColor.size() != 5)
    {
    std::cout << "Pb: the number of data for the new color is not 5";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
  else
    {*/
    NewColor.SetField("Name", iDataNewColor.first);
    /*if (NewColor.DoesThisColorAlreadyExists(this->m_DatabaseConnector)
        != -1)
      {
      QMessageBox msgBox;
      msgBox.setText(
        tr("This name already exits, please chose another one"));
      msgBox.exec();
      emit TheColorNameAlreadyExits();
      }
    else
      {*/
    NewColor.SetField<int>("Red", iDataNewColor.second.red());
    NewColor.SetField<int>("Green", iDataNewColor.second.green());
    NewColor.SetField<int>("Blue", iDataNewColor.second.blue());
    NewColor.SetField<int>("Alpha", iDataNewColor.second.alpha());
    NewColor.SaveInDB(m_DatabaseConnector);

    this->CloseDBConnection();
    this->SetColorComboBoxInfofromDB();
    //this->m_TraceWidget->AddAnewColorInCombobox(iDataNewColor);
      //}
    //}
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoPrintDatabase::IsDatabaseUsed()
{
  return m_IsDatabaseUsed;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<QGoPrintDatabase::ItemColorComboboxData> 
QGoPrintDatabase::GetListCollectionIDFromDB()
{
  OpenDBConnection();
  std::string TraceName = this->m_TraceWidget->GetTraceName();
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);
  std::list<ItemColorComboboxData > oListCollectionIDs;
  //First, build the query with selected fields and table to join with on conditions:
  std::vector<std::string> SelectFields;
  std::string              CollectionID = CurrentlyUsedTraceData->CollectionName;
  CollectionID += ".";
  CollectionID += CurrentlyUsedTraceData->CollectionNameID;
  SelectFields.push_back(CollectionID);
  std::string Red = "color.Red";
  SelectFields.push_back(Red);
  std::string Green = "color.Green";
  SelectFields.push_back(Green);
  std::string Blue = "color.Blue";
  SelectFields.push_back(Blue);
  std::string Alpha = "color.Alpha";
  SelectFields.push_back(Alpha);

  std::vector<std::string> JoinTablesOnTraceTable;
  std::string              JoinTable = "color";
  JoinTablesOnTraceTable.push_back(JoinTable);
  std::string OnCondition = CurrentlyUsedTraceData->CollectionName;
  OnCondition += ".ColorID = color.ColorID";
  JoinTablesOnTraceTable.push_back(OnCondition);

  std::vector<std::vector<std::string> > ResultsQuery;
  if (TraceName == "contour") //for mesh as a collection, there is only one timepoint
    {
    JoinTable = "coordinate";
    JoinTablesOnTraceTable.push_back(JoinTable);
    OnCondition = CurrentlyUsedTraceData->CollectionName;
    OnCondition += ".";
    OnCondition += "CoordIDMin = coordinate.coordid";
    JoinTablesOnTraceTable.push_back(OnCondition);
    std::vector<std::string> WhereAndConditions;
    WhereAndConditions.push_back("mesh.ImagingsessionID");
    WhereAndConditions.push_back(ConvertToString<unsigned int>(this->m_ImgSessionID));
    WhereAndConditions.push_back("coordinate.TCoord");
    WhereAndConditions.push_back(ConvertToString<int>(this->m_SelectedTimePoint));

    ResultsQuery = GetValuesFromSeveralTables(this->m_DatabaseConnector,
                                              CurrentlyUsedTraceData->CollectionName, SelectFields, WhereAndConditions,
                                              JoinTablesOnTraceTable, true);
    }
  else
    {
    //Get the results for the query:(for trace as mesh, tracks):
    ResultsQuery  = GetValuesFromSeveralTables(this->m_DatabaseConnector,
                                               CurrentlyUsedTraceData->CollectionName, SelectFields, "ImagingSessionID",
                                               ConvertToString<unsigned int>(
                                                 this->m_ImgSessionID), JoinTablesOnTraceTable, true);
    }
  unsigned int                                     i = 0;
  std::vector<std::vector<std::string> >::iterator iter = ResultsQuery.begin();
  while (iter != ResultsQuery.end())
    {
    std::vector<std::string>       ResultsOneRow = *iter;
    int                            intRed   = atoi(ResultsOneRow[i + 1].c_str());
    int                            intGreen = atoi(ResultsOneRow[i + 2].c_str());
    int                            intBlue  = atoi(ResultsOneRow[i + 3].c_str());
    int                            intAlpha = atoi(ResultsOneRow[i + 4].c_str());
    QColor                         Color(intRed, intGreen, intBlue, intAlpha);
    ItemColorComboboxData temp;
    temp.first = ResultsOneRow[i];
    temp.second = Color;
    oListCollectionIDs.push_back(temp);
    iter++;
    }
  //for creating traces at the beginning, with no existing collection, we
  //create a new collection:
  /* if (oListCollectionIDs.empty())
     {
     int NewID = 0;
     QColor FirstColor;
     int Red = 0;
     int Green = 0;
     int Blue = 0;
     int Alpha = 0;
     GoDBTraceRow FirstCollection;
     std::string ColorName = "Default";
     ColorName += CurrentlyUsedTraceData->CollectionName;
     ColorName += "Color";
     if (CurrentlyUsedTraceData->CollectionName == "mesh")
       {
       Red   = 255;
       Green = 255;
       Blue  = 0;
       Alpha = 255;
       }
     if (CurrentlyUsedTraceData->CollectionName == "track")
       {
       Red   = 0;
       Green = 255;
       Blue  = 255;
       Alpha = 255;
       }
     if (CurrentlyUsedTraceData->CollectionName == "lineage")
       {
       Red   = 255;
       Green = 0;
       Blue  = 255;
       Alpha = 255;
       }
     FirstColor.setRed(Red);
     FirstColor.setGreen(Green);
     FirstColor.setBlue(Blue);
     FirstColor.setAlpha(Alpha);

     FirstCollection.SetColor(Red,Green,Blue,Alpha,ColorName,this->m_DatabaseConnector);
     NewID = CurrentlyUsedTraceData->CollectionOfTraces->CreateCollectionWithNoTraces(this->m_DatabaseConnector,
       FirstCollection);
     //add a new row in the table widget with the data corresponding to the new collection:
     CurrentlyUsedTraceData = this->GetTraceInfoStructure(CurrentlyUsedTraceData->CollectionName);
     this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace(CurrentlyUsedTraceData->TraceName);

     std::pair<std::string,QColor> temp;
     temp.second = FirstColor;
     temp.first = ConvertToString<int>(NewID);
     oListCollectionIDs.push_back(temp);
     }*/
  CloseDBConnection();
  return oListCollectionIDs;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::
SetListExistingCollectionIDFromDB()
{  
  this->m_TraceWidget->SetListCollectionID(this->GetListCollectionIDFromDB());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateListCollectionIDFromDB()
{
  this->m_TraceWidget->UpdateCollectionComboboxWithDeletedItem(
    this->GetListCollectionIDFromDB());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateTableWidgetAndDBWithNewCreatedTrace(
  std::string iTraceName, GoFigureMeshAttributes* iMeshAttributes)
{
  TraceInfoStructure*       CurrentlyUsedTraceData = this->GetTraceInfoStructure(iTraceName);
  GoDBTableWidgetContainer* LinkToNewTrace = CurrentlyUsedTraceData->CollectionOfTraces->
                                             GetLinkToNewCreatedTraceContainer(this->m_DatabaseConnector);

  //Update the trace table widget with the new trace:
  CurrentlyUsedTraceData->Table->setSortingEnabled(false);
  CurrentlyUsedTraceData->Table->InsertNewRow(LinkToNewTrace, iTraceName,
                                              CurrentlyUsedTraceData->CollectionName);
  CurrentlyUsedTraceData->Table->setSortingEnabled(true);

  //update the bounding box of the collection:
  //Get the ID of the last created trace in the database:
  std::list<int> ListNewTraces;
  int            NewTraceID = MaxValueForOneColumnInTable(this->m_DatabaseConnector,
                                                          CurrentlyUsedTraceData->TraceNameID, iTraceName);
  ListNewTraces.push_back(NewTraceID);
  int CollectionID = FindOneID(this->m_DatabaseConnector, iTraceName,
                               CurrentlyUsedTraceData->CollectionNameID, CurrentlyUsedTraceData->TraceNameID,
                               ConvertToString<int>(NewTraceID));
  //update the database:
  CurrentlyUsedTraceData->CollectionOfTraces->UpdateDBDataForAddedTracesToExistingCollection(
    ListNewTraces, CollectionID, this->m_DatabaseConnector);
  //update the table widget for the collection:
  this->UpdateTableWidgetForAnExistingTrace(
    CurrentlyUsedTraceData->CollectionName, CollectionID);
  //Update the table widget with calculated values:
  if (iMeshAttributes != 0)
    {
    this->PrintVolumeAreaForMesh(iMeshAttributes->m_Volume,
                                 iMeshAttributes->m_Area, NewTraceID);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoPrintDatabase::UpdateTableWidgetAndRowContainerWithNewCollectionID(
  std::string iTraceName,unsigned int iNewCollectionID,
  QColor iColorNewCollection,std::list<int> iListSelectedTraces)
{
 // unused arguments
 (void) iListSelectedTraces;

 TraceInfoStructure* TraceData = this->GetTraceInfoStructure(
   iTraceName);
 //Update the TraceTable with the new collectionID + color:
 TraceData->Table->UpdateIDs(iNewCollectionID,TraceData->CollectionNameID,
   iColorNewCollection);
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SaveNewCollectionInDB()
  //std::pair<std::string, QColor> iColorNewCollection, std::string iTraceName,
  //int iTimePoint, std::string iCellType, std::string iSubCellType)
{
  this->OpenDBConnection();
  std::string TraceName = this->m_TraceWidget->GetTraceName();
  GoDBTraceRow NewCollection;

  NewCollection.SetColor(this->m_SelectedColorData.second.red(), this->m_SelectedColorData.second.green(),
                         this->m_SelectedColorData.second.blue(),
                         this->m_SelectedColorData.second.alpha(), this->m_SelectedColorData.first,
                         this->m_DatabaseConnector);
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);

  int NewCollectionID = CurrentlyUsedTraceData->CollectionOfTraces->CreateCollectionWithNoTraces(
    this->m_DatabaseConnector, NewCollection, this->m_SelectedTimePoint);

  //update the celltype and subcelltype if the collection is a mesh:
  //to check: use the saveindb instead of the updatevalueindb:
  if (TraceName == "contour" && this->m_SelectedCellType != "" && this->m_SelectedSubCellType != "")
    {
    int CellTypeID = FindOneID(this->m_DatabaseConnector, "celltype", "CellTypeID",
      "Name", this->m_SelectedCellType);
    UpdateValueInDB(this->m_DatabaseConnector, "mesh", "CellTypeID",
                    ConvertToString<int>(CellTypeID), "MeshID", ConvertToString<int>(NewCollectionID));
    int SubCellTypeID = FindOneID(this->m_DatabaseConnector, "subcellulartype",
      "SubCellularID", "Name", this->m_SelectedSubCellType);
    UpdateValueInDB(this->m_DatabaseConnector, "mesh", "SubCellularID",
                    ConvertToString<int>(SubCellTypeID), "MeshID", ConvertToString<int>(NewCollectionID));
    }

  //update the table of the collection (which become the trace then):
  CurrentlyUsedTraceData = this->GetTraceInfoStructure(CurrentlyUsedTraceData->CollectionName);
  this->UpdateTableWidgetAndDBWithNewCreatedTrace(CurrentlyUsedTraceData->TraceName);

  this->CloseDBConnection();
  //return NewCollectionData;
  //this->SetListExistingCollectionIDFromDB();
  //update the colorcombobox and set the new collection as the selected one:
  ItemColorComboboxData NewCollectionData;
  NewCollectionData.first = ConvertToString<int>(NewCollectionID);
  NewCollectionData.second = this->m_SelectedColorData.second;
  this->m_TraceWidget->AddANewCollectionID(NewCollectionData);
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
std::pair<std::string, QColor> QGoPrintDatabase::GetSelectedCollectionData()
{
  return this->m_SelectedCollectionData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::ReEditTrace()
{
  std::string         TraceName = this->InWhichTableAreWe();
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);
  std::list<int>      SelectedTrace = CurrentlyUsedTraceData->Table->GetListCheckedTraceID();
  if (SelectedTrace.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select the %1 you want to reedit")
      .arg(TraceName.c_str()));
    msgBox.exec();
    }
  else
    {
    if (SelectedTrace.size() != 1)
      {
      QMessageBox msgBox;
      msgBox.setText(
        tr("Please select only one %1 to reedit")
        .arg(TraceName.c_str()));
      msgBox.exec();
      }
    else
      {
      TraceToReEdit(static_cast<unsigned int>(SelectedTrace.front()));
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateTableWidgetForAnExistingTrace(
  std::string iTraceName, int iTraceID,GoFigureMeshAttributes* iMeshAttributes)
{
  TraceInfoStructure*       CurrentlyUsedTraceData = this->GetTraceInfoStructure(iTraceName);
  GoDBTableWidgetContainer* LinkToUpdatedRow = CurrentlyUsedTraceData->CollectionOfTraces->
                                               GetLinkToUpdatedTraceContainer(this->m_DatabaseConnector, iTraceID);
  CurrentlyUsedTraceData->Table->UpdateRow(LinkToUpdatedRow, iTraceID, iTraceName,
                                           CurrentlyUsedTraceData->CollectionName);
  if (iMeshAttributes != 0)
    {
    this->PrintVolumeAreaForMesh(iMeshAttributes->m_Volume,
                                 iMeshAttributes->m_Area, iTraceID);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddATraceToContourMeshInfo(std::string iTraceName,
                                                  int iTraceID)
{
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure(iTraceName);

  CurrentlyUsedTraceData->ListTracesInfoForVisu->insert(
    GetTraceInfoFromDB(
      this->m_DatabaseConnector, iTraceName,
      CurrentlyUsedTraceData->CollectionName, iTraceID));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTraceInContourMeshStructure(int iTraceID,
                                                         ContourMeshStructureMultiIndexContainer* iTraceInfo)
{
  /** \todo once the info for the vizu regarding the tracks are defined,
  change it here:*/
  if (this->InWhichTableAreWe() != "track")
    {
    ContourMeshStructureMultiIndexContainer::iterator iter =
      iTraceInfo->begin();

    while (iter != iTraceInfo->end())
      {
      if (static_cast<int>(iter->TraceID) == iTraceID)
        {
        iTraceInfo->erase(iter);
        break;
        }
      ++iter;
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
ContourMeshStructureMultiIndexContainer* QGoPrintDatabase::
GetTracesInfoListForVisu(std::string iTraceName)
{
  return this->GetTraceInfoStructure(iTraceName)->ListTracesInfoForVisu;
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
void QGoPrintDatabase::UpdateInfoForTracesToBeDeletedAsCollectionOf(
  std::string iTraceName,
  std::vector<std::string> iTracesAsCollectionToBeDeleted)
{
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure(iTraceName);
  if (CurrentlyUsedTraceData->CollectionOf != "None")
    {
    //signal that all this collection are going to be deleted (for
    //the colorcombobox:
    std::vector<std::string>::iterator iterSelec =
      iTracesAsCollectionToBeDeleted.begin();
    while (iterSelec != iTracesAsCollectionToBeDeleted.end())
      {
      std::string  TraceIDstr = *iterSelec;
      unsigned int TraceID = atoi(TraceIDstr.c_str());
      //emit         DeletedCollection(TraceID);
      if(this->m_TraceWidget->GetTraceName()== iTraceName)
        {
        this->UpdateListCollectionIDFromDB();//update the colorcombobox
        }
      iterSelec++;
      }
    //Get the belonging traces for the traces that are going to be deleted
    //and are collection of these 'belonging traces':
    std::vector<std::string> ListBelongingTraces = ListSpecificValuesForOneColumn(
      this->m_DatabaseConnector, CurrentlyUsedTraceData->CollectionOf,
      CurrentlyUsedTraceData->CollectionOfID, CurrentlyUsedTraceData->TraceNameID,
      iTracesAsCollectionToBeDeleted);

    if (!ListBelongingTraces.empty())
      {
      //put the collectionid of the belonging traces to 0:
      std::list<int>                     TracesWithCollectionToBeNull;
      std::vector<std::string>::iterator it = ListBelongingTraces.begin();
      while (it != ListBelongingTraces.end())
        {
        std::string ID = *it;
        TracesWithCollectionToBeNull.push_back(atoi(ID.c_str()));
        it++;
        }
      //update everything for the collection of: exp: for contour if the trace deleted is a mesh:
      CurrentlyUsedTraceData = this->GetTraceInfoStructure(CurrentlyUsedTraceData->CollectionOf);
      CurrentlyUsedTraceData->CollectionOfTraces->UpdateCollectionIDOfSelectedTraces(
        TracesWithCollectionToBeNull, 0, this->m_DatabaseConnector);
      QColor Color(255, 255, 255, 255);
      CurrentlyUsedTraceData->Table->UpdateIDs(0, CurrentlyUsedTraceData->CollectionNameID, Color,
                                               CurrentlyUsedTraceData->TraceNameID, TracesWithCollectionToBeNull);
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTracesAsPartOfACollection(std::string iTraceName,
                                                       std::list<int> iTracesToDelete)
{
  std::list<int>::iterator iterator = iTracesToDelete.begin();
  while (iterator != iTracesToDelete.end())
    {
    TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(iTraceName);
    int                 tempTraceID = *iterator;
    int                 tempCollectionID = FindOneID(this->m_DatabaseConnector,
                                                     iTraceName,
                                                     CurrentlyUsedTraceData->CollectionNameID,
                                                     CurrentlyUsedTraceData->TraceNameID,
                                                     ConvertToString<int>(tempTraceID));
    //save the current collection of traces corresponding to the trace before recentring to the collection:
    GoDBCollectionOfTraces* CollectionOfTraces = CurrentlyUsedTraceData->CollectionOfTraces;
    CollectionOfTraces->DeleteTraceInDB(tempTraceID, this->m_DatabaseConnector);
    //change the data who were related to the trace to the collection:
    CurrentlyUsedTraceData = this->GetTraceInfoStructure(CurrentlyUsedTraceData->CollectionName);
    if (CurrentlyUsedTraceData->CollectionName != "None")
      {
      if (tempCollectionID != 0)
        {
        CollectionOfTraces->RecalculateDBBoundingBox(this->m_DatabaseConnector, tempCollectionID);
        this->UpdateTableWidgetForAnExistingTrace(CurrentlyUsedTraceData->TraceName, tempCollectionID);
        }
      }
    iterator++;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddListTracesToACollection(
  std::list<int> iListSelectedTraces, std::pair<std::string, QColor> iCollection,
  std::string iTraceName, bool IsANewCollection, int iCollectionID)
{
  if (iCollectionID == -1)
    {
    iCollectionID = atoi(iCollection.first.c_str());
    }
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure(iTraceName);
  //update the corresponding database data:
  std::list<int> ListCollectionsToUpdateInTableWidget =
    CurrentlyUsedTraceData->CollectionOfTraces->
    UpdateDBDataForAddedTracesToExistingCollection(iListSelectedTraces,
                                                   iCollectionID, this->m_DatabaseConnector);

  //update the RowContainer for traces with the new ID for the selected traces:
  //GoDBTableWidgetContainer* LinkToRowContainerForTraces =
  //CurrentlyUsedTraceData->CollectionOfTraces->GetLinkToRowContainer();
  //LinkToRowContainerForTraces->UpdateIDs(ListSelectedTraces,CollectionID);

  //update the Table Widget Display:
  CurrentlyUsedTraceData->Table->UpdateIDs(iCollectionID,
                                           CurrentlyUsedTraceData->CollectionNameID, iCollection.second,
                                           CurrentlyUsedTraceData->TraceNameID, iListSelectedTraces);
  //update the Collection Table with the new bounding box:
  CurrentlyUsedTraceData = this->GetTraceInfoStructure(CurrentlyUsedTraceData->CollectionName);
  if (IsANewCollection)
    {
    this->UpdateTableWidgetAndDBWithNewCreatedTrace(
      CurrentlyUsedTraceData->TraceName);
    }
  else
    {
    this->UpdateTableWidgetForAnExistingTrace(CurrentlyUsedTraceData->TraceName, iCollectionID);
    }

  //update the Collection Table with the old updated bounding box:
  if (!ListCollectionsToUpdateInTableWidget.empty())
    {
    std::list<int>::iterator iter = ListCollectionsToUpdateInTableWidget.begin();
    while (iter != ListCollectionsToUpdateInTableWidget.end())
      {
      int TraceID = *iter;
      this->UpdateTableWidgetForAnExistingTrace(CurrentlyUsedTraceData->TraceName, TraceID);
      iter++;
      }
    }
}
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
    this->m_BookmarkManager->GetListExistingBookmarks(this->m_DatabaseConnector);
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
void QGoPrintDatabase::SetListCellTypes(std::string iCellTypeToSelect)
{
  this->OpenDBConnection();
  NamesDescrContainerType ListCellTypes =
    this->m_CellTypeManager->GetListExistingEntities(this->m_DatabaseConnector);
  QStringList                       QListCellTypes;
  this->CloseDBConnection();
  NamesDescrContainerType::iterator iter = ListCellTypes.begin();
  while (iter != ListCellTypes.end())
    {
    QListCellTypes.append(iter->first.c_str());
    iter++;
    }
  this->m_ListCellType = QListCellTypes;
  //emit ListCellTypesToUpdate(QListCellTypes);
  this->m_TraceWidget->SetListCellTypes(QListCellTypes);
  if (!iCellTypeToSelect.empty())
    {
    this->m_TraceWidget->SetCurrentCellType(iCellTypeToSelect);
    }
  
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddNewCellType()
{
  this->OpenDBConnection();
  std::string NewCellType = this->m_CellTypeManager->AddAnEntity(
    this->m_DatabaseConnector);
  this->SetListCellTypes(NewCellType);
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteCellType()
{
  this->OpenDBConnection();
  this->m_CellTypeManager->DeleteEntity(this->m_DatabaseConnector);
  //get the last selected cell type, which is the one stored in m_SelectedCellType
  //as the selected one:
  this->SetListCellTypes(this->m_SelectedCellType);
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetListSubCellTypes(std::string iNewSubCellType)
{
  this->OpenDBConnection();
  NamesDescrContainerType ListSubCellTypes =
    this->m_SubCellTypeManager->GetListExistingEntities(this->m_DatabaseConnector);
  this->CloseDBConnection();
  QStringList                       QListSubCellTypes;
  NamesDescrContainerType::iterator iter = ListSubCellTypes.begin();
  while (iter != ListSubCellTypes.end())
    {
    QListSubCellTypes.append(iter->first.c_str());
    iter++;
    }
  //this->m_ListSubCellType = QListSubCellTypes;
  this->m_TraceWidget->SetListSubCellTypes(QListSubCellTypes);
  //emit ListSubCellTypesToUpdate(QListSubCellTypes);
  if (!iNewSubCellType.empty())
    {
    this->m_TraceWidget->SetCurrentSubCellType(iNewSubCellType);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddNewSubCellType()
{
  this->OpenDBConnection();
  std::string NewSubCellType = 
    this->m_SubCellTypeManager->AddAnEntity(this->m_DatabaseConnector);
  this->SetListSubCellTypes(NewSubCellType);
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteSubCellType()
{
  this->OpenDBConnection();
  this->m_SubCellTypeManager->DeleteEntity(this->m_DatabaseConnector);
  this->SetListSubCellTypes(this->m_SelectedSubCellType);
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::CheckSelectedRows()
{
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure(this->InWhichTableAreWe());
  CurrentlyUsedTraceData->Table->CheckSelectedRows(CurrentlyUsedTraceData->TraceName,
                                                   CurrentlyUsedTraceData->TraceNameID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UncheckSelectedRows()
{
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure(this->InWhichTableAreWe());
  CurrentlyUsedTraceData->Table->UncheckSelectedRows(CurrentlyUsedTraceData->TraceName,
                                                     CurrentlyUsedTraceData->TraceNameID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::ShowSelectedRows()
{
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure(this->InWhichTableAreWe());
  CurrentlyUsedTraceData->Table->ShowSelectedRows(CurrentlyUsedTraceData->TraceName,
                                                  CurrentlyUsedTraceData->TraceNameID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::HideSelectedRows()
{
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure(this->InWhichTableAreWe());
  CurrentlyUsedTraceData->Table->HideSelectedRows(CurrentlyUsedTraceData->TraceName,
                                                  CurrentlyUsedTraceData->TraceNameID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoPrintDatabase::UpdateListMeshes(int iTimePoint)
{
  this->SetListExistingCollectionIDFromDB();
  //emit PrintExistingCollectionIDsFromDB(
    //this->GetListExistingCollectionIDFromDB("contour", iTimePoint));
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddTracesInTableWidgetFromDB(std::vector<int> ListTracesIDs,
                                                    std::string iTraceName)
{
  TraceInfoStructure* CurrentlyUsedTraceData =
    this->GetTraceInfoStructure(iTraceName);
  this->OpenDBConnection();
  for (unsigned int i = 0; i < ListTracesIDs.size(); i++)
    {
    GoDBTableWidgetContainer* LinkToNewTrace = CurrentlyUsedTraceData->CollectionOfTraces->
                                               GetLinkToNewCreatedTraceContainer(this->m_DatabaseConnector,
                                                                                 ListTracesIDs.at(i));
    CurrentlyUsedTraceData->Table->InsertNewRow(LinkToNewTrace, iTraceName,
                                                CurrentlyUsedTraceData->CollectionName);
    }
  this->CloseDBConnection();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*QStringList QGoPrintDatabase::GetQStringListCellTypes()
{
  return this->m_ListCellType;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*QStringList QGoPrintDatabase::GetQStringListSubCellTypes()
{
  return this->m_ListSubCellType;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoPrintDatabase::UpdateSelectedCellTypeAndSubCellType(
  std::string iSelectedCellType, std::string iSelectedSubCellType)
{
  this->m_SelectedCellType = iSelectedCellType;
  this->m_SelectedSubCellType = iSelectedSubCellType;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateSelectedTimePoint(int iTimePoint)
{
  this->m_SelectedTimePoint = iTimePoint;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoPrintDatabase::GetNameNewCellType()
{
  return this->m_CellTypeManager->GetNameNewEntity();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string QGoPrintDatabase::GetNameNewSubCellType()
{
  return this->m_SubCellTypeManager->GetNameNewEntity();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::TheTabIsChanged(int iIndex)
{
  TraceInfoStructure* CurrentlyUsedTraceData = new TraceInfoStructure;
  switch (iIndex)
    {
    case 1:
      CurrentlyUsedTraceData = this->GetTraceInfoStructure("mesh");
      break;
    case 2:
      CurrentlyUsedTraceData = this->GetTraceInfoStructure("track");
      break;
    case 3:
      CurrentlyUsedTraceData = this->GetTraceInfoStructure("lineage");
      break;
    default:
      CurrentlyUsedTraceData = this->GetTraceInfoStructure("contour");
      break;
    }

  emit TableWidgetTableChanged(CurrentlyUsedTraceData->TraceName,
                               CurrentlyUsedTraceData->CollectionName);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetTable(std::string iTablename)
{
  int Index = 0;
  if (iTablename == "mesh")
    {
    Index = 1;
    }
  if (iTablename == "track")
    {
    Index = 2;
    }
  if (iTablename == "lineage")
    {
    Index = 3;
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
ContourMeshStructureMultiIndexContainer* QGoPrintDatabase::
ImportContours(int iTimePoint)
{
  ContourMeshStructureMultiIndexContainer* ContoursForVisu = NULL;
  QString                                  p = QFileDialog::getOpenFileName(this,
                                                                            tr("Open Contour Export File"), "",
                                                                            tr("TextFile (*.txt)"));
  if (!p.isNull())
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
  return ContoursForVisu;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
ContourMeshStructureMultiIndexContainer* QGoPrintDatabase::
ImportMeshes(int iTimePoint)
{
  ContourMeshStructureMultiIndexContainer* MeshesForVisu = NULL;
  QString                                  p = QFileDialog::getOpenFileName(this,
                                                                            tr("Open Contour Export File"), "",
                                                                            tr("TextFile (*.txt)"));
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
  return MeshesForVisu;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::PrintVolumeAreaForMesh(double iVolume,
                                              double iArea, unsigned int iMeshID)
{
  std::vector<std::string> ColumnNames (2);
  std::vector<std::string> Values (2);
  ColumnNames.at(0) = "SurfaceArea";
  Values.at(0) = ConvertToString<double>(iArea);
  ColumnNames.at(1) = "Volume";
  Values.at(1) = ConvertToString<double>(iVolume);
  this->m_MeshesData->Table->AddValuesForID(ColumnNames, Values, iMeshID,
                                            "meshID");
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
                   SLOT(this->UpdateSelectedCellType(std::string)));

  QObject::connect(this->m_TraceWidget,
                   SIGNAL(NewSubCellTypeActivated(std::string)),
                   this,
                   SLOT(this->UpdateSelectedSubCellType(std::string)));

  QObject::connect(this->m_TraceWidget,
                   SIGNAL(NewColorToBeSaved(ItemColorComboboxData)),
                   this,
                   SLOT(SaveNewColorInDB(ItemColorComboboxData)));

  QObject::connect(this->m_TraceWidget,
                   SIGNAL(NewCollectionToBeCreated()),
                   this, 
                   SLOT(SaveNewCollectionInDB()));

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
   //this->m_TraceManualEditingDockWidget->ShowAndUpdate("contour", "mesh");
   this->m_TraceWidget->UpdateTraceAndCollection(iTraceName, iCollectionName);
   this->SetListExistingCollectionIDFromDB();
   if(UpdateTableWidget)
     {
     /** \todo check if it is still needed to block the signals*/
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
  this->SetColorComboBoxInfofromDB();
  this->SetListCellTypes();
  this->SetListSubCellTypes();
}