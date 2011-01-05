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

#include "vtkSmartPointer.h"

#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"

#include "vtkPoints.h"
#include "vtkPolyLine.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"

#include "vtkProperty.h"

// visu
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkRendererCollection.h"

#include "vtkViewImage3DCommand.h"
#include "itkMacro.h"

#include "vtkLabeledDataMapper.h"

#include "vtkActor2D.h"

#include <limits>

//-------------------------------------------------------------------------
QGoTrackEditingWidget::
QGoTrackEditingWidget( MeshContainer* imeshContainer, QWidget *iParent ) :
  QDialog(iParent), m_MeshContainer( imeshContainer ),
  m_MaxTrackID( 0 ), m_NumberOfTracks( 0 ), m_FirstRender( true ),
  m_SecondClick( false )
{
  this->setupUi(this);

  renderer = vtkSmartPointer<vtkRenderer>::New();

  m_InteractorStyle3D   = vtkInteractorStyleImage3D::New();

  m_LabelData = vtkPolyData::New();

  m_VtkEventQtConnector = vtkSmartPointer<vtkEventQtSlotConnect>::New();
  m_VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject * >( m_InteractorStyle3D ),
    vtkViewImage3DCommand::MeshPickingEvent,
    this, SLOT( UpdateCurrentActorSelection(vtkObject *) ) );

  QObject::connect (this->buttonBox , SIGNAL( accepted() ),
                 this, SLOT( mapContainerIDs2RealIDs() ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTrackEditingWidget::
~QGoTrackEditingWidget()
{
  m_InteractorStyle3D->Delete();

  if( m_LabelData )
    {
    if( m_LabelData->GetPointData() )
      {
    m_LabelData->GetPointData()->Reset();
      }
    if( m_LabelData->GetPoints() )
      {
    m_LabelData->GetPoints()->Reset();
      }
    if( m_LabelData->GetLines() )
      {
    m_LabelData->GetLines()->Reset();
      }
    if( m_LabelData )
      {
    m_LabelData->Reset();
      }
    }
  m_LabelData->Delete();

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
CreatePolylineActor( double* iCenter1, double* iCenter2,
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
preview()
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
UpdateCurrentActorSelection(vtkObject *caller)
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
    if(m_SecondClick)
      {
      // if click the 1st mesh
      ActorMeshIDMapIterator iter = m_Actor2MeshID.find( m_CurrentActor );

      if( iter != m_Actor2MeshID.end() )
        {
        m_SecondMeshID = iter->second;
        m_SecondMeshActor = m_CurrentActor;
        mergeTrack( m_FirstMeshID, m_SecondMeshID);
        }

      m_FirstMeshActor->GetProperty()->SetSpecular(0);
      m_FirstMeshActor->GetProperty()->SetAmbient(0);
      m_SecondClick = false;
      }
    else
      {
      // if click the 1st mesh
      ActorMeshIDMapIterator iter = m_Actor2MeshID.find( m_CurrentActor );

      if( iter != m_Actor2MeshID.end() )
        {
        m_FirstMeshID = iter->second;
        m_FirstMeshActor = m_CurrentActor;
        m_FirstMeshActor->GetProperty()->SetSpecular(1);
        m_FirstMeshActor->GetProperty()->SetAmbient(1);
        m_SecondClick = true;
        }
      }
    }
}

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
setMeshContainer( MeshContainer* imeshContainer )
{
  m_MeshContainer = imeshContainer;
}
//-------------------------------------------------------------------------
// Go through all container and creates actors
//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
initializeVisualization()
{
  //std::list<unsigned int> listOfTrackIDs = m_MeshContainer->GetAllCollectionIDs();
  //std::list<unsigned int>::iterator trackIDsIt = listOfTrackIDs.begin();

   MeshContainer::MultiIndexContainerCollectionIDIterator c_it, c_end;

  // first render: reassign track IDs
  if(m_FirstRender)
    {
    c_it = m_MeshContainer->m_Container.get< CollectionID >().begin();
    unsigned int collection = std::numeric_limits< unsigned int >::max();

    c_end = m_MeshContainer->m_Container.get< CollectionID >().end();
    unsigned int current_track = 0;

    while( c_it != c_end )
      {
      unsigned int temp_collection = c_it->CollectionID;

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
      // to be more efficient here, it would be better to use method from
      // boost::multi_index_container
      m_MeshContainer->ResetCurrentElement();
      m_MeshContainer->UpdateCurrentElementFromExistingOne< CollectionID >( c_it );
      m_MeshContainer->SetCurrentElementCollectionID( current_track );
      m_MeshContainer->InsertCurrentElement();

      ++c_it;
      }
    }
  ////////////////////////////////////////////////////////////////////////////////
  // FOR THE LABELS
  ////////////////////////////////////////////////////////////////////////////////
  vtkSmartPointer<vtkDoubleArray> randomScalars =
    vtkSmartPointer< vtkDoubleArray >::New();
  randomScalars->SetNumberOfComponents(1);
  randomScalars->SetName("TimePoint");

  vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
  ////////////////////////////////////////////////////////////////////////////////



  // For each track, create the actors
  for( unsigned int i = 0; i < m_NumberOfTracks ; ++i )
    {
    std::cout<< "collection ID: " << i << std::endl;
    m_Time2MeshID.clear();
    // Create the meshes actor if first render
    // update the container

    std::list<unsigned int> listOfMeshIDs =
        m_MeshContainer->GetAllTraceIDsGivenCollectionID( i );
    std::list<unsigned int>::iterator listOfMeshIDsIt = listOfMeshIDs.begin();

    while( listOfMeshIDsIt != listOfMeshIDs.end() )
      {
      m_MeshContainer->ResetCurrentElement();
      m_MeshContainer->UpdateCurrentElementFromExistingOne( (*listOfMeshIDsIt) );
      std::cout<< "mesh ID: " << (*listOfMeshIDsIt) << std::endl;

      // Get the polydata
      // Might need a deep copy
      vtkPolyData* nodes = m_MeshContainer->GetCurrentElementNodes();
      double* rgba = m_MeshContainer->GetCurrentElementColor();
      unsigned int time = m_MeshContainer->GetCurrentElementTimePoint();

      if( m_FirstRender ){
      //setup actor and mapper
      vtkSmartPointer<vtkPolyDataMapper> mapper =
          vtkSmartPointer<vtkPolyDataMapper>::New();
      mapper->SetInput( nodes );
      vtkActor* actor = vtkActor::New();
      actor->SetMapper( mapper );
      actor->GetProperty()->SetColor( rgba );

      std::cout<< "center: " << std::endl;
      std::cout<< "X: " << actor->GetCenter()[0] << std::endl;
      std::cout<< "Y: " << actor->GetCenter()[1] << std::endl;
      std::cout<< "Z: " << actor->GetCenter()[2] << std::endl;
      std::cout<< "T: " << time << std::endl;

      randomScalars->InsertNextTuple1( time );
      pts->InsertNextPoint(actor->GetCenter());

      // Add actor to visu
      renderer->AddActor(actor);

      /// \todo Find a better solution - Nicolas

      std::vector< vtkActor * > listOfActors; // to satisfy API
      listOfActors.push_back( actor );
      vtkActor* actor1 = vtkActor::New();
      listOfActors.push_back( actor1 );
      vtkActor* actor2 = vtkActor::New();
      listOfActors.push_back( actor2 );
      vtkActor* actor3 = vtkActor::New();
      listOfActors.push_back( actor3 );

      m_MeshContainer->UpdateCurrentElementFromVisu( listOfActors,
                                       nodes,
                                       0,       //time - not used
                                       false,   //highlighted
                                       false ); // visible

      m_Actor2MeshID[actor] = (*listOfMeshIDsIt);
      }

      // Fill map - to construct lines
      m_Time2MeshID[time] = (*listOfMeshIDsIt);

      // Insert Element
      m_MeshContainer->InsertCurrentElement();

      ++listOfMeshIDsIt;
      }
    //   }

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
        vtkActor* polyLine = CreatePolylineActor(firstActor->GetCenter(),
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

  if(m_FirstRender)
    {
  m_LabelData->GetPointData()->SetScalars(randomScalars);
  m_LabelData->SetPoints(pts);

  // The labeled data mapper will place labels at the points
  vtkSmartPointer<vtkLabeledDataMapper> labelMapper =
    vtkSmartPointer<vtkLabeledDataMapper>::New();
  labelMapper->SetFieldDataName("TimePoint");
  labelMapper->SetInput(m_LabelData);
  labelMapper->SetLabelModeToLabelScalars();
  labelMapper->SetLabelFormat("%6.0f");

  vtkSmartPointer<vtkActor2D> isolabels =
    vtkSmartPointer<vtkActor2D>::New();
  isolabels->SetMapper(labelMapper);

  renderer->AddActor( isolabels );
  m_FirstRender = false;
  }
}
//-------------------------------------------------------------------------
// Go through all container and creates actors
//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
cutTrack( vtkActor* iActor)
{
  ActorMeshIDMapIterator it = m_Line2MeshID.find( iActor );
  //int timePoint;

  // Find the mesh ID
  if( it != m_Line2MeshID.end() )
    {
    unsigned int collectionID =
         m_MeshContainer->GetCollectionIDOfGivenTraceID( it->second );

    std::cout << "Cut collection: " << collectionID << std::endl;

    std::list<unsigned int> listOfMeshIDs =
        m_MeshContainer->GetAllTraceIDsGivenCollectionID( collectionID );

    // here check that the track is not a new track!
    std::map< unsigned int, TrackStatusType >::iterator
        stat_it = m_TrackStatus.find( collectionID );

    if( stat_it == m_TrackStatus.end() )
      {
      m_TrackStatus[ collectionID ] = UPDATED_TRACK;
      }

    std::list<unsigned int>::iterator iterator = listOfMeshIDs.begin();

    // border time point
    m_MeshContainer->ResetCurrentElement();
    m_MeshContainer->UpdateCurrentElementFromExistingOne( it->second );
    unsigned int tLimit = m_MeshContainer->GetCurrentElementTimePoint();
    m_MeshContainer->InsertCurrentElement();

    std::cout << "Time limit: " << tLimit << std::endl;

    while( iterator != listOfMeshIDs.end() )
      {
      m_MeshContainer->ResetCurrentElement();
      m_MeshContainer->UpdateCurrentElementFromExistingOne( (*iterator) );

      unsigned int time = m_MeshContainer->GetCurrentElementTimePoint();

      // change track ID if we are before the mesh
      if( time < tLimit )
        {
        // Collection ID
        m_MeshContainer->SetCurrentElementCollectionID( m_NumberOfTracks );
        m_TrackStatus[ m_NumberOfTracks ] = NEW_TRACK;
        // Visibility: i.e. modified
        }
      m_MeshContainer->InsertCurrentElement();
      ++iterator;
      }

    ++m_NumberOfTracks;

    if( m_NumberOfTracks > m_MaxTrackID)
      {
      m_MaxTrackID = m_NumberOfTracks;
      }

    std::cout << "m_NumberOfTracks " << m_NumberOfTracks << std::endl;

    // update visu
    removeLineActors();
    initializeVisualization();
    }
}
//-------------------------------------------------------------------------
// Go through all container and creates actors
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
// ONLY CALLED AT THE END
//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
mapContainerIDs2RealIDs()
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

std::list< std::list< unsigned int > >
QGoTrackEditingWidget::
GetListOfTracksToBeCreated()
  {
  return this->m_ListOfNewTrack;
  }

std::map< unsigned int, std::list< unsigned int > >
QGoTrackEditingWidget::
GetListOfTracksToBeUpdated()
  {
  return this->m_ListOfUpdatedTracks;
  }

std::list< unsigned int >
QGoTrackEditingWidget::
GetListOfTracksToBeDeleted()
  {
  return this->m_ListOfDeletedTracks;
  }

//-------------------------------------------------------------------------
// ONLY CALLED AT THE END
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
        border1 = GetTrackBorders( FirstCollectionID );

    std::pair< std::pair<unsigned int, unsigned int>,
               std::pair<unsigned int, unsigned int> >
      border2 = GetTrackBorders(SecondCollectionID);

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

    std::cout<< " Mesh to update: " << trackToUpdate << std::endl;
    std::cout<< " Mesh to delete: " << trackToDelete << std::endl;

    // Change the ID of the track by the other one
    updateTracksIDs( trackToDelete, trackToUpdate );

    // update visu
    removeLineActors();
    initializeVisualization();

    return true;
    }

  return false;
}
//-------------------------------------------------------------------------
// ONLY CALLED AT THE END
//-------------------------------------------------------------------------
std::pair< std::pair<unsigned int, unsigned int>,
           std::pair<unsigned int, unsigned int> >
QGoTrackEditingWidget::
GetTrackBorders( const unsigned int& iCollectionID )
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

void
QGoTrackEditingWidget::
updateTracksIDs( const unsigned int& iIDToDelete,
                 const unsigned int& iIDToUpdate)
{
  std::list<unsigned int> listOfMeshIDs =
      m_MeshContainer->GetAllTraceIDsGivenCollectionID( iIDToDelete );
  std::list<unsigned int>::iterator iterator = listOfMeshIDs.begin();

  while( iterator != listOfMeshIDs.end() )
    {
    m_MeshContainer->ResetCurrentElement();
    m_MeshContainer->UpdateCurrentElementFromExistingOne( (*iterator) );
    m_MeshContainer->SetCurrentElementCollectionID( iIDToUpdate );
    m_MeshContainer->InsertCurrentElement();
    ++iterator;
    }
}
