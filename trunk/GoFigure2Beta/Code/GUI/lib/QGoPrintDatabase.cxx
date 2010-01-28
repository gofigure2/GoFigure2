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

  m_VisibilityAction = new QAction(tr("Show/hide the table widget"),this );
  QIcon TableWidgetIcon;
  TableWidgetIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/TableWidget.png")),
    QIcon::Normal, QIcon::Off );
  m_VisibilityAction->setIcon(TableWidgetIcon);
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
  delete m_CollectionOfLineages;
  delete m_CollectionOfTracks;
  delete m_CollectionOfMeshes;
  delete m_CollectionOfContours;

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

  GetContentAndDisplayFromDB( "contour" );
  GetContentAndDisplayFromDB( "mesh" );
  GetContentAndDisplayFromDB( "track" );
  GetContentAndDisplayFromDB( "lineage" );

  LoadContoursAndMeshesFromDB(m_DatabaseConnector);

  m_IsDatabaseUsed = true;
  PrintExistingColorsFromDB(this->GetColorComboBoxInfofromDB());
  /** \todo get the trace name from the visudockwidget*/
  PrintExistingCollectionIDsFromDB(
    this->GetListExistingCollectionIDFromDB("contour"));
  CloseDBConnection();

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::closeEvent(QCloseEvent* iEvent)
{
  m_VisibilityAction->setChecked( false );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateContextMenu(const QPoint &iPos)
{
  QMenu* ContextMenu = new QMenu;
  NeedCurrentSelectedCollectionID();
  std::string TraceName = this->InWhichTableAreWe();
  this->SetCurrentlyUsedTraceData(TraceName);
  ContextMenu->addAction(tr("Delete selected %1s").arg(TraceName.c_str()),
    this,SLOT(DeleteTraces()));
  ContextMenu->addAction(tr("Create a new %1").arg(this->m_CurrentlyUsedCollectionName.c_str()),
    this,SLOT(CreateCorrespondingCollection()));
  ContextMenu->addAction(
    tr("Add to selected %1 : %2").arg(this->m_CurrentlyUsedCollectionName.c_str())
    .arg(this->m_CurrentCollectionData.first.c_str()),this,SLOT(AddToSelectedCollection()));
  ContextMenu->addAction(tr("ReEdit the selected %1").arg(TraceName.c_str()),
    this,SLOT(ReEditTrace()));
  ContextMenu->addAction(tr("Copy Selection"),
    this->m_CurrentlyUsedTable,SLOT(CopySelection()));
  ContextMenu->exec(this->mapToGlobal(iPos));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTraces()
{
  std::string TraceName = this->InWhichTableAreWe();
  this->SetCurrentlyUsedTraceData(TraceName);
  std::list<int> SelectedTraces = this->m_CurrentlyUsedTable->GetListCheckedTraceID();
  
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
                 "permanently the selected %1s?").arg(TraceName.c_str()),
              QMessageBox::Yes,
              QMessageBox::No|QMessageBox::Default);

    if (r == QMessageBox::Yes)
      {
      OpenDBConnection();
      //check that the traces are not collection of existing traces:
      std::vector<std::string> VectorValues;
      std::list<int>::iterator iter = SelectedTraces.begin();
      while(iter != SelectedTraces.end())
        {
        int ID = *iter;
        //delete the trace in the vector<ContourMeshStructure>:
        if (this->m_CurrentlyUsedTraceName == "contour")
          {
          this->DeleteTraceInContourMeshStructure(ID,this->m_ContoursInfo);
          }
        else
          {
          this->DeleteTraceInContourMeshStructure(ID,this->m_MeshesInfo);
          }
        VectorValues.push_back(ConvertToString<int>(ID));
        iter++;
        }
      if (!this->m_CurrentlyUsedCollectionOfName.empty())
        {
        //delete collectionID in the colorcollectionid combobox:
        std::list<int>::iterator iterSelec = SelectedTraces.begin();
        while(iterSelec != SelectedTraces.end())
         {
         int TraceID = *iterSelec;
         this->m_CurrentCollectionData.first = ConvertToString<int>(TraceID);
         DeletedCollection();
         iterSelec++;
         }

         std::vector<std::string> ListBelongingTraces = ListSpecificValuesForOneColumn(
           this->m_DatabaseConnector,this->m_CurrentlyUsedCollectionOfName,
           this->m_CurrentlyUsedCollectionOfNameID,this->m_CurrentlyUsedTraceIDName,VectorValues);

        if (!ListBelongingTraces.empty())
          {
          //put the collectionid of the belonging traces to 0:
          std::list<int> TracesWithCollectionToBeNull;
          std::vector<std::string>::iterator it = ListBelongingTraces.begin();
          while (it != ListBelongingTraces.end())
            {
            std::string ID = *it;
            TracesWithCollectionToBeNull.push_back(atoi(ID.c_str()));
            it++;
            }
          //update everything for the collection of: exp: for contour if the trace deleted is a mesh:
          this->SetCurrentlyUsedTraceData(this->m_CurrentlyUsedCollectionOfName);
          this->m_CurrentlyUsedCollectionOfTraces->UpdateCollectionIDOfSelectedTraces(
            TracesWithCollectionToBeNull,0,this->m_DatabaseConnector);
          QColor Color(255,255,255,255);
          this->m_CurrentlyUsedTable->UpdateIDs(0,this->m_CurrentlyUsedCollectionIDName,Color,
            this->m_CurrentlyUsedTraceIDName,TracesWithCollectionToBeNull); 
          }
        }     
      this->SetCurrentlyUsedTraceData(TraceName);
      //delete traces in the row container:
      GoDBTableWidgetContainer* LinkToTracesContainer = this->m_CurrentlyUsedCollectionOfTraces->GetLinkToRowContainer();
      LinkToTracesContainer->DeleteSelectedTraces(SelectedTraces);
      this->m_CurrentlyUsedTable->DeleteSelectedRows(this->m_CurrentlyUsedTraceIDName);
      //delete traces in database + update bounding box for collection and update table widget:
      std::list<int>::iterator iterator = SelectedTraces.begin();
      while (iterator != SelectedTraces.end())
        {
        this->SetCurrentlyUsedTraceData(TraceName);
        int tempTraceID = *iterator;
        int tempCollectionID = FindOneID(this->m_DatabaseConnector,TraceName,this->m_CurrentlyUsedCollectionIDName,
          this->m_CurrentlyUsedTraceIDName, ConvertToString<int>(tempTraceID));
        //save the current collection of traces corresponding to the trace before recentring to the collection:
        GoDBCollectionOfTraces* CollectionOfTraces = this->m_CurrentlyUsedCollectionOfTraces;
        CollectionOfTraces->DeleteTraceInDB(tempTraceID,this->m_DatabaseConnector);
        //change the data who were related to the trace to the collection:
        this->SetCurrentlyUsedTraceData(this->m_CurrentlyUsedCollectionName);
        
        if (tempCollectionID != 0)
          {
          CollectionOfTraces->RecalculateDBBoundingBox(this->m_DatabaseConnector,tempCollectionID);
          this->UpdateTableWidgetForAnExistingTrace(this->m_CurrentlyUsedTraceName,tempCollectionID);
          }
        iterator++;
        }
      CloseDBConnection();
      TracesToDeleteInVisu();
      }
    }
  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::CreateCorrespondingCollection()
{
  std::string TraceName = InWhichTableAreWe();
  this->SetCurrentlyUsedTraceData(TraceName);

  std::list<int> ListSelectedTraces = this->m_CurrentlyUsedTable->GetListCheckedTraceID();
  if (ListSelectedTraces.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1 to be part of the new %2")
      .arg(this->m_CurrentlyUsedTraceName.c_str() )
      .arg(this->m_CurrentlyUsedCollectionName.c_str()));
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

    int NewCollectionID = this->m_CurrentlyUsedCollectionOfTraces->CreateNewCollectionFromSelection(
      ListSelectedTraces,this->m_DatabaseConnector,NewCollection);

    //update the Collection Table and the row container with the new created Collection:
    this->SetCurrentlyUsedTraceData(this->m_CurrentlyUsedCollectionName);
    this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace(this->m_CurrentlyUsedTraceName);

    //update the Trace Table and the row container with the new Collection ID:
    this->SetCurrentlyUsedTraceData(TraceName);
    this->UpdateTableWidgetAndRowContainerWithNewCollectionID(this->m_CurrentlyUsedTable,
      this->m_DatabaseConnector,this->m_CurrentlyUsedCollectionOfTraces,NewCollectionID,
      this->m_CurrentColorData.second,ListSelectedTraces);

    CloseDBConnection();
    QString CollectionIDQString = ConvertToString<int>(NewCollectionID).c_str();
    NewCreatedCollection(this->m_CurrentColorData.second,CollectionIDQString);
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
  this->SetCurrentlyUsedTraceData(TraceName);
  std::list<int> ListSelectedTraces = this->m_CurrentlyUsedTable->GetListCheckedTraceID();
  NeedCurrentSelectedCollectionID();

  if (ListSelectedTraces.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1 to be part of the %2 %3")
      .arg(this->m_CurrentlyUsedTraceName.c_str() )
      .arg(this->m_CurrentlyUsedCollectionName.c_str() )
      .arg(this->m_CurrentCollectionData.first.c_str() ) );
    msgBox.exec();
    }
  else
    {
    OpenDBConnection();
    int CollectionID = atoi(this->m_CurrentCollectionData.first.c_str());
    QColor ColorCollection = this->m_CurrentCollectionData.second;

    GoDBTableWidgetContainer* LinkToRowContainerForTraces =
      this->m_CurrentlyUsedCollectionOfTraces->GetLinkToRowContainer();

    //update the corresponding database data:
    std::list<int> ListCollectionsToUpdateInTableWidget = 
      this->m_CurrentlyUsedCollectionOfTraces->
      UpdateDBDataForAddedTracesToExistingCollection(ListSelectedTraces,
      CollectionID,this->m_DatabaseConnector);
    
    //update the RowContainer for traces with the new ID for the selected traces:
    LinkToRowContainerForTraces->UpdateIDs(ListSelectedTraces,CollectionID);

    //update the Table Widget Display:
    this->m_CurrentlyUsedTable->UpdateIDs(CollectionID,
      this->m_CurrentlyUsedCollectionIDName,ColorCollection);
    //update the Collection Table with the new bounding box:
    this->SetCurrentlyUsedTraceData(this->m_CurrentlyUsedCollectionName);
    this->UpdateTableWidgetForAnExistingTrace(this->m_CurrentlyUsedTraceName,CollectionID); 
    //update the Collection Table with the old updated bounding box:
    if (!ListCollectionsToUpdateInTableWidget.empty())
      {
      std::list<int>::iterator iter = ListCollectionsToUpdateInTableWidget.begin();
      while (iter != ListCollectionsToUpdateInTableWidget.end())
        {
        int TraceID = *iter;
        this->UpdateTableWidgetForAnExistingTrace(this->m_CurrentlyUsedTraceName,TraceID);
        iter++;
        }
      }    
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
  this->SetCurrentlyUsedTraceData(TraceName);

  if ( TraceName == "contour")
    {
    this->m_CurrentlyUsedTable->TracesToHighlight(TraceName,this->m_ContoursInfo);
    emit SelectionContoursToHighLightChanged();
    }
  if (TraceName == "mesh")
    {
    this->m_CurrentlyUsedTable->TracesToHighlight(TraceName,this->m_MeshesInfo);
    emit SelectionMeshesToHighLightChanged();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::
ChangeContoursToHighLightInfoFromVisu(
  std::list<int> iListContoursHighLightedInVisu, bool Reedit )
{
  /** \todo get the tracename from the visu dock widget*/
  QTableWidgetChild* Table = this->GetTableWidgetChild("contour");
  if (!Reedit)
    {
    if( !iListContoursHighLightedInVisu.empty())
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

  this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace("contour");
  this->AddATraceToContourMeshInfo("contour",NewContourID);

  CloseDBConnection();

  return NewContourID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoPrintDatabase::UpdateContourFromVisuInDB(unsigned int iXCoordMin,
    unsigned int iYCoordMin, unsigned int iZCoordMin, unsigned int iTCoord,
    unsigned int iXCoordMax, unsigned int iYCoordMax, unsigned int iZCoordMax,
    vtkPolyData* iContourNodes,int ContourID )
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

  contour_row.SetField< int >("ContourID",ContourID);

  UpdateContourInDB(this->m_DatabaseConnector,contour_row);
  
  this->UpdateTableWidgetForAnExistingTrace("contour",ContourID);

  CloseDBConnection();
  return ContourID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::GetContentAndDisplayFromDB( std::string iTraceName)
{
  this->SetCurrentlyUsedTraceData(iTraceName);
  //Get the column names to be displayed in the table widget:
  std::list<std::string> ColumnsNames =
    this->m_CurrentlyUsedCollectionOfTraces->GetListColumnsNamesForTableWidget();
  this->m_CurrentlyUsedTable->DisplayColumnNames(
    iTraceName.c_str(),ColumnsNames);
  this->DBTabWidget->addTab(this->m_CurrentlyUsedTable,iTraceName.c_str());
  DBTableWidgetContainerType Row_Container =
    this->m_CurrentlyUsedCollectionOfTraces->GetRowContainer(m_DatabaseConnector);
  this->m_CurrentlyUsedTable->DisplayContent( this->m_CurrentlyUsedCollectionOfTraces->GetLinkToRowContainer(),
    iTraceName,this->m_CurrentlyUsedCollectionName);
  this->m_CurrentlyUsedTable->setSortingEnabled(true);
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
  GetListExistingCollectionIDFromDB(std::string TraceName)
{
  OpenDBConnection();
  this->SetCurrentlyUsedTraceData(TraceName);
  std::list<std::pair<std::string, QColor> > oListCollectionIDs;
  //First, build the query with selected fields and table to join with on conditions:
  std::vector<std::string> SelectFields;
  std::string CollectionID = this->m_CurrentlyUsedCollectionName;
  CollectionID += ".";
  CollectionID += this->m_CurrentlyUsedCollectionIDName;
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
  std::string OnCondition = this->m_CurrentlyUsedCollectionName;
  OnCondition += ".ColorID = color.ColorID";
   JoinTablesOnTraceTable.push_back(OnCondition);

 //Get the results for the query:
  std::vector<std::vector<std::string> >ResultsQuery  = GetValuesFromSeveralTables(
    this->m_DatabaseConnector,this->m_CurrentlyUsedCollectionName,SelectFields, "ImagingSessionID",
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
    ColorName += this->m_CurrentlyUsedCollectionName;
    ColorName += "Color";
    if (this->m_CurrentlyUsedCollectionName == "mesh")
      {
      Red   = 255;
      Green = 255;
      Blue  = 0;
      Alpha = 255;
      }
    if (this->m_CurrentlyUsedCollectionName == "track")
      {
      Red   = 0;
      Green = 255;
      Blue  = 255;
      Alpha = 255;
      }
    if (this->m_CurrentlyUsedCollectionName == "lineage")
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
    NewID = this->m_CurrentlyUsedCollectionOfTraces->CreateCollectionWithNoTraces(this->m_DatabaseConnector,
      FirstCollection);
    //add a new row in the table widget with the data corresponding to the new collection:
    this->SetCurrentlyUsedTraceData(this->m_CurrentlyUsedCollectionName);
    this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace(this->m_CurrentlyUsedTraceName);

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
  std::string iTraceName)
{
  this->SetCurrentlyUsedTraceData(iTraceName);
  GoDBTableWidgetContainer* LinkToNewTrace = this->m_CurrentlyUsedCollectionOfTraces
    ->GetLinkToNewCreatedTraceContainer(this->m_DatabaseConnector);
  //update the RowContainer for the trace:
  this->m_CurrentlyUsedCollectionOfTraces->GetLinkToRowContainer()
    ->InsertNewCreatedTrace(*LinkToNewTrace);

  //Update the trace table widget with the new trace:
  this->m_CurrentlyUsedTable->setSortingEnabled(false);
  this->m_CurrentlyUsedTable->InsertNewRow( LinkToNewTrace,iTraceName,
    this->m_CurrentlyUsedCollectionName);
  this->m_CurrentlyUsedTable->setSortingEnabled(true);

  //update the bounding box of the collection:
    //Get the ID of the last created trace in the database:
  std::list<int> ListNewTraces;
  int NewTraceID = MaxValueForOneColumnInTable(this->m_DatabaseConnector,
    this->m_CurrentlyUsedTraceIDName,iTraceName);
  ListNewTraces.push_back(NewTraceID);
  int CollectionID = FindOneID(this->m_DatabaseConnector,iTraceName,
    this->m_CurrentlyUsedCollectionIDName,this->m_CurrentlyUsedTraceIDName, 
    ConvertToString<int>(NewTraceID));
    //update the database:
  this->m_CurrentlyUsedCollectionOfTraces->
    UpdateDBDataForAddedTracesToExistingCollection(ListNewTraces,CollectionID,
    this->m_DatabaseConnector);
    //update the table widget for the collection:
  this->UpdateTableWidgetForAnExistingTrace(
    this->m_CurrentlyUsedCollectionName,CollectionID);
  
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
  this->SetCurrentlyUsedTraceData(iTraceName);
  int NewCollectionID = this->m_CurrentlyUsedCollectionOfTraces->CreateCollectionWithNoTraces(
    this->m_DatabaseConnector,NewCollection);

  std::pair<std::string,QColor> NewCollectionData;
  NewCollectionData.first = ConvertToString<int>(NewCollectionID);
  NewCollectionData.second = iColorNewCollection.second;

  //update the table of the collection (which become the trace then):
  this->SetCurrentlyUsedTraceData(this->m_CurrentlyUsedCollectionName);
  this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace(
    this->m_CurrentlyUsedTraceName);

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
  this->SetCurrentlyUsedTraceData(TraceName);
  std::list<int> SelectedTrace = this->m_CurrentlyUsedTable->GetListCheckedTraceID();
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
      TraceToReEdit( static_cast< unsigned int >( SelectedTrace.front() ) );
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::SetCurrentlyUsedTraceData(std::string iTraceName)
{
  this->m_CurrentlyUsedTraceName = iTraceName;
  this->m_CurrentlyUsedTable = this->GetTableWidgetChild(iTraceName);
  this->m_CurrentlyUsedTraceIDName = iTraceName;
  this->m_CurrentlyUsedTraceIDName += "ID";
  this->m_CurrentlyUsedCollectionOfTraces = this->GetCollectionOfTraces(iTraceName);
  this->m_CurrentlyUsedCollectionName = this->m_CurrentlyUsedCollectionOfTraces->CollectionName();
  this->m_CurrentlyUsedCollectionIDName = this->m_CurrentlyUsedCollectionName;
  this->m_CurrentlyUsedCollectionIDName += "ID";
  this->m_CurrentlyUsedCollectionOfName = this->m_CurrentlyUsedCollectionOfTraces->GetCollectionOf();
  this->m_CurrentlyUsedCollectionOfNameID = this->m_CurrentlyUsedCollectionOfName;
  this->m_CurrentlyUsedCollectionOfNameID += "ID";
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateTableWidgetForAnExistingTrace(
  std::string iTraceName, int iTraceID)
{
  this->SetCurrentlyUsedTraceData(iTraceName);
  GoDBTableWidgetContainer* LinkToUpdatedRow = this->m_CurrentlyUsedCollectionOfTraces
    ->GetLinkToUpdatedTraceContainer(this->m_DatabaseConnector,iTraceID);
  this->m_CurrentlyUsedTable->UpdateRow(LinkToUpdatedRow,iTraceID,iTraceName,
    this->m_CurrentlyUsedCollectionName);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddATraceToContourMeshInfo(std::string iTraceName,
  int iTraceID)
{ 
  this->SetCurrentlyUsedTraceData(iTraceName);
  if (iTraceName == "contour")
    {
    this->m_ContoursInfo.push_back(GetTraceInfoFromDB(
      this->m_DatabaseConnector, iTraceName,
      this->m_CurrentlyUsedCollectionName,iTraceID));
    }
  if (iTraceName == "mesh")
    {
    this->m_MeshesInfo.push_back(GetTraceInfoFromDB(
      this->m_DatabaseConnector, iTraceName,
      this->m_CurrentlyUsedCollectionName,iTraceID));
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTraceInContourMeshStructure(int iTraceID,
  std::vector<ContourMeshStructure> &iTraceInfo )
{
  std::vector<ContourMeshStructure>::iterator iter = iTraceInfo.begin();
  bool FoundErase = false;
  while(!FoundErase )
    {
    if (iter->TraceID == iTraceID)
      {
      iTraceInfo.erase(iter);
      FoundErase = true;
      }
    if (!FoundErase)
      {
      iter++;
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
