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

#include "QGoTrackEditingWidget.h"

#include "ContourMeshStructure.cxx"

#include "vtkPolyDataMapper.h"
#include "vtkLabeledDataMapper.h"

#include "vtkPoints.h"
#include "vtkPolyLine.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"

#include "vtkActor2D.h"
#include "vtkProperty.h"

// visu
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRendererCollection.h"

#include "vtkViewImage3DCommand.h"
#include "itkMacro.h"

#include <limits>

//-------------------------------------------------------------------------
QGoTrackEditingWidget::
QGoTrackEditingWidget( MeshContainer* imeshContainer, QWidget *iParent ) :
  QDialog(iParent), m_MeshContainer( imeshContainer ),
  m_MaxTrackID( 0 ), m_NumberOfTracks( 0 ), m_SecondClick( false )
{
  this->setupUi(this);

  renderer = vtkSmartPointer<vtkRenderer>::New();

  m_InteractorStyle3D   = vtkInteractorStyleImage3D::New();

  m_VtkEventQtConnector = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  m_VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject * >( m_InteractorStyle3D ),
    vtkViewImage3DCommand::MeshPickingEvent,
    this, SLOT( updateCurrentActorSelection(vtkObject *) ) );

  QObject::connect (this->buttonBox , SIGNAL( accepted() ),
                 this, SLOT( restoreTrackIDs() ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTrackEditingWidget::
~QGoTrackEditingWidget()
{
  m_InteractorStyle3D->Delete();

  ActorMeshIDMapIterator  it = m_Line2MeshID.begin();
  while( it != m_Line2MeshID.end() )
    {
    renderer->RemoveActor( it->first );
    it->first->Delete();
    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkActor*
QGoTrackEditingWidget::
createPolylineActor( double* iCenter1, double* iCenter2,
    const double* iColor1, const double* iColor2)
{
  //create a vtkPoints object and storevtkRenderWindow the points in it
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  points->InsertNextPoint(iCenter1);
  points->InsertNextPoint(iCenter2);

  vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
  polyLine->GetPointIds()->SetNumberOfIds(2);
  polyLine->GetPointIds()->SetId(0,0);
  polyLine->GetPointIds()->SetId(1,1);

  //Create a cell array to store the lines in and add the lines to it
  vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
  cells->InsertNextCell(polyLine);

  //Create a polydata to store everything in
  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();

  //add the points to the dataset
  polyData->SetPoints(points);

  //add the lines to the dataset
  polyData->SetLines(cells);

  //setup actor and mapper
  vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInput(polyData);

  vtkActor* actor = vtkActor::New();
  actor->SetMapper(mapper);

  return actor;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
updateCurrentActorSelection(vtkObject *caller)
{
  vtkInteractorStyleImage3D *t =
    static_cast< vtkInteractorStyleImage3D * >( caller );

  m_CurrentActor = vtkActor::
                   SafeDownCast( t->GetCurrentProp() );

  // if we click on the background
  if( !m_CurrentActor )
    {
    return;
    }

  ActorMeshIDMapIterator polyToMeshID =
      m_Line2MeshID.find(m_CurrentActor);

  if(polyToMeshID != m_Line2MeshID.end())
    {
    if( m_CurrentActor->GetProperty()->GetOpacity() == 1 )
      {
      //Update track IDs - CUT
      cutTrack( m_CurrentActor );
      }
    else
      {
      m_CurrentActor->GetProperty()->SetOpacity(1);
      // Update Track IDs - MERGE
      }
    }
  else
    {
    // if click the 2nd mesh
    ActorMeshIDMapIterator iter = m_Actor2MeshID.find( m_CurrentActor );
    if( iter != m_Actor2MeshID.end() )
      {
      if(m_SecondClick)
        {
        unsigned int secondMeshID = iter->second;
        mergeTrack( m_FirstMeshID, secondMeshID);
        }
      else
        {
        m_FirstMeshID = iter->second;
        m_FirstMeshActor = m_CurrentActor;
        }
      HighlightFirstActor( !m_SecondClick );
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
reassignTrackIDs()
{
  MeshContainer::MultiIndexContainerCollectionIDIterator c_it, c_end;

    c_it = m_MeshContainer->m_Container.get< CollectionID >().begin();
    unsigned int collection = std::numeric_limits< unsigned int >::max();

    c_end = m_MeshContainer->m_Container.get< CollectionID >().end();
    unsigned int current_track = 0;

    while( c_it != c_end )
      {
      unsigned int temp_collection = c_it->CollectionID;
      unsigned int traceID = c_it->TraceID;
      ++c_it;

      if( temp_collection != collection )
        {
        collection = temp_collection;
        current_track = m_NumberOfTracks;
        m_TrackIDsMapping[current_track] = collection;
        ++m_NumberOfTracks;
        }

      if( temp_collection > m_MaxTrackID)
        {
        m_MaxTrackID = temp_collection;
        }

      ModifyMeshCollectionID(traceID, current_track);
      }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
initializeVisualization()
{
  reassignTrackIDs();

  vtkSmartPointer<vtkDoubleArray> randomScalars =
    vtkSmartPointer< vtkDoubleArray >::New();
  randomScalars->SetNumberOfComponents(1);
  randomScalars->SetName("TimePoint");

  vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();

  // Create the actors
  for( unsigned int i = 0; i < m_NumberOfTracks ; ++i )
    {
    MeshContainer::MultiIndexContainerCollectionIDIterator it0, it1;

    boost::tuples::tie(it0, it1) =
      m_MeshContainer->m_Container.get< CollectionID >().equal_range( i );

    while( it0 != it1 )
      {
      ContourMeshStructure tempStructure(*it0);

      // Get the polydata
      vtkPolyData* nodes = tempStructure.Nodes;
      double* rgba = tempStructure.rgba;
      unsigned int time = tempStructure.TCoord;

      //setup actor and mapper
      vtkSmartPointer<vtkPolyDataMapper> mapper =
          vtkSmartPointer<vtkPolyDataMapper>::New();
      mapper->SetInput( nodes );

      vtkActor* actor = vtkActor::New();
      actor->SetMapper( mapper );
      actor->GetProperty()->SetColor( rgba );
      tempStructure.ActorXY = actor;

      randomScalars->InsertNextTuple1( time );
      pts->InsertNextPoint(actor->GetCenter());

      // Add actor to visu
      renderer->AddActor(actor);

      m_Actor2MeshID[actor] = tempStructure.TraceID;

      m_MeshContainer->m_Container.get< CollectionID >().replace(it0, tempStructure);

      ++it0;
      }

    // Go through map to create polylines
    computeLineActors();
    }

  computeLabelActor(randomScalars, pts);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
computeLabelActor( vtkSmartPointer<vtkDoubleArray> iScalars,
    vtkSmartPointer<vtkPoints> iPts)
{
  vtkSmartPointer<vtkPolyData> labelData = vtkSmartPointer<vtkPolyData>::New();
  labelData->GetPointData()->SetScalars(iScalars);
  labelData->SetPoints(iPts);

  // The labeled data mapper will place labels at the points
  vtkSmartPointer<vtkLabeledDataMapper> labelMapper =
    vtkSmartPointer<vtkLabeledDataMapper>::New();
  labelMapper->SetFieldDataName("TimePoint");
  labelMapper->SetInput(labelData);
  labelMapper->SetLabelModeToLabelScalars();
  labelMapper->SetLabelFormat("%6.0f");

  vtkSmartPointer<vtkActor2D> isolabels =
    vtkSmartPointer<vtkActor2D>::New();
  isolabels->SetMapper(labelMapper);

  renderer->AddActor( isolabels );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
cutTrack( vtkActor* iActor)
{
  ActorMeshIDMapIterator it = m_Line2MeshID.find( iActor );

  // Find the mesh ID
  if( it != m_Line2MeshID.end() )
    {
    MeshContainer::MultiIndexContainerTraceIDIterator it0, it1;
    boost::tuples::tie(it0, it1) =
      m_MeshContainer->m_Container.get< TraceID >().equal_range( it->second );

    unsigned int tLimit = it0->TCoord;
    unsigned int collectionID = it0->CollectionID;

    // here check that the track is not a new track!
    std::map< unsigned int, TrackStatusType >::iterator
        stat_it = m_TrackStatus.find( collectionID );

    if( stat_it == m_TrackStatus.end() )
      {
      m_TrackStatus[ collectionID ] = UPDATED_TRACK;
      }
    MeshContainer::MultiIndexContainerCollectionIDIterator it2, it3;
    boost::tuples::tie(it2, it3) =
      m_MeshContainer->m_Container.get< CollectionID >().equal_range( collectionID );

    while( it2 != it3 )
      {
      unsigned int time = it2->TCoord;
      unsigned int traceID = it2->TraceID;
      ++it2;
      // change track ID if we are before the mesh
      if( time < tLimit )
        {
        ModifyMeshCollectionID(traceID, m_NumberOfTracks);
        m_TrackStatus[ m_NumberOfTracks ] = NEW_TRACK;
        }
      }
    ++m_NumberOfTracks;

    if( m_NumberOfTracks > m_MaxTrackID)
      {
      m_MaxTrackID = m_NumberOfTracks;
      }
    // update visu
    removeLineActors();
    computeLineActors();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
removeLineActors()
{
  ActorMeshIDMapIterator  it = m_Line2MeshID.begin();
  while( it != m_Line2MeshID.end() )
    {
    renderer->RemoveActor( it->first );
    it->first->Delete();
    ++it;
    }
  m_Line2MeshID.clear();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
computeLineActors()
{
  std::map<unsigned int, unsigned int> m_Time2MeshID;
  // For each track, create the actors
  for( unsigned int i = 0; i < m_NumberOfTracks ; ++i )
    {
    m_Time2MeshID.clear();

    MeshContainer::MultiIndexContainerCollectionIDIterator it0, it1;
    boost::tuples::tie(it0, it1) =
     m_MeshContainer->m_Container.get< CollectionID >().equal_range( i );

    while( it0 != it1 )
      {
      m_Time2MeshID[it0->TCoord] = (it0->TraceID);
      ++it0;
      }

    // Go through map to create polylines
    std::map<unsigned int, unsigned int>::iterator polyLIt = m_Time2MeshID.begin();
    if( polyLIt != m_Time2MeshID.end() )
      {
      vtkActor* firstActor = (m_MeshContainer->GetActorGivenTraceID( polyLIt->second ))[0];
      vtkActor* secondActor = NULL;
      ++polyLIt;

      while( polyLIt != m_Time2MeshID.end() )
        {
        secondActor = (m_MeshContainer->GetActorGivenTraceID( polyLIt->second ))[0];
        vtkActor* polyLine = createPolylineActor(firstActor->GetCenter(),
                                                 secondActor->GetCenter());

         ///\todo should color be hard coded? hoew to define it? - Nicolas
          double color[3] = {1, 1, 1};
        polyLine->GetProperty()->SetColor(color);
        // Add actor to visu
        renderer->AddActor( polyLine );

        // key is actor
        m_Line2MeshID[polyLine] = polyLIt->second;

        firstActor = secondActor;
        ++polyLIt;
        }
      }
    else
      {
      std::cout << "List is empty" << std::endl;
      }
    }

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
restoreTrackIDs()
{
  std::map< unsigned int, TrackStatusType >::iterator
      iter = m_TrackStatus.begin();
  unsigned int collection = 0;
  unsigned int real_collectionID = 0;

  m_ListOfNewTrack.clear();
  m_ListOfUpdatedTracks.clear();
  m_ListOfDeletedTracks.clear();

  while( iter != m_TrackStatus.end() )
    {
    collection = iter->first;

    switch( iter->second )
      {
      case NEW_TRACK:
        {
        std::list< unsigned int > list_meshid;

        MeshContainer::MultiIndexContainerCollectionIDIterator it0, it1;

        boost::tuples::tie(it0, it1) =
          m_MeshContainer->m_Container.get< CollectionID >().equal_range( collection );

        while( it0 != it1 )
          {
          list_meshid.push_back( it0->TraceID );
          ++it0;
          }
        m_ListOfNewTrack.push_back( list_meshid );
        break;
        }

      case UPDATED_TRACK:
        {
        std::map< unsigned int, unsigned int >::iterator
            id_map_it = m_TrackIDsMapping.find( collection );

        if( id_map_it == m_TrackIDsMapping.end() )
          {
          std::cout << "error!" <<std::endl;
          return;
          }
        else
          {
          real_collectionID = id_map_it->second;
          }

        std::list< unsigned int > list_meshid;

        MeshContainer::MultiIndexContainerCollectionIDIterator it0, it1;

        boost::tuples::tie(it0, it1) =
          m_MeshContainer->m_Container.get< CollectionID >().equal_range( collection );

        while( it0 != it1 )
          {
          list_meshid.push_back( it0->TraceID );
          ++it0;
          }
        m_ListOfUpdatedTracks[real_collectionID] = list_meshid;
        break;
        }

      case DELETED_TRACK:
        {
        std::map< unsigned int, unsigned int >::iterator
            id_map_it = m_TrackIDsMapping.find( collection );

        if( id_map_it == m_TrackIDsMapping.end() )
          {
          std::cout << "error!" <<std::endl;
          return;
          }
        else
          {
          real_collectionID = id_map_it->second;
          }

        m_ListOfDeletedTracks.push_back( real_collectionID );
        break;
        }
      }
    ++iter;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< std::list< unsigned int > >
QGoTrackEditingWidget::
GetListOfTracksToBeCreated()
  {
  return this->m_ListOfNewTrack;
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::map< unsigned int, std::list< unsigned int > >
QGoTrackEditingWidget::
GetListOfTracksToBeUpdated()
  {
  return this->m_ListOfUpdatedTracks;
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int >
QGoTrackEditingWidget::
GetListOfTracksToBeDeleted()
  {
  return this->m_ListOfDeletedTracks;
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
QGoTrackEditingWidget::
mergeTrack( const unsigned int& iFirstMesh, const unsigned int& iSecondMesh )
{
  unsigned int FirstCollectionID = 0;
  unsigned int SecondCollectionID = 0;

  try
    {
    FirstCollectionID = m_MeshContainer->GetCollectionIDOfGivenTraceID( iFirstMesh );
    }
  catch( const itk::ExceptionObject& e )
    {
    std::cout << "caught an exception: " <<std::endl;
    e.Print( std::cout );
    return false;
    }

  try
    {
    SecondCollectionID = m_MeshContainer->GetCollectionIDOfGivenTraceID( iSecondMesh );
    }
  catch( const itk::ExceptionObject& e )
    {
    std::cout << "caught an exception: " <<std::endl;
    e.Print( std::cout );
    return false;
    }

  if( FirstCollectionID != SecondCollectionID )
    {
    //Check if actors are border of track
    std::pair< std::pair<unsigned int, unsigned int>,
               std::pair<unsigned int, unsigned int> >
        border1 = getTrackBorders( FirstCollectionID );

    std::pair< std::pair<unsigned int, unsigned int>,
               std::pair<unsigned int, unsigned int> >
      border2 = getTrackBorders(SecondCollectionID);

    // Check for overlap
    if(    ( border2.first.second <= border1.second.second )
        && ( border1.first.second <= border2.second.second ) )
      {
      std::cout << " Tracks are overlaping" << std::endl;
      return false;
      }

    //Get Highest time to know which meshes we should update
    unsigned int trackToUpdate = 0;
    unsigned int trackToDelete = 0;
    if( border1.first.first < border2.first.first)
      {
      trackToDelete = FirstCollectionID;
      trackToUpdate = SecondCollectionID;
      }
    else
      {
      trackToUpdate = FirstCollectionID;
      trackToDelete = SecondCollectionID;
      }

    // C est la!!!
    m_TrackStatus[trackToDelete] = DELETED_TRACK;
    m_TrackStatus[trackToUpdate] = UPDATED_TRACK;

    // Change the ID of the track by the other one
    updateTracksIDs( trackToDelete, trackToUpdate );

    // update visu
    removeLineActors();
    computeLineActors();

    return true;
    }

  return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::pair< std::pair<unsigned int, unsigned int>,
           std::pair<unsigned int, unsigned int> >
QGoTrackEditingWidget::
getTrackBorders( const unsigned int& iCollectionID )
{
  MeshContainer::MultiIndexContainerCollectionIDIterator it0, it1;

  boost::tuples::tie(it0, it1) =
    m_MeshContainer->m_Container.get< CollectionID >().equal_range( iCollectionID );

  std::pair<unsigned int, unsigned int>
      minBorder( 0, std::numeric_limits<unsigned int>::max() );

  std::pair<unsigned int, unsigned int>
      maxBorder( 0, 0 );

  unsigned int time = 0;

  while( it0 != it1 )
    {
    time = it0->TCoord;

    if( minBorder.second > time )
      {
      minBorder.first = it0->TraceID;
      minBorder.second = time;
      }

    if( maxBorder.second < time )
      {
      maxBorder.first = it0->TraceID;
      maxBorder.second = time;
      }
    ++it0;
    }

  return std::make_pair( minBorder, maxBorder );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
updateTracksIDs( const unsigned int& iIDToDelete,
                 const unsigned int& iIDToUpdate)
{
  MeshContainer::MultiIndexContainerCollectionIDIterator it0, it1;
  boost::tuples::tie(it0, it1) =
    m_MeshContainer->m_Container.get< CollectionID >().equal_range( iIDToDelete );

  while( it0 != it1 )
    {
    unsigned int traceID = it0->TraceID;
    ++it0;
    ModifyMeshCollectionID(traceID, iIDToUpdate);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
init()
{
  initializeVisualization();

  //setup render window, renderer, and interactor
  this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);

  this->qvtkWidget->GetInteractor()->SetInteractorStyle(m_InteractorStyle3D);
  m_InteractorStyle3D->EnablePickMode();

  this->qvtkWidget->GetRenderWindow()->Render();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
ModifyMeshCollectionID( unsigned int iMeshID, unsigned int iCollectionID)
{
  MeshContainer::MultiIndexContainerTraceIDIterator
    it = m_MeshContainer->m_Container.get< TraceID >().find( iMeshID );
  ContourMeshStructure tempStructure(*it);
  tempStructure.CollectionID = iCollectionID;
  m_MeshContainer->m_Container.get< TraceID >().replace(it, tempStructure);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
HighlightFirstActor( bool iHighlight )
{
  m_FirstMeshActor->GetProperty()->SetSpecular( iHighlight );
  m_FirstMeshActor->GetProperty()->SetAmbient( iHighlight );
  m_SecondClick = iHighlight;
}
