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
#include <qstringlist.h>
#include <qwidget.h>
#include <QDialog>
#include <QInputDialog>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QSettings>
#include <QMenu>

#include "QGoPrintDatabase.h"
#include "QTableWidgetChild.h"
#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStringArray.h"
#include "vtkStdString.h"
#include "GoDBRecordSet.h"
#include "GoDBRecordSetHelper.h"
#include "GoDBContourRow.h"
#include "GoDBMeshRow.h"
#include "GoDBTrackRow.h"
#include "GoDBLineageRow.h"
#include "QueryDataBaseHelper.h"
#include "ConvertToStringHelper.h"
#include <iostream>
#include <QCloseEvent>

QGoPrintDatabase::
QGoPrintDatabase( QWidget* iParent ) :
  QWidget( iParent ),
  m_DatabaseConnector( 0 )
{
  this->setupUi( this );

  DBTabWidget->setTabPosition( QTabWidget::West );
  DBTabWidget->setTabShape( QTabWidget::Triangular );
  DBTabWidget->removeTab( 0 );

  ContourTable = new QTableWidgetChild( this );
  MeshTable = new QTableWidgetChild( this );
  TrackTable = new QTableWidgetChild( this );
  LineageTable = new QTableWidgetChild( this );

  m_CollectionOfContours = new GoDBCollectionOfTraces( "mesh",
    "MeshID", "contour", "ContourID" );
  m_CollectionOfMeshes = new GoDBCollectionOfTraces( "track",
    "TrackID", "mesh", "MeshID" );
  m_CollectionOfTracks = new GoDBCollectionOfTraces( "lineage",
    "LineageID", "track", "TrackID" );

  this->setContextMenuPolicy( Qt::CustomContextMenu );

  m_VisibilityAction = new QAction( this );
  m_VisibilityAction->setCheckable( true );
  
  QObject::connect( m_VisibilityAction, SIGNAL( toggled( bool ) ),
    this, SLOT( setVisible( bool ) ) );

  QObject::connect( this, SIGNAL( customContextMenuRequested( const QPoint & ) ),
    this, SLOT( CreateContextMenu( const QPoint & ) ) );
  
 // QObject::connect( this->ContourTable, SIGNAL(itemSelectionChanged()),
   // this, SLOT(ChangeTracesToHighLightInfoFromTableWidget()));

 // QObject::connect( this->MeshTable, SIGNAL(itemSelectionChanged()),
 //   this, SLOT(ChangeTracesToHighLightInfoFromTableWidget()));

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoPrintDatabase::~QGoPrintDatabase()
{
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
void QGoPrintDatabase::QPrintColumnNames( QString TableName,
  std::vector< std::string > ColumnNames, QTableWidgetChild* QTabTableName )
{
  int numberCol=ColumnNames.size();
  this->DBTabWidget->addTab(QTabTableName,TableName);
  QTabTableName->setColumnCount(numberCol);

  for ( int i = 0; i < numberCol; i++ )
    {
    QTableWidgetItem* HeaderCol=new QTableWidgetItem;
    std::string NameHeader;
    NameHeader =ColumnNames[i];
    
    HeaderCol->setText(NameHeader.c_str());
    QFont serifFont("Arial", 10, QFont::Bold);
    HeaderCol->setFont(serifFont);
    QTabTableName->setHorizontalHeaderItem(i,HeaderCol);
    QTabTableName->resizeColumnToContents(i);
    
    }

  QTabTableName->horizontalHeader()->setSortIndicatorShown(true);
  /*Need to disabled the Sorting while printing the values from the database in
  the table widget as the sorting is making trouble*/
  QTabTableName->setSortingEnabled(false);
  QTabTableName->horizontalHeader()->setMovable(true);

  QObject::connect( QTabTableName->horizontalHeader(),
    SIGNAL( sortIndicatorChanged(int,Qt::SortOrder) ),
    QTabTableName,SLOT( sortItems(int,Qt::SortOrder)) );

  QSettings settings( "MegasonLab", "Gofigure2" );
  QByteArray stateTableWidget = settings.value("StateTableWidget").toByteArray();
  //QTabTableName->horizontalHeader()->restoreState(stateTableWidget);
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
  m_CollectionOfMeshes   ->SetImgSessionID(m_ImgSessionID);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::OpenDBConnectionForTables()
{
  if (m_DatabaseConnector == 0)
    {
    m_DatabaseConnector = OpenDatabaseConnection(m_Server,m_User,
      m_Password,m_DBName);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CloseDBConnectionForTables()
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
  OpenDBConnectionForTables();

  QString title = QString( "Imaging Session: %1 " ).arg( m_ImgSessionName.c_str() );
  this->setWindowTitle( title );
  m_VisibilityAction->setText( title );

  GetContentAndDisplayFromDB< GoDBContourRow >( "contour", ContourTable);
  GetContentAndDisplayFromDB< GoDBMeshRow    >( "mesh", MeshTable );
  GetContentAndDisplayFromDB< GoDBTrackRow   >( "track", TrackTable );
  GetContentAndDisplayFromDB< GoDBLineageRow >( "lineage", LineageTable );

  LoadContoursAndMeshesFromDB(m_DatabaseConnector);

  CloseDBConnectionForTables();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::closeEvent(QCloseEvent* iEvent)
{
  /*int r = QMessageBox::warning(this, tr(""),
                        tr("Are you sure you want to close\n"
                           "permanently the table?"),
                          QMessageBox::Yes,
                          QMessageBox::No|QMessageBox::Default);
  if (r == QMessageBox::Yes)
    {
    iEvent->accept();
    QByteArray stateContourTable = ContourTable->horizontalHeader()->saveState();
    QByteArray stateMeshTable = MeshTable->horizontalHeader()->saveState();
    QByteArray stateTrackTable = TrackTable->horizontalHeader()->saveState();
    QByteArray stateLineageTable = LineageTable->horizontalHeader()->saveState();
    QSettings settings( "MegasonLab", "Gofigure2" );
    settings.setValue("StateContourTable", stateContourTable);
    settings.setValue("StateMeshTable", stateMeshTable);
    settings.setValue("StateTrackTable",stateTrackTable);
    settings.setValue("StateLineageTable",stateLineageTable);
    }
  else
    {
    iEvent->ignore();
    }*/

  m_VisibilityAction->setChecked( false );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::UpdateTableFromDB()
{
  //todo: make it ok for all tables
  UpdateContentAndDisplayFromDB< GoDBContourRow >("contour",
    ContourTable,m_DatabaseConnector);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateContextMenu(const QPoint &iPos)
{
  QMenu* ContextMenu = new QMenu;
  ContextMenu->addAction(tr("Delete Traces"),this,SLOT(DeleteTraces()));
  ContextMenu->addAction(tr("Create New Collection"),this,SLOT(CreateCorrespondingCollection()));
  ContextMenu->addAction(tr("Add to existing Collection"),this,SLOT(AddToExistingCollection()));
  ContextMenu->exec(this->mapToGlobal(iPos));

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTraces()
{
  OpenDBConnectionForTables();
  int TabIndex = InWhichTableAreWe();

  switch (TabIndex)
    {
    case 0: //contour
      {
      //add the tableWidgetChild in the CollectionOfTraces?
      QStringList ContoursToDelete = this->ContourTable->ValuesForSelectedRows("ContourID");
      m_CollectionOfContours->DeleteTraces(ContoursToDelete,m_DatabaseConnector);
      this->UpdateContentAndDisplayFromDB<GoDBContourRow>("contour",
        ContourTable,m_DatabaseConnector);
      break;
      }
    case 1: //mesh
      {
      QStringList MeshesToDelete = this->MeshTable->ValuesForSelectedRows("MeshID");
      m_CollectionOfMeshes->DeleteTraces(MeshesToDelete,m_DatabaseConnector);
      this->UpdateContentAndDisplayFromDB<GoDBMeshRow>("mesh",
        MeshTable,m_DatabaseConnector);
      break;
      }
    case 2: //track
      {
      QStringList TracksToDelete = this->TrackTable->ValuesForSelectedRows("TrackID");
        m_CollectionOfTracks->DeleteTraces(TracksToDelete,m_DatabaseConnector);
      this->UpdateContentAndDisplayFromDB<GoDBTrackRow>("track",
        TrackTable,m_DatabaseConnector);
      break;
      }
    case 3: //lineage
      {
      QStringList LineagesToDelete = this->LineageTable->ValuesForSelectedRows("LineageID");
      m_CollectionOfLineages->DeleteTraces(LineagesToDelete,m_DatabaseConnector);
      this->UpdateContentAndDisplayFromDB<GoDBLineageRow>("lineage",
        LineageTable,m_DatabaseConnector);
      break;
      }
    default:
      {
      std::cout<<"error, tab doesn't exist";
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      break;
      }
    }
  CloseDBConnectionForTables();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateCorrespondingCollection()
{
  OpenDBConnectionForTables();
  int TabIndex = InWhichTableAreWe();

  switch (TabIndex)
    {
    case 0: //contour
        {
        //add the tableWidgetChild in the CollectionOfTraces?
        QStringList ListSelectedContours = this->ContourTable->ValuesForSelectedRows("ContourID");
        GoDBMeshRow myNewMesh;
        //The specified fields for mesh will be filled here:
        //myNewMesh.SetField("ColorID") = to be defined for some color chosen by the user
        //myNewMesh.SetField("CellTypeID") = to be defined for some color chosen by the user
        //myNewMesh.SetField("SubCellularTypeID") = to be defined for some color chosen by the user

        m_CollectionOfContours->CreateNewCollectionFromSelection<GoDBMeshRow>(ListSelectedContours,
          m_DatabaseConnector,myNewMesh);
        this->UpdateContentAndDisplayFromDB<GoDBContourRow>("contour",
          ContourTable,m_DatabaseConnector);
        this->UpdateContentAndDisplayFromDB<GoDBMeshRow>("mesh",
          MeshTable,m_DatabaseConnector);
        break;
        }
    case 1: //mesh
        {
        QStringList ListSelectedMeshes = this->MeshTable->ValuesForSelectedRows("MeshID");
        GoDBTrackRow myNewTrack;
        //myNewTrack.SetField("ColorID",  to be defined for some color chosen by the user
        m_CollectionOfMeshes->CreateNewCollectionFromSelection<GoDBTrackRow>(ListSelectedMeshes,
          m_DatabaseConnector,myNewTrack);
        this->UpdateContentAndDisplayFromDB<GoDBMeshRow>("mesh",MeshTable,m_DatabaseConnector);
        this->UpdateContentAndDisplayFromDB<GoDBTrackRow>("track",TrackTable,m_DatabaseConnector);
        break;
        }
    case 2: //track
      {
      QStringList ListSelectedTracks = this->TrackTable->ValuesForSelectedRows("TrackID");
      GoDBLineageRow myNewLineage;
      //myNewLineage.SetField("ColorID",  to be defined for some color chosen by the user
      m_CollectionOfTracks->CreateNewCollectionFromSelection<GoDBLineageRow>(ListSelectedTracks,
        m_DatabaseConnector,myNewLineage);
      this->UpdateContentAndDisplayFromDB<GoDBTrackRow>("track",TrackTable,m_DatabaseConnector);
      this->UpdateContentAndDisplayFromDB<GoDBLineageRow>("lineage",LineageTable,m_DatabaseConnector);
      break;
      }
    default:
      {
      std::cout<<"error, tab doesn't exist";
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      break;
      }
    }
  CloseDBConnectionForTables();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//
//  Return negative value on error
//  Return 0 if we are in the Contour Table
//  Return 1 if we are in the Mesh    Table
//  Return 2 if we are in the Track   Table
//  Return 3 if we are in the Track   Table
int QGoPrintDatabase::InWhichTableAreWe ()
{
  int CurrentIndex = this->DBTabWidget->currentIndex();
  QString TabName = this->DBTabWidget->tabText(CurrentIndex);

  int TabIndex = -1; // Default value.

  if (TabName == "contour")
    {
    TabIndex = 0;
    }
  if (TabName == "mesh")
    {
    TabIndex = 1;
    }
  if (TabName == "track")
    {
    TabIndex = 2;
    }
  if (TabName == "lineage")
    {
    TabIndex = 3;
    }

  return TabIndex;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::AddToExistingCollection()
{
  OpenDBConnectionForTables();
  QStringList items;
  QString LabelDialog;
  int TabIndex = InWhichTableAreWe();

  switch (TabIndex)
    {
    case 0: //contour
        {
        items = m_CollectionOfContours->ListCollectionID(m_DatabaseConnector);
        LabelDialog = tr("Choose the Mesh ID you want\n the selected contours to be part of: ");
        break;
        }
    case 1: //mesh
        {
        items = m_CollectionOfMeshes->ListCollectionID(m_DatabaseConnector);
        LabelDialog = tr("Choose the Track ID you want\n the selected meshes to be part of: ");
        break;
        }
    case 2://track
      {
      items = m_CollectionOfTracks->ListCollectionID(m_DatabaseConnector);
      LabelDialog = tr("Choose the Lineage ID you want\n the selected tracks to be part of: ");
      break;
      }
    default:
      {
      std::cout<<"error, tab doesn't exist";
      std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
      std::cout << std::endl;
      break;
      }
    }

  bool ok;
  QString CollectionID = QInputDialog::getItem(this, tr("Collection ID"),
                         LabelDialog, items,0,false,&ok);

  if (ok && !CollectionID.isEmpty())
    {
    switch (TabIndex)
      {
      case 0: //contour
          {
          QStringList ListContours = this->ContourTable->ValuesForSelectedRows("ContourID");
          m_CollectionOfContours->AddSelectedTracesToCollection(ListContours,
            CollectionID.toInt(),m_DatabaseConnector);
          this->UpdateContentAndDisplayFromDB<GoDBContourRow>("contour",
            ContourTable,m_DatabaseConnector);
          break;
          }
      case 1: //mesh
          {
          QStringList ListMeshes = this->MeshTable->ValuesForSelectedRows("MeshID");
          m_CollectionOfMeshes->AddSelectedTracesToCollection(ListMeshes,
            CollectionID.toInt(),m_DatabaseConnector);
          this->UpdateContentAndDisplayFromDB<GoDBMeshRow>("mesh",
            MeshTable, m_DatabaseConnector);
          break;
          }
      case 2: //track
        {
        QStringList ListTracks = this->TrackTable->ValuesForSelectedRows("TrackID");
        m_CollectionOfTracks->AddSelectedTracesToCollection(ListTracks,
          CollectionID.toInt(),m_DatabaseConnector);
        this->UpdateContentAndDisplayFromDB<GoDBTrackRow>("track",
          TrackTable, m_DatabaseConnector);
        break;
        }
      default:
        {
        std::cout<<"error, tab doesn't exist";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        break;
        }
      }
    }
  CloseDBConnectionForTables();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::LoadContoursAndMeshesFromDB(
  vtkMySQLDatabase* DatabaseConnector)
{  
  m_ContoursInfo = GetTracesInfoFromDB(DatabaseConnector,"contour",
    m_ImgSessionID);  
  m_MeshesInfo   = GetTracesInfoFromDB(DatabaseConnector,"mesh",
    m_ImgSessionID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::ChangeTracesToHighLightInfoFromTableWidget()
{
  int TabIndex = InWhichTableAreWe();
  switch (TabIndex)
    {
    case 0: //contour
        {
        this->ContourTable->TracesToHighlight("Contour",m_ContoursInfo);
        emit SelectionContoursToHighLightChanged();
        break;
        }
    case 1: //mesh
        {
        this->MeshTable->TracesToHighlight("Mesh",m_MeshesInfo);
        emit SelectionMeshesToHighLightChanged();
        break;
        }
    default:
        {
        std::cout<<"error, tab doesn't exist";
        std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
        std::cout << std::endl;
        break;
        }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::ChangeContoursToHighLightInfoFromVisu(
  std::list<int> iListContoursHighLightedInVisu)
{
    int i = 0; 
    std::list<int>::iterator it = iListContoursHighLightedInVisu.begin();
    while (it != iListContoursHighLightedInVisu.end())
      {
      
      for (int j = 0 ; j < m_ContoursInfo.size(); j++)
        {
        if (*it == j+1)
          {
          m_ContoursInfo[j].IsHighLighted = true;
          this->ContourTable->SetSelectRowTraceID("Contour",
            m_ContoursInfo[j].TraceID,true);
          it++;
          }
        else
          {
          m_ContoursInfo[j].IsHighLighted = false;
          this->ContourTable->SetSelectRowTraceID("Contour",
            m_ContoursInfo[j].TraceID,false);
          }
        }
      }
}