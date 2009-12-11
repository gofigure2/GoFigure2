/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Revision: 602 $  // Revision of last commit
  Date: $Date: 2009-08-27 07:47:13 -0400 (Thu, 27 Aug 2009) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QMenu>
#include <QCloseEvent>

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



//--------------------------------------------------------------------------
QGoPrintDatabase::
QGoPrintDatabase( QWidget* iParent ) :
  QWidget( iParent ),
  m_DatabaseConnector( 0 ),
  m_IsDatabaseUsed( false )
{
  this->setupUi( this );

  DBTabWidget->setTabPosition( QTabWidget::West );
  DBTabWidget->setTabShape( QTabWidget::Triangular );
  DBTabWidget->removeTab( 0 );

  ContourTable = new QTableWidgetChild( this );
  MeshTable = new QTableWidgetChild( this );
  TrackTable = new QTableWidgetChild( this );
  LineageTable = new QTableWidgetChild( this );

  m_CollectionOfContours = new GoDBCollectionOfTraces( "mesh","contour");
  m_CollectionOfMeshes = new GoDBCollectionOfTraces( "track","mesh");
  m_CollectionOfTracks = new GoDBCollectionOfTraces( "lineage","track");
  m_CollectionOfLineages = new GoDBCollectionOfTraces("None","lineage");

  this->setContextMenuPolicy( Qt::CustomContextMenu );

  m_VisibilityAction = new QAction( this );
  m_VisibilityAction->setCheckable( true );

  QObject::connect( m_VisibilityAction, SIGNAL( toggled( bool ) ),
    this, SLOT( setVisible( bool ) ) );

  QObject::connect( this, SIGNAL( customContextMenuRequested( const QPoint & ) ),
    this, SLOT( CreateContextMenu( const QPoint & ) ) );

 // QObject::connect( this->ContourTable, SIGNAL(itemSelectionChanged()),
 //   this, SLOT(ChangeTracesToHighLightInfoFromTableWidget()));

 // QObject::connect( this->MeshTable, SIGNAL(itemSelectionChanged()),
 //   this, SLOT(ChangeTracesToHighLightInfoFromTableWidget()));
  QObject::connect(this->ContourTable, SIGNAL (CheckedRowsChanged()),
      this, SLOT(ChangeTracesToHighLightInfoFromTableWidget()));

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoPrintDatabase::~QGoPrintDatabase()
{
  // Need to release memory allocated for contours, meshes and tracks
  // contours
  std::vector<ContourMeshStructure>::iterator c_it = m_ContoursInfo.begin();

  while( c_it != m_ContoursInfo.end() )
    {
    // nodes
    if( c_it->Nodes )
      {
      c_it->Nodes->Delete();
      c_it->Nodes = 0;
      }
    // used actor for the visualization
    if( c_it->Actor )
      {
      c_it->Actor->Delete();
      c_it->Actor = 0;
      }
    ++c_it;
    }

  // meshes
  std::vector<ContourMeshStructure>::iterator m_it = m_MeshesInfo.begin();

  while( m_it != m_MeshesInfo.end() )
    {
    // polydata
    if( m_it->Nodes )
      {
      m_it->Nodes->Delete();
      m_it->Nodes = 0;
      }
    // used actor for the visualization
    if( m_it->Actor )
      {
      m_it->Actor->Delete();
      m_it->Actor = 0;
      }
    ++m_it;
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
  const unsigned int& iImgSessionID, const std::string& iImgSessionName )
{
  m_Server         = iServer;
  m_User           = iUser;
  m_Password       = iPassword;
  m_DBName         = iNameDB;
  m_ImgSessionID   = iImgSessionID;
  m_ImgSessionName = iImgSessionName;
  m_CollectionOfContours ->SetImgSessionID(m_ImgSessionID);
  m_CollectionOfMeshes   ->SetImgSessionID(m_ImgSessionID);
  m_CollectionOfTracks   ->SetImgSessionID(m_ImgSessionID);
  m_CollectionOfLineages ->SetImgSessionID(m_ImgSessionID);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::OpenDBConnection()
{
  if (m_DatabaseConnector == 0)
    {
    m_DatabaseConnector = OpenDatabaseConnection(m_Server,m_User,
      m_Password,m_DBName);
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
void QGoPrintDatabase::FillTableFromDatabase()
{
  OpenDBConnection();

  QString title = QString( "Imaging Session: %1 " ).arg( m_ImgSessionName.c_str() );
  this->setWindowTitle( title );
  m_VisibilityAction->setText( title );

  GetContentAndDisplayFromDB( "contour", ContourTable, m_CollectionOfContours);
  GetContentAndDisplayFromDB( "mesh",    MeshTable,    m_CollectionOfMeshes);
  GetContentAndDisplayFromDB( "track",   TrackTable,   m_CollectionOfTracks);
  GetContentAndDisplayFromDB( "lineage", LineageTable, m_CollectionOfLineages);

  LoadContoursAndMeshesFromDB(m_DatabaseConnector);

  CloseDBConnection();
  m_IsDatabaseUsed = true;
  FillDatabaseFinished();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::closeEvent(QCloseEvent* iEvent)
{
  m_VisibilityAction->setChecked( false );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::UpdateTableFromDB()
{
  //todo: make it ok for all tables
  //UpdateContentAndDisplayFromDB< GoDBContourRow >("contour",
   // ContourTable,m_DatabaseConnector);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateContextMenu(const QPoint &iPos)
{
  QMenu* ContextMenu = new QMenu;
  NeedCurrentSelectedCollectionID();
  std::string TraceName = this->InWhichTableAreWe();
  std::string CollectionName = this->GetCollectionOfTraces(TraceName)->CollectionName();
  ContextMenu->addAction(tr("Delete selected %1s").arg(TraceName.c_str()),
    this,SLOT(DeleteTraces()));
  ContextMenu->addAction(tr("Create a new %1").arg(CollectionName.c_str()),
    this,SLOT(CreateCorrespondingCollection()));
  ContextMenu->addAction(
    tr("Add to selected %1 : %2").arg(CollectionName.c_str())
    .arg(this->m_CurrentCollectionData.first.c_str()),this,SLOT(AddToSelectedCollection()));
  ContextMenu->addAction(tr("ReEdit the selected %1").arg(TraceName.c_str()),
    this,SLOT(ReEditTrace()));
  ContextMenu->exec(this->mapToGlobal(iPos));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTraces()
{
  std::string TraceName = this->InWhichTableAreWe();
  QTableWidgetChild* Table = this->GetTableWidgetChild(TraceName);
  GoDBCollectionOfTraces* CollectionOfTraces = this->GetCollectionOfTraces(TraceName);
  std::list<int> SelectedTraces = Table->GetListCheckedTraceID();
  if(SelectedTraces.empty())
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
                 "permanently the selected %1s?").arg(CollectionOfTraces->GetTraceName().c_str()),
              QMessageBox::Yes,
              QMessageBox::No|QMessageBox::Default);
    if (r == QMessageBox::Yes)
      {
      OpenDBConnection();
      //check that the traces are not collection of existing traces:
      std::string TraceID = TraceName;
      TraceID += "ID";
      std::string CollectionOf = CollectionOfTraces->GetCollectionOf();
      std::string CollectionOfID = CollectionOf;
      CollectionOfID += "ID";

      std::vector<std::string> VectorValues;
      std::list<int>::iterator iter = SelectedTraces.begin();
      while(iter != SelectedTraces.end())
        {
        int ID = *iter;
        VectorValues.push_back(ConvertToString<int>(ID));
        iter++;
        }
      if (!CollectionOf.empty())
        {
        std::vector<std::string> ListBelongingTraces = ListSpecificValuesForOneColumn(
          this->m_DatabaseConnector,CollectionOf,CollectionOfID,TraceID,VectorValues);

        if (!ListBelongingTraces.empty())
          {
          /** \todo put the collectionid of the belonging traces to 0/null*/
          //put the collectionid of the belonging traces to 0:
          std::list<int> TracesWithCollectionToBeNull;
          std::vector<std::string>::iterator it = ListBelongingTraces.begin();
          while (it != ListBelongingTraces.end())
            {
            std::string ID = *it;
            TracesWithCollectionToBeNull.push_back(atoi(ID.c_str()));
            it++;
            }
          GoDBCollectionOfTraces* TracesCollectionOf = this->GetCollectionOfTraces(CollectionOf);
          TracesCollectionOf->AddSelectedTracesToCollection(TracesWithCollectionToBeNull,0,
            this->m_DatabaseConnector);
          QTableWidgetChild* TableCollectionOf= this->GetTableWidgetChild(TracesCollectionOf->GetTraceName());
          QColor Color(255,255,255,255);
          std::string CollectionIDName = TracesCollectionOf->GetCollectionName();
          CollectionIDName += "ID";
          std::string TraceIDName = TracesCollectionOf->GetTraceName();
          TraceIDName += "ID";
          TableCollectionOf->UpdateIDs(0,CollectionIDName,Color,TraceIDName,TracesWithCollectionToBeNull);
          }
        }
        
      //delete traces in the database:
      CollectionOfTraces->DeleteTraces(SelectedTraces,this->m_DatabaseConnector);
      //delete traces in the row container:
      GoDBTableWidgetContainer* LinkToTracesContainer = CollectionOfTraces->GetLinkToRowContainer();
      LinkToTracesContainer->DeleteSelectedTraces(SelectedTraces);
      //delete traces in the table widget with the vector of selected traces:
      Table->DeleteSelectedRows();
      //delete collectionID in the colorcollectionid combobox:
      std::list<int>::iterator iterSelec = SelectedTraces.begin();
      while(iterSelec != SelectedTraces.end())
        {
        int TraceID = *iterSelec;
        this->m_CurrentCollectionData.first = ConvertToString<int>(TraceID);
        DeletedCollection();
        iterSelec++;
        }
 
      CloseDBConnection();
      }
    }




  //int TabIndex = InWhichTableAreWe();

 /* switch (TabIndex)
    {
    case 0: //contour
      {
      //add the tableWidgetChild in the CollectionOfTraces?
      QStringList ContoursToDelete = this->ContourTable->ValuesForSelectedRows("ContourID");
      m_CollectionOfContours->DeleteTraces(ContoursToDelete,m_DatabaseConnector);
      //this->UpdateContentAndDisplayFromDB<GoDBContourRow>("contour",
      //  ContourTable,m_DatabaseConnector);
      break;
      }
    case 1: //mesh
      {
      QStringList MeshesToDelete = this->MeshTable->ValuesForSelectedRows("MeshID");
      m_CollectionOfMeshes->DeleteTraces(MeshesToDelete,m_DatabaseConnector);
     // this->UpdateContentAndDisplayFromDB<GoDBMeshRow>("mesh",
      //  MeshTable,m_DatabaseConnector);
      break;
      }
    case 2: //track
      {
      QStringList TracksToDelete = this->TrackTable->ValuesForSelectedRows("TrackID");
        m_CollectionOfTracks->DeleteTraces(TracksToDelete,m_DatabaseConnector);
      //this->UpdateContentAndDisplayFromDB<GoDBTrackRow>("track",
     //   TrackTable,m_DatabaseConnector);
      break;
      }
    case 3: //lineage
      {
      QStringList LineagesToDelete = this->LineageTable->ValuesForSelectedRows("LineageID");
      m_CollectionOfLineages->DeleteTraces(LineagesToDelete,m_DatabaseConnector);
     // this->UpdateContentAndDisplayFromDB<GoDBLineageRow>("lineage",
     //   LineageTable,m_DatabaseConnector);
      break;
      }
    default:
      {
      std::cout<<"error, tab doesn't exist";
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      break;
      }
    }*/
  CloseDBConnection();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateCorrespondingCollection()
{
  //Get all the needed data:
  std::string TraceName = InWhichTableAreWe();
  GoDBCollectionOfTraces* CollectionOfTraces = this->GetCollectionOfTraces(TraceName);
  std::string TraceNameID = CollectionOfTraces->TracesName();
  QTableWidgetChild* TraceTable = this->GetTableWidgetChild(TraceName);

  //Get the list of IDs in the row checked by the user:
  std::list<int> ListSelectedTraces = TraceTable->GetListCheckedTraceID();
  if (ListSelectedTraces.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1 to be part of the new %2")
      .arg(CollectionOfTraces->GetTraceName().c_str() )
      .arg(CollectionOfTraces->GetCollectionName().c_str()));
    msgBox.exec();
    }
  else
    {
    OpenDBConnection();
    //set the color for the new collection:
    GoDBTraceRow NewCollection;
    NeedToGetCurrentSelectedColor();
    NewCollection.SetColor(this->m_CurrentColorData.second.red(),this->m_CurrentColorData.second.green(),
      this->m_CurrentColorData.second.blue(),this->m_CurrentColorData.second.alpha(),
      this->m_CurrentColorData.first,this->m_DatabaseConnector);
    //create the collection in the database and get the corresponding ID:

    int NewCollectionID = CollectionOfTraces->CreateNewCollectionFromSelection(
      ListSelectedTraces,this->m_DatabaseConnector,NewCollection);

    //update the Collection Table and the row container with the new created Collection:
    QTableWidgetChild* CollectionTable = this->GetTableWidgetChild(CollectionOfTraces->CollectionName());
    this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace(CollectionTable,this->m_DatabaseConnector,
      this->GetCollectionOfTraces(CollectionOfTraces->CollectionName()));

    //update the Trace Table and the row containerwith the new Collection ID:
    this->UpdateTableWidgetAndRowContainerWithNewCollectionID(TraceTable,this->m_DatabaseConnector,
      CollectionOfTraces,NewCollectionID,this->m_CurrentColorData.second,ListSelectedTraces);

    CloseDBConnection();
    this->m_LastCreatedCollection.first = ConvertToString<int>(NewCollectionID);
    this->m_LastCreatedCollection.second = this->m_CurrentColorData.second;

    NewCreatedCollection();
    }

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string QGoPrintDatabase::InWhichTableAreWe ()
{
  int CurrentIndex = this->DBTabWidget->currentIndex();
  return this->DBTabWidget->tabText(CurrentIndex).toStdString();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::AddToSelectedCollection()
{
  std::string TraceName = this->InWhichTableAreWe();
    QTableWidgetChild* Table = this->GetTableWidgetChild(TraceName);
  std::list<int> ListSelectedTraces = Table->GetListCheckedTraceID();
  GoDBCollectionOfTraces* CollectionOfTraces = this->GetCollectionOfTraces(TraceName);
  NeedCurrentSelectedCollectionID();

  if (ListSelectedTraces.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1 to be part of the %2 %3")
      .arg(CollectionOfTraces->GetTraceName().c_str() )
      .arg(CollectionOfTraces->GetCollectionName().c_str() )
      .arg(this->m_CurrentCollectionData.first.c_str() ) );
    msgBox.exec();
    }
  else
    {
    OpenDBConnection();

    int CollectionID = atoi(this->m_CurrentCollectionData.first.c_str());
    QColor ColorCollection = this->m_CurrentCollectionData.second;


    GoDBTableWidgetContainer* LinkToRowContainerForTraces =
      CollectionOfTraces->GetLinkToRowContainer();

    //update the collectionID of the traces in the database:
    CollectionOfTraces->AddSelectedTracesToCollection(ListSelectedTraces,
      CollectionID,this->m_DatabaseConnector);
    //update the RowContainer for traces with the new ID for the selected traces:
    LinkToRowContainerForTraces->UpdateIDs(ListSelectedTraces,CollectionID);

    std::string CollectionIDName = CollectionOfTraces->GetCollectionName();
    CollectionIDName += "ID";
    //update the Table Widget Display:
    Table->UpdateIDs(CollectionID,CollectionIDName,ColorCollection);

    CloseDBConnection();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::LoadContoursAndMeshesFromDB(
  vtkMySQLDatabase* DatabaseConnector)
{
  m_ContoursInfo = GetTracesInfoFromDB(DatabaseConnector,"contour","mesh",
    m_ImgSessionID);
  m_MeshesInfo   = GetTracesInfoFromDB(DatabaseConnector,"mesh","track",
    m_ImgSessionID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::
ChangeTracesToHighLightInfoFromTableWidget()
{
  std::string TraceName = this->InWhichTableAreWe();
  QTableWidgetChild* Table = this->GetTableWidgetChild(TraceName);
  if ( TraceName == "contour")
    {
    Table->TracesToHighlight(TraceName,this->m_ContoursInfo);
    emit SelectionContoursToHighLightChanged();
    }
  if (TraceName == "mesh")
    {
    Table->TracesToHighlight(TraceName,this->m_MeshesInfo);
    emit SelectionMeshesToHighLightChanged();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::
ChangeContoursToHighLightInfoFromVisu(
  std::list<int> iListContoursHighLightedInVisu )
{
  /** \todo get the tracename from the visu dock widget*/
  QTableWidgetChild* Table = this->GetTableWidgetChild("contour");

  if( !iListContoursHighLightedInVisu.empty() )
    {
    std::list<int>::iterator it = iListContoursHighLightedInVisu.begin();

    while( it != iListContoursHighLightedInVisu.end() )
      {
      for( unsigned int j = 0 ; j < m_ContoursInfo.size(); j++ )
        {
        if (*it == this->m_ContoursInfo[j].TraceID)
          {
          m_ContoursInfo[j].Highlighted = true;

          Table->SetSelectRowTraceID( "contour",
            m_ContoursInfo[j].TraceID, true );
          }
        else
          {
          m_ContoursInfo[j].Highlighted = false;

          Table->SetSelectRowTraceID( "contour",
            m_ContoursInfo[j].TraceID, false);
          }
        }
      ++it;
      }
    }
  else
    {
    for( unsigned int j = 0 ; j < m_ContoursInfo.size(); j++ )
      {
      m_ContoursInfo[j].Highlighted = false;

      Table->SetSelectRowTraceID( "contour",
        m_ContoursInfo[j].TraceID, false);
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
  QTableWidgetChild* Table = this->GetTableWidgetChild("mesh");

  while( it != iListMeshesHighLightedInVisu.end() )
    {
    for( unsigned int j = 0 ; j < m_MeshesInfo.size(); j++ )
      {
      if (*it == this->m_MeshesInfo[j].TraceID)
        {
        m_MeshesInfo[j].Highlighted = true;

        Table->SetSelectRowTraceID( "mesh",
          m_MeshesInfo[j].TraceID, true );
        }
      else
        {
        m_MeshesInfo[j].Highlighted = false;

        Table->SetSelectRowTraceID( "mesh",
          m_MeshesInfo[j].TraceID, false);
        }
      ++it;
      }
    }
 }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoPrintDatabase::SaveContoursFromVisuInDB(unsigned int iXCoordMin,
  unsigned int iYCoordMin,unsigned int iZCoordMin,unsigned int iTCoord,
  unsigned int iXCoordMax,unsigned int iYCoordMax,unsigned int iZCoordMax,
  vtkPolyData* iContourNodes, std::pair<std::string,QColor> iColorData,
  unsigned int iMeshID)
{
  OpenDBConnection();

  GoDBCoordinateRow coord_min;
  coord_min.SetField< unsigned int >( "XCoord", iXCoordMin );
  coord_min.SetField< unsigned int >( "YCoord", iYCoordMin );
  coord_min.SetField< unsigned int >( "ZCoord", iZCoordMin );
  coord_min.SetField< unsigned int >( "TCoord", iTCoord );

  GoDBCoordinateRow coord_max;
  coord_max.SetField< unsigned int >( "XCoord", iXCoordMax );
  coord_max.SetField< unsigned int >( "YCoord", iYCoordMax );
  coord_max.SetField< unsigned int >( "ZCoord", iZCoordMax );
  coord_max.SetField< unsigned int >( "TCoord", iTCoord );

  GoDBContourRow contour_row( this->m_DatabaseConnector,coord_min, coord_max,
    this->m_ImgSessionID, iContourNodes );
  contour_row.SetColor(iColorData.second.red(),iColorData.second.green(),
    iColorData.second.blue(),iColorData.second.alpha(),iColorData.first,
    this->m_DatabaseConnector);

  contour_row.SetCollectionID(iMeshID);

  int NewContourID = contour_row.SaveInDB( this->m_DatabaseConnector);

  this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace(this->ContourTable,
    this->m_DatabaseConnector,this->m_CollectionOfContours);

  CloseDBConnection();

  return NewContourID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoPrintDatabase::SaveContoursFromVisu(unsigned int iXCoordMin,
    unsigned int iYCoordMin, unsigned int iZCoordMin, unsigned int iTCoord,
    unsigned int iXCoordMax, unsigned int iYCoordMax, unsigned int iZCoordMax,
    vtkPolyData* iContourNodes, std::pair<std::string, QColor> iColorData,
    unsigned int iMeshID,int ContourID )
{
  OpenDBConnection();

  GoDBCoordinateRow coord_min;
  coord_min.SetField< unsigned int >( "XCoord", iXCoordMin );
  coord_min.SetField< unsigned int >( "YCoord", iYCoordMin );
  coord_min.SetField< unsigned int >( "ZCoord", iZCoordMin );
  coord_min.SetField< unsigned int >( "TCoord", iTCoord );

  GoDBCoordinateRow coord_max;
  coord_max.SetField< unsigned int >( "XCoord", iXCoordMax );
  coord_max.SetField< unsigned int >( "YCoord", iYCoordMax );
  coord_max.SetField< unsigned int >( "ZCoord", iZCoordMax );
  coord_max.SetField< unsigned int >( "TCoord", iTCoord );

  GoDBContourRow contour_row( this->m_DatabaseConnector,coord_min, coord_max,
    this->m_ImgSessionID, iContourNodes );
  contour_row.SetColor(iColorData.second.red(),iColorData.second.green(),
    iColorData.second.blue(),iColorData.second.alpha(),iColorData.first,
    this->m_DatabaseConnector);

  contour_row.SetCollectionID(iMeshID);
  

  UpdateContourInDB(this->m_DatabaseConnector,contour_row);
  
  /** \todo update the table widget and the row container*/
  //this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace(this->ContourTable,
    //this->m_DatabaseConnector,this->m_CollectionOfContours);

  CloseDBConnection();

  return ContourID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::GetContentAndDisplayFromDB( QString TableName,
  QTableWidgetChild* Table, GoDBCollectionOfTraces* iCollectionOfTraces)
{
  //Get the column names to be displayed in the table widget:
  std::list<std::string> ColumnsNames =
      iCollectionOfTraces->GetListColumnsNamesForTableWidget();
  Table->DisplayColumnNames( TableName, ColumnsNames);
  this->DBTabWidget->addTab(Table,TableName);
  //Get all the necessary data from the database:
  DBTableWidgetContainerType Row_Container =
    iCollectionOfTraces->GetRowContainer(m_DatabaseConnector);

  Table->DisplayContent( iCollectionOfTraces->GetLinkToRowContainer(),
    iCollectionOfTraces->GetTraceName(),
    iCollectionOfTraces->GetCollectionName() );
  Table->setSortingEnabled(true);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure> QGoPrintDatabase::
  GetContoursForAGivenTimepoint(unsigned int iTimePoint)
{
  return GetTracesForAGivenTimepoint(this->m_ContoursInfo, iTimePoint);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure> QGoPrintDatabase::
  GetMeshesForAGivenTimepoint (unsigned int iTimePoint)
{
  return GetTracesForAGivenTimepoint(this->m_MeshesInfo, iTimePoint);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure> QGoPrintDatabase::
  GetTracesForAGivenTimepoint(std::vector<ContourMeshStructure> iAllTraces,
  unsigned int iTimePoint)
{
  std::vector<ContourMeshStructure> SelectedTraces;
  std::vector<ContourMeshStructure>::iterator iter = iAllTraces.begin();

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
  GetContoursForAGivenZCoord (unsigned int iZCoord)
{
  return this->GetTracesForAGivenZCoord(this->m_ContoursInfo,iZCoord,
    this->m_CollectionOfContours);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure> QGoPrintDatabase::
  GetMeshesForAGivenZCoord (unsigned int iZCoord)
{
  return this->GetTracesForAGivenZCoord(this->m_MeshesInfo,iZCoord,
    this->m_CollectionOfTracks);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure> QGoPrintDatabase::
  GetTracesForAGivenZCoord(std::vector<ContourMeshStructure> iAllTraces,
  unsigned int iZCoord, GoDBCollectionOfTraces* iCollectionOfTraces)
{
  std::vector<ContourMeshStructure> oSelectedTraces;
  GoDBTableWidgetContainer* LinkToRowContainer =
    iCollectionOfTraces->GetLinkToRowContainer();
  std::list<std::string> ListOfSelectedTracesID =
    LinkToRowContainer->GetTracesIDForAGivenZCoord(iZCoord);

  std::vector<ContourMeshStructure>::iterator iterAllTraces = iAllTraces.begin();
  std::list<std::string>::iterator iterTracesID = ListOfSelectedTracesID.begin();

  while (iterTracesID != ListOfSelectedTracesID.end())
    {
    std::string TraceIDToFindStrg = *iterTracesID;
    unsigned int TraceIDToFind =
      static_cast< unsigned int >( atoi( TraceIDToFindStrg.c_str() ) );
    bool found = false;
    while( ( iterAllTraces != iAllTraces.end() ) && (found == false) )
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
std::list<std::pair<std::string,std::vector<int> > > QGoPrintDatabase::
  GetColorComboBoxInfofromDB()
{
  OpenDBConnection();

  std::list<std::pair<std::string,std::vector<int> > > oInfoColors;
  std::vector<std::string> ResultsQuery  = ListAllValuesForOneColumn(
    m_DatabaseConnector,"*", "color");
  unsigned int i = 0;
  while ( i<ResultsQuery.size())
    {
    std::pair<std::string,std::vector<int> > temp;
    temp.first = ResultsQuery[i+1];
    std::string Red = ResultsQuery[i+2];
    temp.second.push_back(atoi(Red.c_str()));
    temp.second.push_back(atoi(ResultsQuery[i+3].c_str()));
    temp.second.push_back(atoi(ResultsQuery[i+4].c_str()));
    temp.second.push_back(atoi(ResultsQuery[i+5].c_str()));
    oInfoColors.push_back(temp);
    i = i+7;
    }

  CloseDBConnection();

  return oInfoColors;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::
SaveNewColorInDB(std::vector<std::string> iDataNewColor)
{
  this->OpenDBConnection();
  GoDBColorRow NewColor;
  if (iDataNewColor.size() != 5)
    {
    std::cout<<"Pb: the number of data for the new color is not 5";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
  else
    {
    NewColor.SetField("Name",iDataNewColor[0]);
    NewColor.SetField("Red",iDataNewColor[1]);
    NewColor.SetField("Green",iDataNewColor[2]);
    NewColor.SetField("Blue",iDataNewColor[3]);
    NewColor.SetField("Alpha",iDataNewColor[4]);
    NewColor.SaveInDB(m_DatabaseConnector);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoPrintDatabase::IsDatabaseUsed()
{
  return m_IsDatabaseUsed;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<std::pair<std::string,QColor> > QGoPrintDatabase::
  GetListExistingCollectionIDFromDB(std::string TraceName,
  std::string CollectionName)
{
  OpenDBConnection();
  std::list<std::pair<std::string, QColor> > oListCollectionIDs;
  //First, build the query with selected fields and table to join with on conditions:
  std::vector<std::string> SelectFields;
  std::string CollectionID = CollectionName;
  CollectionID += ".";
  CollectionID += CollectionName;
  CollectionID += "ID";
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
  std::string JoinTable = "color";
  JoinTablesOnTraceTable.push_back(JoinTable);
  std::string OnCondition = CollectionName;
  OnCondition += ".ColorID = color.ColorID";
   JoinTablesOnTraceTable.push_back(OnCondition);

 //Get the results for the query:
  std::vector<std::vector<std::string> >ResultsQuery  = GetValuesFromSeveralTables(
    this->m_DatabaseConnector,CollectionName,SelectFields, "ImagingSessionID",
    ConvertToString<unsigned int>(this->m_ImgSessionID),JoinTablesOnTraceTable,true);

  unsigned int i = 0;
  std::vector<std::vector<std::string> >::iterator iter = ResultsQuery.begin();
  while ( iter != ResultsQuery.end())
    {
    std::vector<std::string> ResultsOneRow = *iter;
    int Red   = atoi(ResultsOneRow[i+1].c_str());
    int Green = atoi(ResultsOneRow[i+2].c_str());
    int Blue  = atoi(ResultsOneRow[i+3].c_str());
    int Alpha = atoi(ResultsOneRow[i+4].c_str());
    QColor Color(Red,Green,Blue,Alpha);
    std::pair<std::string,QColor> temp;
    temp.first = ResultsOneRow[i];
    temp.second = Color;
    oListCollectionIDs.push_back(temp);
    iter++;
    }
  //for creating traces at the beginning, with no existing collection, we
  //create a new collection:
  if (oListCollectionIDs.empty())
    {
    int NewID = 0;
    QColor FirstColor;
    int Red = 0;
    int Green = 0;
    int Blue = 0;
    int Alpha = 0;
    GoDBTraceRow FirstCollection;
    std::string ColorName = "Default";
    ColorName += CollectionName;
    ColorName += "Color";
    if (CollectionName == "mesh")
      {
      Red   = 255;
      Green = 255;
      Blue  = 0;
      Alpha = 255;
      }
    if (CollectionName == "track")
      {
      Red   = 0;
      Green = 255;
      Blue  = 255;
      Alpha = 255;
      }
    if (CollectionName == "lineage")
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
    NewID = this->m_CollectionOfContours->CreateCollectionWithNoTraces(this->m_DatabaseConnector,
      FirstCollection);

    std::pair<std::string,QColor> temp;
    temp.second = FirstColor;
    temp.first = ConvertToString<int>(NewID);
    oListCollectionIDs.push_back(temp);
    }

  CloseDBConnection();

  return oListCollectionIDs;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateTableWidgetAndRowContainerWithNewCreatedTrace(
  QTableWidgetChild* Table,  vtkMySQLDatabase* DatabaseConnector,
  GoDBCollectionOfTraces* iCollectionOfTraces)
{
  GoDBTableWidgetContainer* LinkToNewTrace = iCollectionOfTraces->GetLinkToNewCreatedTraceContainer(
    this->m_DatabaseConnector);
  //update the RowContainer for the trace:
  iCollectionOfTraces->GetLinkToRowContainer()->InsertNewCreatedTrace(*LinkToNewTrace);

  //Update the table widget with the new trace:
  Table->setSortingEnabled(false);

  Table->InsertNewRow( LinkToNewTrace,iCollectionOfTraces->GetTraceName(),
    iCollectionOfTraces->GetCollectionName());

  Table->setSortingEnabled(true);
 }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
 void QGoPrintDatabase::UpdateTableWidgetAndRowContainerWithNewCollectionID(
    QTableWidgetChild* Table,vtkMySQLDatabase* DatabaseConnector,
    GoDBCollectionOfTraces* iCollectionOfTraces, unsigned int iNewCollectionID,
    QColor iColorNewCollection,std::list<int> iListSelectedTraces)
{
 GoDBTableWidgetContainer* LinkToRowContainerForTraces =
   iCollectionOfTraces->GetLinkToRowContainer();

 //update the RowContainer for traces with the new ID for the selected traces:
 LinkToRowContainerForTraces->UpdateIDs(iListSelectedTraces,iNewCollectionID);

 //update the RowContainer for collections with the created Collection:
 GoDBCollectionOfTraces* CollectionOfTracesForCollection = this->GetCollectionOfTraces(
   iCollectionOfTraces->GetCollectionName());
 QTableWidgetChild* TableForCollection =
   this->GetTableWidgetChild(iCollectionOfTraces->GetCollectionName());
 this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace(TableForCollection,
   this->m_DatabaseConnector,CollectionOfTracesForCollection);


 std::string CollectionIDName = iCollectionOfTraces->GetCollectionName();
 CollectionIDName += "ID";
 //update the Table Widget Display:
 Table->UpdateIDs(iNewCollectionID,CollectionIDName,iColorNewCollection);

}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBCollectionOfTraces* QGoPrintDatabase::GetCollectionOfTraces(
  std::string TraceName)
{
  if (TraceName == "contour")
    {
    return this->m_CollectionOfContours;
    }
  else
    {
    if (TraceName == "mesh")
      {
      return this->m_CollectionOfMeshes;
      }
    else
      {
      if (TraceName == "track")
        {
        return this->m_CollectionOfTracks;
        }
      else
        {
        if (TraceName == "lineage")
          {
          return this->m_CollectionOfLineages;
          }
        }
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::pair<std::string,QColor> QGoPrintDatabase::SaveNewCollectionInDB(
  std::pair<std::string,QColor> iColorNewCollection, std::string iTraceName)
{
  this->OpenDBConnection();
  GoDBTraceRow NewCollection;
  NewCollection.SetColor(iColorNewCollection.second.red(),iColorNewCollection.second.green(),
    iColorNewCollection.second.blue(),iColorNewCollection.second.alpha(),iColorNewCollection.first,
    this->m_DatabaseConnector);
  GoDBCollectionOfTraces* CollectionOfTracesForTraces = this->GetCollectionOfTraces(iTraceName);
  int NewCollectionID = CollectionOfTracesForTraces->CreateCollectionWithNoTraces(
    this->m_DatabaseConnector,NewCollection);

  std::pair<std::string,QColor> NewCollectionData;
  NewCollectionData.first = ConvertToString<int>(NewCollectionID);
  NewCollectionData.second = iColorNewCollection.second;

  //update the table of the collection (which become the trace then):
  GoDBCollectionOfTraces* CollectionOfTracesForCollection = this->GetCollectionOfTraces(
    CollectionOfTracesForTraces->CollectionName());

  this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace(
    this->GetTableWidgetChild(CollectionOfTracesForCollection->TracesName()),this->m_DatabaseConnector,
    CollectionOfTracesForCollection);

  this->CloseDBConnection();
  return NewCollectionData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QTableWidgetChild* QGoPrintDatabase::GetTableWidgetChild(std::string TraceName)
{
 if (TraceName == "contour")
   {
   return this->ContourTable;
   }
 if (TraceName == "mesh")
   {
   return this->MeshTable;
   }
 if (TraceName == "track")
   {
   return this->TrackTable;
   }
 if (TraceName == "lineage")
   {
   return this->LineageTable;
   }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateCurrentColorData(
  std::pair<std::string,QColor> iCurrentColorData)
{
 this->m_CurrentColorData = iCurrentColorData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::pair<std::string,QColor> QGoPrintDatabase::GetDataNewCreatedCollection()
{
  return this->m_LastCreatedCollection;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetCurrentCollectionID(
  std::pair<std::string,QColor> iCurrentCollectionData)
{
  this->m_CurrentCollectionData = iCurrentCollectionData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::pair<std::string,QColor> QGoPrintDatabase::GetCurrentCollectionData()
{
  return this->m_CurrentCollectionData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::ReEditTrace()
{
  std::string TraceName = this->InWhichTableAreWe();
  QTableWidgetChild* Table = this->GetTableWidgetChild(TraceName.c_str());
  std::list<int> SelectedTrace = Table->GetListCheckedTraceID();
  if (SelectedTrace.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select the %1 you want to reedit")
      .arg(TraceName.c_str() ) );
    msgBox.exec();
    }
  else
    {
    if (SelectedTrace.size() != 1)
      {
      QMessageBox msgBox;
      msgBox.setText(
        tr("Please select only one %1 to reedit")
        .arg(TraceName.c_str() ) );
      msgBox.exec();
      }
    else
      {
      std::list<int>::iterator iter = SelectedTrace.begin();
      this->m_TraceIDToReedit = *iter;
      TraceToReEdit();
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoPrintDatabase::GetTraceIDToReedit()
{ /** \todo look if possible the signal to return the int directly*/
  return this->m_TraceIDToReedit;
}