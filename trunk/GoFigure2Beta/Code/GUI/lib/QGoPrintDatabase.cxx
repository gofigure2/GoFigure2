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
  QDockWidget( iParent ),
  m_DatabaseConnector( 0 ),
  m_IsDatabaseUsed( false )
{
  this->setupUi( this );

  DBTabWidget->setTabPosition( QTabWidget::West );
  DBTabWidget->setTabShape( QTabWidget::Triangular );
  DBTabWidget->removeTab( 0 );
  
  m_ContoursData = new TraceInfoStructure("contour",this);
  m_MeshesData = new TraceInfoStructure("mesh",this);
  m_TracksData = new TraceInfoStructure("track",this);
  m_LineagesData = new TraceInfoStructure("lineage",this);

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
  QObject::connect(this->m_ContoursData->Table, SIGNAL (CheckedRowsChanged()),
      this, SLOT(ChangeTracesToHighLightInfoFromTableWidget()));

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoPrintDatabase::~QGoPrintDatabase()
{
  // Need to release memory allocated for contours, meshes and tracks
  // contours
 // std::vector<ContourMeshStructure>::iterator c_it = m_ContoursInfo.begin();
  if (this->m_ContoursData->ListTracesInfoForVisu != 0)
    {
    std::vector<ContourMeshStructure>::iterator c_it = 
      this->m_ContoursData->ListTracesInfoForVisu->begin();
    while( c_it != this->m_ContoursData->ListTracesInfoForVisu->end() )
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
    }
  // meshes
  //std::vector<ContourMeshStructure>::iterator m_it = m_MeshesInfo.begin();
  if (this->m_MeshesData->ListTracesInfoForVisu != 0)
    {
    std::vector<ContourMeshStructure>::iterator m_it = 
      this->m_MeshesData->ListTracesInfoForVisu->begin();

    while( m_it != this->m_MeshesData->ListTracesInfoForVisu->end() )
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
  delete m_ContoursData;
  delete m_MeshesData;
  delete m_TracksData;
  delete m_LineagesData;
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

  this->m_ContoursData->CollectionOfTraces->SetImgSessionID(m_ImgSessionID);
  this->m_MeshesData->CollectionOfTraces->SetImgSessionID(m_ImgSessionID);
  this->m_TracksData->CollectionOfTraces->SetImgSessionID(m_ImgSessionID);
  this->m_LineagesData->CollectionOfTraces->SetImgSessionID(m_ImgSessionID);
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

  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);

  ContextMenu->addAction(tr("Delete selected %1s").arg(TraceName.c_str()),
    this,SLOT(DeleteTraces()));
  ContextMenu->addAction(tr("Create a new %1").arg(CurrentlyUsedTraceData->CollectionName.c_str()),
    this,SLOT(CreateCorrespondingCollection()));
  ContextMenu->addAction(
    tr("Add to selected %1 : %2").arg(CurrentlyUsedTraceData->CollectionName.c_str())
    .arg(this->m_CurrentCollectionData.first.c_str()),this,SLOT(AddToSelectedCollection()));
  ContextMenu->addAction(tr("ReEdit the selected %1").arg(TraceName.c_str()),
    this,SLOT(ReEditTrace()));
  ContextMenu->addAction(tr("Copy Selection"),
    CurrentlyUsedTraceData->Table,SLOT(CopySelection()));
  ContextMenu->addAction(tr("Copy table"),CurrentlyUsedTraceData->Table,SLOT(CopyTable()));
  ContextMenu->exec(this->mapToGlobal(iPos));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTraces()
{
  std::string TraceName = this->InWhichTableAreWe();
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(
    TraceName);
  
  std::list<int> SelectedTraces = CurrentlyUsedTraceData->Table->GetListCheckedTraceID();
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
      std::vector<std::string> VectorDeletedValues;
      std::list<int>::iterator iter = SelectedTraces.begin();
      while(iter != SelectedTraces.end())
        {
        int ID = *iter;
        //delete the trace in the vector<ContourMeshStructure>:
        this->DeleteTraceInContourMeshStructure(ID, 
          CurrentlyUsedTraceData->ListTracesInfoForVisu);
        VectorDeletedValues.push_back(ConvertToString<int>(ID));
        iter++;
        }
       this->UpdateInfoForTracesToBeDeletedAsCollectionOf(TraceName,
         VectorDeletedValues);
       //delete traces in the row container:
       GoDBTableWidgetContainer* LinkToTracesContainer = CurrentlyUsedTraceData->CollectionOfTraces->GetLinkToRowContainer();
       LinkToTracesContainer->DeleteSelectedTraces(SelectedTraces);
       CurrentlyUsedTraceData->Table->DeleteSelectedRows(CurrentlyUsedTraceData->TraceNameID);
       //delete traces in database + update bounding box for collections and update table widget:
       this->DeleteTracesAsPartOfACollection(TraceName,SelectedTraces);
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
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);
  std::list<int> ListSelectedTraces = CurrentlyUsedTraceData->Table->GetListCheckedTraceID();
  if (ListSelectedTraces.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1 to be part of the new %2")
      .arg(CurrentlyUsedTraceData->TraceName.c_str() )
      .arg(CurrentlyUsedTraceData->CollectionName.c_str()));
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
    int NewCollectionID = CurrentlyUsedTraceData->CollectionOfTraces->CreateNewCollectionFromSelection(
      ListSelectedTraces,this->m_DatabaseConnector,NewCollection);

    //update the Collection Table and the row container with the new created Collection:
    CurrentlyUsedTraceData = this->GetTraceInfoStructure(CurrentlyUsedTraceData->CollectionName);
    this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace(CurrentlyUsedTraceData->TraceName);

    //update the Trace Table and the row container with the new Collection ID:
    CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);
    this->UpdateTableWidgetAndRowContainerWithNewCollectionID(CurrentlyUsedTraceData->TraceName,
      this->m_DatabaseConnector,NewCollectionID,this->m_CurrentColorData.second,ListSelectedTraces);

    CloseDBConnection();
    QString CollectionIDQString = ConvertToString<int>(NewCollectionID).c_str();
    emit NewCreatedCollection(this->m_CurrentColorData.second,CollectionIDQString);
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
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);
  std::list<int> ListSelectedTraces = CurrentlyUsedTraceData->Table->GetListCheckedTraceID();
  NeedCurrentSelectedCollectionID();

  if (ListSelectedTraces.empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please select at least one %1 to be part of the %2 %3")
      .arg(CurrentlyUsedTraceData->TraceName.c_str() )
      .arg(CurrentlyUsedTraceData->CollectionName.c_str() )
      .arg(this->m_CurrentCollectionData.first.c_str() ) );
    msgBox.exec();
    }
  else
    {
    OpenDBConnection();
    int CollectionID = atoi(this->m_CurrentCollectionData.first.c_str());
    QColor ColorCollection = this->m_CurrentCollectionData.second;
    GoDBTableWidgetContainer* LinkToRowContainerForTraces =
      CurrentlyUsedTraceData->CollectionOfTraces->GetLinkToRowContainer();

    //update the corresponding database data:
    std::list<int> ListCollectionsToUpdateInTableWidget = 
      CurrentlyUsedTraceData->CollectionOfTraces->
      UpdateDBDataForAddedTracesToExistingCollection(ListSelectedTraces,
      CollectionID,this->m_DatabaseConnector);
    
    //update the RowContainer for traces with the new ID for the selected traces:
    LinkToRowContainerForTraces->UpdateIDs(ListSelectedTraces,CollectionID);

    //update the Table Widget Display:
    CurrentlyUsedTraceData->Table->UpdateIDs(CollectionID,
      CurrentlyUsedTraceData->CollectionNameID,ColorCollection);
    //update the Collection Table with the new bounding box:
    CurrentlyUsedTraceData = this->GetTraceInfoStructure(CurrentlyUsedTraceData->CollectionName);
    this->UpdateTableWidgetForAnExistingTrace(CurrentlyUsedTraceData->TraceName,CollectionID); 
    //update the Collection Table with the old updated bounding box:
    if (!ListCollectionsToUpdateInTableWidget.empty())
      {
      std::list<int>::iterator iter = ListCollectionsToUpdateInTableWidget.begin();
      while (iter != ListCollectionsToUpdateInTableWidget.end())
        {
        int TraceID = *iter;
        this->UpdateTableWidgetForAnExistingTrace(CurrentlyUsedTraceData->TraceName,TraceID);
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
  /** \todo put it in the TraceInfoStructure and the GetTracesInforFromDB
  in the collection of traces?*/
  this->m_ContoursData->ListTracesInfoForVisu = 
    GetTracesInfoFromDB(DatabaseConnector,"contour","mesh",m_ImgSessionID);
  this->m_MeshesData->ListTracesInfoForVisu = 
    GetTracesInfoFromDB(DatabaseConnector,"mesh","track",m_ImgSessionID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::
ChangeTracesToHighLightInfoFromTableWidget()
{
  std::string TraceName = this->InWhichTableAreWe();
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(
    TraceName);
  CurrentlyUsedTraceData->Table->TracesToHighlight(
    TraceName,CurrentlyUsedTraceData->ListTracesInfoForVisu);
  emit SelectionContoursToHighLightChanged();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoPrintDatabase::
ChangeContoursToHighLightInfoFromVisu(
  std::list<int> iListContoursHighLightedInVisu, bool Reedit )
{
  /** \todo get the tracename from the visu dock widget*/
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure("contour");
  if (!Reedit)
    {
    if( !iListContoursHighLightedInVisu.empty())
      {
      std::list<int>::iterator it = iListContoursHighLightedInVisu.begin();

      while( it != iListContoursHighLightedInVisu.end() )
        {
        //for( unsigned int j = 0 ; j < m_ContoursInfo.size(); j++ )
        for( unsigned int j = 0 ; j < CurrentlyUsedTraceData->ListTracesInfoForVisu->size(); j++ )
          {
          bool temp_selected =
            ( static_cast< unsigned int >( *it ) == (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].TraceID );

          (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].Highlighted = temp_selected;
          CurrentlyUsedTraceData->Table->SetSelectRowTraceID( CurrentlyUsedTraceData->TraceName,
            (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].TraceID, temp_selected );
          }
        ++it;
        }
      }
    else
      {
      for( unsigned int j = 0 ; j < CurrentlyUsedTraceData->ListTracesInfoForVisu->size(); j++ )
        {
        (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].Highlighted = false;

        CurrentlyUsedTraceData->Table->SetSelectRowTraceID( CurrentlyUsedTraceData->TraceName,
          (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].TraceID, false);
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
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure("mesh");

  while( it != iListMeshesHighLightedInVisu.end() )
    {
    for( unsigned int j = 0 ; j < CurrentlyUsedTraceData->ListTracesInfoForVisu->size(); j++ )
      {
      bool temp_selected =
        ( static_cast< unsigned int >( *it ) == 
        (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].TraceID );

      (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].Highlighted = temp_selected;

      CurrentlyUsedTraceData->Table->SetSelectRowTraceID( CurrentlyUsedTraceData->TraceName,
        (*CurrentlyUsedTraceData->ListTracesInfoForVisu)[j].TraceID, temp_selected );
      }
      ++it;
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
  TraceInfoStructure* CurrentlyUsedTraceData = 
    this->GetTraceInfoStructure(iTraceName);
  //Get the column names to be displayed in the table widget:
  std::list<std::string> ColumnsNames =
    CurrentlyUsedTraceData->CollectionOfTraces->GetListColumnsNamesForTableWidget();
  CurrentlyUsedTraceData->Table->DisplayColumnNames(
    iTraceName.c_str(),ColumnsNames);
  this->DBTabWidget->addTab(CurrentlyUsedTraceData->Table,iTraceName.c_str());
  DBTableWidgetContainerType Row_Container =
    CurrentlyUsedTraceData->CollectionOfTraces->GetRowContainer(m_DatabaseConnector);
  CurrentlyUsedTraceData->Table->DisplayContent( CurrentlyUsedTraceData->CollectionOfTraces->GetLinkToRowContainer(),
    iTraceName,CurrentlyUsedTraceData->CollectionName);
  CurrentlyUsedTraceData->Table->setSortingEnabled(true);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure> QGoPrintDatabase::
  GetContoursForAGivenTimepoint(unsigned int iTimePoint)
{
  return GetTracesForAGivenTimepoint(
    *this->m_ContoursData->ListTracesInfoForVisu,iTimePoint);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure> QGoPrintDatabase::
  GetMeshesForAGivenTimepoint (unsigned int iTimePoint)
{
  return GetTracesForAGivenTimepoint(
    *this->m_MeshesData->ListTracesInfoForVisu, iTimePoint);
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
  return this->GetTracesForAGivenZCoord(*this->m_ContoursData->ListTracesInfoForVisu,iZCoord,
    this->m_ContoursData->CollectionOfTraces);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure> QGoPrintDatabase::
  GetMeshesForAGivenZCoord (unsigned int iZCoord)
{
  return this->GetTracesForAGivenZCoord(*this->m_MeshesData->ListTracesInfoForVisu,iZCoord,
    this->m_MeshesData->CollectionOfTraces);
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
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);
  std::list<std::pair<std::string, QColor> > oListCollectionIDs;
  //First, build the query with selected fields and table to join with on conditions:
  std::vector<std::string> SelectFields;
  std::string CollectionID = CurrentlyUsedTraceData->CollectionName;
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
  std::string JoinTable = "color";
  JoinTablesOnTraceTable.push_back(JoinTable);
  std::string OnCondition = CurrentlyUsedTraceData->CollectionName;
  OnCondition += ".ColorID = color.ColorID";
   JoinTablesOnTraceTable.push_back(OnCondition);

 //Get the results for the query:
  std::vector<std::vector<std::string> >ResultsQuery  = GetValuesFromSeveralTables(
    this->m_DatabaseConnector,CurrentlyUsedTraceData->CollectionName,SelectFields, "ImagingSessionID",
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
    }

  CloseDBConnection();

  return oListCollectionIDs;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::UpdateTableWidgetAndRowContainerWithNewCreatedTrace(
  std::string iTraceName)
{
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(iTraceName);
  GoDBTableWidgetContainer* LinkToNewTrace = CurrentlyUsedTraceData->CollectionOfTraces->
    GetLinkToNewCreatedTraceContainer(this->m_DatabaseConnector);
  //update the RowContainer for the trace:
  CurrentlyUsedTraceData->CollectionOfTraces->GetLinkToRowContainer()
    ->InsertNewCreatedTrace(*LinkToNewTrace);

  //Update the trace table widget with the new trace:
  CurrentlyUsedTraceData->Table->setSortingEnabled(false);
  CurrentlyUsedTraceData->Table->InsertNewRow( LinkToNewTrace,iTraceName,
    CurrentlyUsedTraceData->CollectionName);
  CurrentlyUsedTraceData->Table->setSortingEnabled(true);

  //update the bounding box of the collection:
    //Get the ID of the last created trace in the database:
  std::list<int> ListNewTraces;
  int NewTraceID = MaxValueForOneColumnInTable(this->m_DatabaseConnector,
    CurrentlyUsedTraceData->TraceNameID,iTraceName);
  ListNewTraces.push_back(NewTraceID);
  int CollectionID = FindOneID(this->m_DatabaseConnector,iTraceName,
    CurrentlyUsedTraceData->CollectionNameID,CurrentlyUsedTraceData->TraceNameID, 
    ConvertToString<int>(NewTraceID));
    //update the database:
  CurrentlyUsedTraceData->CollectionOfTraces->UpdateDBDataForAddedTracesToExistingCollection(
    ListNewTraces,CollectionID,this->m_DatabaseConnector);
    //update the table widget for the collection:
  this->UpdateTableWidgetForAnExistingTrace(
    CurrentlyUsedTraceData->CollectionName,CollectionID);
  
 }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
 void QGoPrintDatabase::UpdateTableWidgetAndRowContainerWithNewCollectionID(
   std::string iTraceName,vtkMySQLDatabase* DatabaseConnector,
   unsigned int iNewCollectionID,QColor iColorNewCollection,
   std::list<int> iListSelectedTraces)
{  
  TraceInfoStructure* TraceData = this->GetTraceInfoStructure(
    iTraceName);
  //Update the TraceTable with the new collectionID + color:
  TraceData->Table->UpdateIDs(iNewCollectionID,TraceData->CollectionNameID,
    iColorNewCollection);
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
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(iTraceName);
  int NewCollectionID = CurrentlyUsedTraceData->CollectionOfTraces->CreateCollectionWithNoTraces(
    this->m_DatabaseConnector,NewCollection);

  std::pair<std::string,QColor> NewCollectionData;
  NewCollectionData.first = ConvertToString<int>(NewCollectionID);
  NewCollectionData.second = iColorNewCollection.second;

  //update the table of the collection (which become the trace then):
  CurrentlyUsedTraceData = this->GetTraceInfoStructure(CurrentlyUsedTraceData->CollectionName);
  this->UpdateTableWidgetAndRowContainerWithNewCreatedTrace(CurrentlyUsedTraceData->TraceName);

  this->CloseDBConnection();
  return NewCollectionData;
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
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(TraceName);
  std::list<int> SelectedTrace = CurrentlyUsedTraceData->Table->GetListCheckedTraceID();
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
void QGoPrintDatabase::UpdateTableWidgetForAnExistingTrace(
  std::string iTraceName, int iTraceID)
{
  TraceInfoStructure* CurrentlyUsedTraceData = this->GetTraceInfoStructure(iTraceName);
  GoDBTableWidgetContainer* LinkToUpdatedRow = CurrentlyUsedTraceData->CollectionOfTraces->
    GetLinkToUpdatedTraceContainer(this->m_DatabaseConnector,iTraceID);
  CurrentlyUsedTraceData->Table->UpdateRow(LinkToUpdatedRow,iTraceID,iTraceName,
    CurrentlyUsedTraceData->CollectionName);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::AddATraceToContourMeshInfo(std::string iTraceName,
  int iTraceID)
{ 
  TraceInfoStructure* CurrentlyUsedTraceData = 
    this->GetTraceInfoStructure(iTraceName);

  CurrentlyUsedTraceData->ListTracesInfoForVisu->push_back(
    GetTraceInfoFromDB(
      this->m_DatabaseConnector, iTraceName,
      CurrentlyUsedTraceData->CollectionName,iTraceID));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoPrintDatabase::DeleteTraceInContourMeshStructure(int iTraceID,
  std::vector<ContourMeshStructure>* iTraceInfo )
{
  std::vector<ContourMeshStructure>::iterator iter = iTraceInfo->begin();
  
  while( iter != iTraceInfo->end() )
    {
    if( static_cast< int >( iter->TraceID ) == iTraceID)
      {
      iTraceInfo->erase(iter);
      break;
      }
    ++iter;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<ContourMeshStructure>* QGoPrintDatabase::
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
  std::cout<<"error, this tracename is not related to any TracesData"<<std::endl;

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
    while(iterSelec != iTracesAsCollectionToBeDeleted.end())
      {
      std::string TraceIDstr = *iterSelec;
      unsigned int TraceID =atoi(TraceIDstr.c_str());
      emit DeletedCollection(TraceID);
      iterSelec++;
      }
    //Get the belonging traces for the traces that are going to be deleted
    //and are collection of these 'belonging traces':
    std::vector<std::string> ListBelongingTraces = ListSpecificValuesForOneColumn(
      this->m_DatabaseConnector,CurrentlyUsedTraceData->CollectionOf,
      CurrentlyUsedTraceData->CollectionOfID,CurrentlyUsedTraceData->TraceNameID,
      iTracesAsCollectionToBeDeleted);

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
      CurrentlyUsedTraceData = this->GetTraceInfoStructure(CurrentlyUsedTraceData->CollectionOf);
      CurrentlyUsedTraceData->CollectionOfTraces->UpdateCollectionIDOfSelectedTraces(
        TracesWithCollectionToBeNull,0,this->m_DatabaseConnector);
      QColor Color(255,255,255,255);
      CurrentlyUsedTraceData->Table->UpdateIDs(0,CurrentlyUsedTraceData->CollectionNameID,Color,
        CurrentlyUsedTraceData->TraceNameID,TracesWithCollectionToBeNull); 
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
    int tempTraceID = *iterator;
    int tempCollectionID = FindOneID(this->m_DatabaseConnector,iTraceName,CurrentlyUsedTraceData->CollectionNameID,
      CurrentlyUsedTraceData->TraceNameID, ConvertToString<int>(tempTraceID));
    //save the current collection of traces corresponding to the trace before recentring to the collection:
    GoDBCollectionOfTraces* CollectionOfTraces = CurrentlyUsedTraceData->CollectionOfTraces;
    CollectionOfTraces->DeleteTraceInDB(tempTraceID,this->m_DatabaseConnector);
    //change the data who were related to the trace to the collection:
    CurrentlyUsedTraceData = this->GetTraceInfoStructure(CurrentlyUsedTraceData->CollectionName);
    if (CurrentlyUsedTraceData->CollectionName != "None")
      {
      if (tempCollectionID != 0)
        {
        CollectionOfTraces->RecalculateDBBoundingBox(this->m_DatabaseConnector,tempCollectionID);
        this->UpdateTableWidgetForAnExistingTrace(CurrentlyUsedTraceData->TraceName,tempCollectionID);
        }
      }
    iterator++;
    }
}

       