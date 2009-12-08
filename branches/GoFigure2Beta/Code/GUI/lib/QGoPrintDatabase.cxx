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

  QObject::connect( this->ContourTable, SIGNAL(itemSelectionChanged()),
    this, SLOT(ChangeTracesToHighLightInfoFromTableWidget()));

 // QObject::connect( this->MeshTable, SIGNAL(itemSelectionChanged()),
 //   this, SLOT(ChangeTracesToHighLightInfoFromTableWidget()));

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
  ContextMenu->addAction(tr("Delete Traces"),this,SLOT(DeleteTraces()));
  ContextMenu->addAction(tr("Create New Collection"),this,SLOT(CreateCorrespondingCollection()));
  ContextMenu->addAction(tr("Add to existing Collection"),this,SLOT(AddToExistingCollection()));
  ContextMenu->exec(this->mapToGlobal(iPos));

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTraces()
{
  OpenDBConnection();
  int TabIndex = InWhichTableAreWe();

  switch (TabIndex)
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
    }
  CloseDBConnection();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateCorrespondingCollection()
{
  OpenDBConnection();
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
       // this->UpdateContentAndDisplayFromDB<GoDBContourRow>("contour",
       //   ContourTable,m_DatabaseConnector);
       // this->UpdateContentAndDisplayFromDB<GoDBMeshRow>("mesh",
       //   MeshTable,m_DatabaseConnector);
        break;
        }
    case 1: //mesh
        {
        QStringList ListSelectedMeshes = this->MeshTable->ValuesForSelectedRows("MeshID");
        GoDBTrackRow myNewTrack;
        //myNewTrack.SetField("ColorID",  to be defined for some color chosen by the user
        m_CollectionOfMeshes->CreateNewCollectionFromSelection<GoDBTrackRow>(ListSelectedMeshes,
          m_DatabaseConnector,myNewTrack);
      //  this->UpdateContentAndDisplayFromDB<GoDBMeshRow>("mesh",MeshTable,m_DatabaseConnector);
      //  this->UpdateContentAndDisplayFromDB<GoDBTrackRow>("track",TrackTable,m_DatabaseConnector);
        break;
        }
    case 2: //track
      {
      QStringList ListSelectedTracks = this->TrackTable->ValuesForSelectedRows("TrackID");
      GoDBLineageRow myNewLineage;
      //myNewLineage.SetField("ColorID",  to be defined for some color chosen by the user
      m_CollectionOfTracks->CreateNewCollectionFromSelection<GoDBLineageRow>(ListSelectedTracks,
        m_DatabaseConnector,myNewLineage);
     // this->UpdateContentAndDisplayFromDB<GoDBTrackRow>("track",TrackTable,m_DatabaseConnector);
     // this->UpdateContentAndDisplayFromDB<GoDBLineageRow>("lineage",LineageTable,m_DatabaseConnector);
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
  CloseDBConnection();
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
  OpenDBConnection();
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
         // this->UpdateContentAndDisplayFromDB<GoDBContourRow>("contour",
         //   ContourTable,m_DatabaseConnector);
          break;
          }
      case 1: //mesh
          {
          QStringList ListMeshes = this->MeshTable->ValuesForSelectedRows("MeshID");
          m_CollectionOfMeshes->AddSelectedTracesToCollection(ListMeshes,
            CollectionID.toInt(),m_DatabaseConnector);
         // this->UpdateContentAndDisplayFromDB<GoDBMeshRow>("mesh",
         //   MeshTable, m_DatabaseConnector);
          break;
          }
      case 2: //track
        {
        QStringList ListTracks = this->TrackTable->ValuesForSelectedRows("TrackID");
        m_CollectionOfTracks->AddSelectedTracesToCollection(ListTracks,
          CollectionID.toInt(),m_DatabaseConnector);
       // this->UpdateContentAndDisplayFromDB<GoDBTrackRow>("track",
      //    TrackTable, m_DatabaseConnector);
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
  CloseDBConnection();
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
void QGoPrintDatabase::ChangeTracesToHighLightInfoFromTableWidget()
{
  int TabIndex = InWhichTableAreWe();
  switch (TabIndex)
    {
    case 0: //contour
        {
        this->ContourTable->TracesToHighlight("contour",m_ContoursInfo);
        emit SelectionContoursToHighLightChanged();
        break;
        }
    case 1: //mesh
        {
        this->MeshTable->TracesToHighlight("mesh",m_MeshesInfo);
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
void
QGoPrintDatabase::
ChangeContoursToHighLightInfoFromVisu( std::list<int> iListContoursHighLightedInVisu)
{
//     int i = 0;
    std::list<int>::iterator it = iListContoursHighLightedInVisu.begin();
    while (it != iListContoursHighLightedInVisu.end())
      {

      for (unsigned int j = 0 ; j < m_ContoursInfo.size(); j++)
        {
        if (*it == static_cast< int >( j+1 ) )
          {
          m_ContoursInfo[j].Highlighted = true;
          this->ContourTable->SetSelectRowTraceID("Contour",
            m_ContoursInfo[j].TraceID,true);
          it++;
          }
        else
          {
          m_ContoursInfo[j].Highlighted = false;
          this->ContourTable->SetSelectRowTraceID("Contour",
            m_ContoursInfo[j].TraceID,false);
          }
        }
      }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SaveContoursFromVisuInDB(unsigned int iXCoordMin,
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

  contour_row.SaveInDB( this->m_DatabaseConnector);

 // UpdateContentAndDisplayFromDB< GoDBContourRow >("contour",
  //  ContourTable,m_DatabaseConnector);
  this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace(this->ContourTable,
    this->m_DatabaseConnector,this->m_CollectionOfContours);

  CloseDBConnection();
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
void QGoPrintDatabase::SaveNewColorInDB(std::vector<std::string> iDataNewColor)
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
  /*std::string CollectionID = CollectionName;
  CollectionID += "ID";*/
  std::list<std::pair<std::string, QColor> > oListCollectionIDs;
  //First, build the query with selected fields and table to join with on conditions:
  std::vector<std::string> SelectFields;
  std::string CollectionID = TraceName;
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

  std::string JoinTable = "mesh";
  JoinTablesOnTraceTable.push_back(JoinTable);
  std::string OnCondition = TraceName;
  OnCondition += ".";
  OnCondition += CollectionName;
  OnCondition += "ID = ";
  OnCondition += CollectionName;
  OnCondition += ".";
  OnCondition += CollectionName;
  OnCondition += "ID";
   JoinTablesOnTraceTable.push_back(OnCondition);

  std::string JoinTable2 = "color";
  JoinTablesOnTraceTable.push_back(JoinTable2);
  std::string OnCondition2 = CollectionName;
  OnCondition2 += ".ColorID = color.ColorID";
   JoinTablesOnTraceTable.push_back(OnCondition2);

 //Get the results for the query:
  std::vector<std::vector<std::string> >ResultsQuery  = GetValuesFromSeveralTables(
    this->m_DatabaseConnector,TraceName,SelectFields, "ImagingSessionID",
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
    i=i+4;
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
    if (CollectionName == "mesh")
      {
      GoDBMeshRow FirstMesh;
      NewID = this->m_CollectionOfContours->CreateFirstCollection<GoDBMeshRow>(
        this->m_DatabaseConnector, FirstMesh);
      Red = 255;
      Green = 255;
      Blue = 0;
      Alpha = 255;
      }
    if (CollectionName == "track")
      {
      GoDBTrackRow FirstTrack;
      NewID = this->m_CollectionOfMeshes->CreateFirstCollection<GoDBTrackRow>(
        this->m_DatabaseConnector, FirstTrack);
      Red = 0;
      Green = 255;
      Blue = 255;
      Alpha = 255;
      }
    if (CollectionName == "lineage")
      {
      GoDBLineageRow FirstLineage;
      NewID = this->m_CollectionOfTracks->CreateFirstCollection<GoDBLineageRow>(
        this->m_DatabaseConnector, FirstLineage);
      Red = 255;
      Green = 0;
      Blue = 255;
      Alpha = 255;
      }
    FirstColor.setRed(Red);
    FirstColor.setGreen(Green);
    FirstColor.setBlue(Blue);
    FirstColor.setAlpha(Alpha);

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
GoDBCollectionOfTraces* QGoPrintDatabase::GetCurrentCollection(
  std::string CollectionName)
{
  if (CollectionName == "mesh")
    {
    return this->m_CollectionOfContours;
    }
  else
    {
    if (CollectionName == "track")
      {
      return this->m_CollectionOfMeshes;
      }
    else
      {
      if (CollectionName == "lineage")
        {
        return this->m_CollectionOfTracks;
        }
      else
        {
        return 0;
        }
      }
    }
}
//-------------------------------------------------------------------------
