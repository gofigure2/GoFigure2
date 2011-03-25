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

#include "ContourMeshStructure.h"

#include "vtkSphereSource.h"

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

#include "vtkMath.h"

#include <limits>

//-------------------------------------------------------------------------
QGoTrackEditingWidget::QGoTrackEditingWidget(MeshContainer *imeshContainer, QWidget *iParent) :
  QDialog(iParent), m_MeshContainer(imeshContainer),
  m_MaxTrackID(0), m_NumberOfTracks(0), m_SecondClick(false)
{
  this->setupUi(this);

  renderer = vtkSmartPointer< vtkRenderer >::New();

  m_InteractorStyle3D   = vtkInteractorStyleImage3D::New();

  m_VtkEventQtConnector = vtkSmartPointer< vtkEventQtSlotConnect >::New();
  m_VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject * >( m_InteractorStyle3D ),
    vtkViewImage3DCommand::MeshPickingEvent,
    this, SLOT( updateCurrentActorSelection(vtkObject *) ) );

  m_MinimalDistance = std::numeric_limits< double >::max();

  // ADD A STATUS BAR
  m_StatusBar = new QStatusBar;
  this->gridLayout_2->addWidget(m_StatusBar, 1, 0, 1, 1);
  m_StatusBar->showMessage("No track selected");

  QObject::connect ( this->buttonBox, SIGNAL( accepted() ),
                     this, SLOT( restoreTrackIDs() ) );

  QObject::connect ( this->realMeshes, SIGNAL( toggled(bool) ),
                     this, SLOT( updateMeshesActors(bool) ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTrackEditingWidget::
~QGoTrackEditingWidget()
{
  m_InteractorStyle3D->Delete();

  LineActor2MeshIDIterator it = m_Line2MeshID.begin();
  while ( it != m_Line2MeshID.end() )
    {
    renderer->RemoveActor(it->first);
    it->first->Delete();
    ++it;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkActor *
QGoTrackEditingWidget::createPolylineActor(double *iCenter1, double *iCenter2,
                                           const double *iColor1, const double *iColor2)
{
  // Might be useful at some point
  (void)iColor1;
  (void)iColor2;
  //create a vtkPoints object and storevtkRenderWindow the points in it
  vtkSmartPointer< vtkPoints > points = vtkSmartPointer< vtkPoints >::New();
  points->InsertNextPoint(iCenter1);
  points->InsertNextPoint(iCenter2);

  vtkSmartPointer< vtkPolyLine > polyLine = vtkSmartPointer< vtkPolyLine >::New();
  polyLine->GetPointIds()->SetNumberOfIds(2);
  polyLine->GetPointIds()->SetId(0, 0);
  polyLine->GetPointIds()->SetId(1, 1);

  //Create a cell array to store the lines in and add the lines to it
  vtkSmartPointer< vtkCellArray > cells = vtkSmartPointer< vtkCellArray >::New();
  cells->InsertNextCell(polyLine);

  //Create a polydata to store everything in
  vtkSmartPointer< vtkPolyData > polyData = vtkSmartPointer< vtkPolyData >::New();

  //add the points to the dataset
  polyData->SetPoints(points);

  //add the lines to the dataset
  polyData->SetLines(cells);

  //setup actor and mapper
  vtkSmartPointer< vtkPolyDataMapper > mapper =
    vtkSmartPointer< vtkPolyDataMapper >::New();
  mapper->SetInput(polyData);

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  return actor;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::updateCurrentActorSelection(vtkObject *caller)
{
  vtkInteractorStyleImage3D *t =
    static_cast< vtkInteractorStyleImage3D * >( caller );

  m_CurrentActor = vtkActor::
    SafeDownCast( t->GetCurrentProp() );

  // if we click on the background
  if ( !m_CurrentActor )
    {
    return;
    }

  LineActor2MeshIDIterator polyToMeshID =
    m_Line2MeshID.find(m_CurrentActor);

  if ( polyToMeshID != m_Line2MeshID.end() )
    {
    if ( m_CurrentActor->GetProperty()->GetOpacity() == 1 )
      {
      //Update track IDs - CUT
      cutTrack(m_CurrentActor);
      }
    }
  else
    {
    MeshContainer::MultiIndexContainerTraceIDIterator iter, c_end;
    vtkIntArray* testArray =
        static_cast<vtkIntArray*>(m_CurrentActor->GetMapper()->GetInput()
            ->GetPointData()->GetArray("MESH"));
    iter = m_MeshContainer->m_Container.get< TraceID >().find( testArray->GetValue(0) );
    c_end = m_MeshContainer->m_Container.get< TraceID >().end();
    merge< MeshContainer::MultiIndexContainerTraceIDIterator >(iter, c_end);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TIterator >
void
QGoTrackEditingWidget::merge(TIterator iBegin, TIterator iEnd)
{
  if ( iBegin != iEnd )
    {
    if ( m_SecondClick )
      {
      mergeTrack(m_FirstMeshID, iBegin->TraceID);
      }
    else
      {
      m_FirstMeshID    = iBegin->TraceID;
      m_FirstMeshActor = m_CurrentActor;
      m_StatusBar->showMessage("Select another mesh to merge tracks");
      }
    highlightFirstActor(!m_SecondClick);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::reassignTrackIDs()
{
  MeshContainer::MultiIndexContainerCollectionIDIterator c_it, c_end;

  c_it = m_MeshContainer->m_Container.get< CollectionID >().begin();
  unsigned int collection = std::numeric_limits< unsigned int >::max();

  c_end = m_MeshContainer->m_Container.get< CollectionID >().end();
  unsigned int current_track = 0;

  while ( c_it != c_end )
    {
    unsigned int temp_collection = c_it->CollectionID;
    unsigned int traceID = c_it->TraceID;
    ++c_it;

    if ( temp_collection != collection )
      {
      collection = temp_collection;
      current_track = m_NumberOfTracks;

      TrackInformation track(collection, UPDATED_TRACK);
      m_TrackMapping[current_track] = track;

      ++m_NumberOfTracks;
      }

    if ( temp_collection > m_MaxTrackID )
      {
      m_MaxTrackID = temp_collection;
      }

    modifyMeshCollectionID(traceID, current_track);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::initializeVisualization()
{
  reassignTrackIDs();

  getClosestPoints();

  computeMeshActors();

  computeLineActors();

  computeLabelActor();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::computeMeshActors()
{
  // Create the actors
  for ( unsigned int i = 0; i < m_NumberOfTracks; ++i )
    {
    MeshContainer::MultiIndexContainerCollectionIDIterator it0, it1;

    boost::tuples::tie(it0, it1) =
      m_MeshContainer->m_Container.get< CollectionID >().equal_range(i);

    while ( it0 != it1 )
      {
      ContourMeshStructure tempStructure(*it0);

      // Get the polydata
      vtkPolyData *nodes = tempStructure.Nodes;
      double *     rgba = tempStructure.rgba;

      // add trace info in the polydata
      vtkSmartPointer<vtkIntArray> trackIDArray = vtkSmartPointer<vtkIntArray>::New();
      trackIDArray->SetNumberOfComponents(1);
      trackIDArray->SetNumberOfValues(1);
      trackIDArray->SetName("MESH");
      trackIDArray->SetValue(0,tempStructure.TraceID);

      nodes->GetPointData()->AddArray(trackIDArray);

      //setup actor and mapper
      vtkSmartPointer< vtkPolyDataMapper > mapper =
        vtkSmartPointer< vtkPolyDataMapper >::New();
      mapper->SetInput(nodes);

      vtkActor *actor = vtkActor::New();
      actor->SetMapper(mapper);
      actor->GetProperty()->SetColor(rgba);
      tempStructure.ActorXY = actor; // real mesh
      // generate sphere based on the minimal distance between 2 points in a
      // track
      // actors are invisible
      vtkActor *sphereActor = computeSphere(tempStructure.TraceID,
          actor->GetCenter(), m_MinimalDistance / 3);
      tempStructure.ActorXZ = sphereActor;

      // Add actor to visu
      renderer->AddActor(actor);
      renderer->AddActor(sphereActor);

      m_MeshContainer->m_Container.get< CollectionID >().replace(it0, tempStructure);

      ++it0;
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::computeLabelActor()
{
  vtkSmartPointer< vtkDoubleArray > randomScalars =
    vtkSmartPointer< vtkDoubleArray >::New();
  randomScalars->SetNumberOfComponents(1);
  randomScalars->SetName("TimePoint");

  vtkSmartPointer< vtkPoints > pts = vtkSmartPointer< vtkPoints >::New();

  // Create the actors
  for ( unsigned int i = 0; i < m_NumberOfTracks; ++i )
    {
    MeshContainer::MultiIndexContainerCollectionIDIterator it0, it1;

    boost::tuples::tie(it0, it1) =
      m_MeshContainer->m_Container.get< CollectionID >().equal_range(i);

    while ( it0 != it1 )
      {
      // Get the polydata
      vtkPolyData *nodes = it0->Nodes;
      unsigned int time = it0->TCoord;

      randomScalars->InsertNextTuple1(time);
      pts->InsertNextPoint( nodes->GetCenter() );

      ++it0;
      }
    }

  vtkSmartPointer< vtkPolyData > labelData = vtkSmartPointer< vtkPolyData >::New();
  labelData->GetPointData()->SetScalars(randomScalars);
  labelData->SetPoints(pts);

  // The labeled data mapper will place labels at the points
  vtkSmartPointer< vtkLabeledDataMapper > labelMapper =
    vtkSmartPointer< vtkLabeledDataMapper >::New();
  labelMapper->SetFieldDataName("TimePoint");
  labelMapper->SetInput(labelData);
  labelMapper->SetLabelModeToLabelScalars();
  labelMapper->SetLabelFormat("%6.0f");

  vtkSmartPointer< vtkActor2D > isolabels =
    vtkSmartPointer< vtkActor2D >::New();
  isolabels->SetMapper(labelMapper);

  renderer->AddActor(isolabels);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::cutTrack(vtkActor *iActor)
{
  LineActor2MeshIDIterator it = m_Line2MeshID.find(iActor);

  // Find the mesh ID
  if ( it != m_Line2MeshID.end() )
    {
    MeshContainer::MultiIndexContainerTraceIDIterator it0;
    it0 = m_MeshContainer->m_Container.get< TraceID >().find(it->second);

    unsigned int collectionID = it0->CollectionID;
    unsigned int tLimit = it0->TCoord;

    MeshContainer::MultiIndexContainerCollectionIDIterator it2, it3;
    boost::tuples::tie(it2, it3) =
      m_MeshContainer->m_Container.get< CollectionID >().equal_range(collectionID);

    while ( it2 != it3 )
      {
      unsigned int time = it2->TCoord;
      unsigned int traceID = it2->TraceID;
      ++it2;
      // change track ID if we are before the mesh
      if ( time < tLimit )
        {
        modifyMeshCollectionID(traceID, m_NumberOfTracks);

        TrackInformation track(0, NEW_TRACK);
        m_TrackMapping[m_NumberOfTracks] = track;
        }
      }
    ++m_NumberOfTracks;

    if ( m_NumberOfTracks > m_MaxTrackID )
      {
      m_MaxTrackID = m_NumberOfTracks;
      }
    // update visu
    removeLineActors();
    computeLineActors();
    }
  m_StatusBar->showMessage("Track splitted");
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::removeLineActors()
{
  LineActor2MeshIDIterator it = m_Line2MeshID.begin();

  while ( it != m_Line2MeshID.end() )
    {
    renderer->RemoveActor(it->first);
    it->first->Delete();
    ++it;
    }
  m_Line2MeshID.clear();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::computeLineActors()
{
  std::map< unsigned int, unsigned int > m_Time2MeshID;
  // For each track, create the actors
  for ( unsigned int i = 0; i < m_NumberOfTracks; ++i )
    {
    m_Time2MeshID.clear();

    MeshContainer::MultiIndexContainerCollectionIDIterator it0, it1;
    boost::tuples::tie(it0, it1) =
      m_MeshContainer->m_Container.get< CollectionID >().equal_range(i);

    while ( it0 != it1 )
      {
      m_Time2MeshID[it0->TCoord] = ( it0->TraceID );
      ++it0;
      }

    // Go through map to create polylines
    std::map< unsigned int, unsigned int >::iterator polyLIt = m_Time2MeshID.begin();
    if ( polyLIt != m_Time2MeshID.end() )
      {
      vtkActor *firstActor = ( m_MeshContainer->GetActorGivenTraceID(polyLIt->second) )[0];
      vtkActor *secondActor = NULL;
      ++polyLIt;

      while ( polyLIt != m_Time2MeshID.end() )
        {
        secondActor = ( m_MeshContainer->GetActorGivenTraceID(polyLIt->second) )[0];
        vtkActor *polyLine = createPolylineActor( firstActor->GetCenter(),
                                                  secondActor->GetCenter() );

        ///\todo should color be hard coded? hoew to define it? - Nicolas
        double color[3] = { 1, 1, 1 };
        polyLine->GetProperty()->SetColor(color);
        // Add actor to visu
        renderer->AddActor(polyLine);

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
QGoTrackEditingWidget::restoreTrackIDs()
{
  TrackMapping::iterator it = m_TrackMapping.begin();

  unsigned int collection = 0;

  m_ListOfNewTrack.clear();
  m_ListOfUpdatedTracks.clear();
  m_ListOfDeletedTracks.clear();

  while ( it != m_TrackMapping.end() )
    {
    collection = it->first;

    switch ( it->second.Status )
      {
      case NEW_TRACK:
        {
        std::list< unsigned int > list_meshid = getMeshIDsInTrack(collection);
        m_ListOfNewTrack.push_back(list_meshid);
        break;
        }

      case UPDATED_TRACK:
        {
        std::list< unsigned int > list_meshid = getMeshIDsInTrack(collection);
        m_ListOfUpdatedTracks[it->second.RealID] = list_meshid;
        break;
        }

      case DELETED_TRACK:
        {
        // if real ID > 0
        if ( it->second.RealID )
          {
          m_ListOfDeletedTracks.push_back(it->second.RealID);
          }
        break;
        }
      }
    ++it;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int >
QGoTrackEditingWidget::getMeshIDsInTrack(unsigned int iCollection)
{
  std::list< unsigned int > list_meshid;

  MeshContainer::MultiIndexContainerCollectionIDIterator it0, it1;

  boost::tuples::tie(it0, it1) =
    m_MeshContainer->m_Container.get< CollectionID >().equal_range(iCollection);

  while ( it0 != it1 )
    {
    list_meshid.push_back(it0->TraceID);
    ++it0;
    }
  return list_meshid;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< std::list< unsigned int > >
QGoTrackEditingWidget::GetListOfTracksToBeCreated()
{
  return this->m_ListOfNewTrack;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::map< unsigned int, std::list< unsigned int > >
QGoTrackEditingWidget::GetListOfTracksToBeUpdated()
{
  return this->m_ListOfUpdatedTracks;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int >
QGoTrackEditingWidget::GetListOfTracksToBeDeleted()
{
  return this->m_ListOfDeletedTracks;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
QGoTrackEditingWidget::mergeTrack(const unsigned int & iFirstMesh, const unsigned int & iSecondMesh)
{
  unsigned int FirstCollectionID = 0;
  unsigned int SecondCollectionID = 0;

  try
    {
    FirstCollectionID = m_MeshContainer->GetCollectionIDOfGivenTraceID(iFirstMesh);
    }
  catch ( const itk::ExceptionObject & e )
    {
    std::cout << "caught an exception: " << std::endl;
    e.Print(std::cout);
    return false;
    }

  try
    {
    SecondCollectionID = m_MeshContainer->GetCollectionIDOfGivenTraceID(iSecondMesh);
    }
  catch ( const itk::ExceptionObject & e )
    {
    std::cout << "caught an exception: " << std::endl;
    e.Print(std::cout);
    return false;
    }

  if ( FirstCollectionID != SecondCollectionID )
    {
    //Check if actors are border of track
    std::pair< std::pair< unsigned int, unsigned int >,
               std::pair< unsigned int, unsigned int > >
    border1 = getTrackBorders(FirstCollectionID);

    std::pair< std::pair< unsigned int, unsigned int >,
               std::pair< unsigned int, unsigned int > >
    border2 = getTrackBorders(SecondCollectionID);

    // Check for overlap
    if ( ( border2.first.second <= border1.second.second )
         && ( border1.first.second <= border2.second.second ) )
      {
      m_StatusBar->showMessage("Can't merge tracks: overlap");
      return false;
      }

    //Get Highest time to know which meshes we should update
    unsigned int trackToUpdate = 0;
    unsigned int trackToDelete = 0;
    if ( border1.first.first < border2.first.first )
      {
      trackToDelete = FirstCollectionID;
      trackToUpdate = SecondCollectionID;
      }
    else
      {
      trackToUpdate = FirstCollectionID;
      trackToDelete = SecondCollectionID;
      }

    m_TrackMapping[trackToDelete].Status = DELETED_TRACK;

    // Change the ID of the track by the other one
    updateTracksIDs(trackToDelete, trackToUpdate);

    // update visu
    removeLineActors();
    computeLineActors();

    m_StatusBar->showMessage("Tracks merged");

    return true;
    }

  m_StatusBar->showMessage("Meshes belong to same track");
  return false;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::pair< std::pair< unsigned int, unsigned int >,
           std::pair< unsigned int, unsigned int > >
QGoTrackEditingWidget::getTrackBorders(const unsigned int & iCollectionID)
{
  MeshContainer::MultiIndexContainerCollectionIDIterator it0, it1;

  boost::tuples::tie(it0, it1) =
    m_MeshContainer->m_Container.get< CollectionID >().equal_range(iCollectionID);

  std::pair< unsigned int, unsigned int >
  minBorder( 0, std::numeric_limits< unsigned int >::max() );

  std::pair< unsigned int, unsigned int >
  maxBorder(0, 0);

  unsigned int time = 0;

  while ( it0 != it1 )
    {
    time = it0->TCoord;

    if ( minBorder.second > time )
      {
      minBorder.first = it0->TraceID;
      minBorder.second = time;
      }

    if ( maxBorder.second < time )
      {
      maxBorder.first = it0->TraceID;
      maxBorder.second = time;
      }
    ++it0;
    }

  return std::make_pair(minBorder, maxBorder);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::updateTracksIDs(const unsigned int & iIDToDelete,
                                       const unsigned int & iIDToUpdate)
{
  MeshContainer::MultiIndexContainerCollectionIDIterator it0, it1;

  boost::tuples::tie(it0, it1) =
    m_MeshContainer->m_Container.get< CollectionID >().equal_range(iIDToDelete);

  while ( it0 != it1 )
    {
    unsigned int traceID = it0->TraceID;
    ++it0;
    modifyMeshCollectionID(traceID, iIDToUpdate);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::init()
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
QGoTrackEditingWidget::modifyMeshCollectionID(unsigned int iMeshID, unsigned int iCollectionID)
{
  MeshContainer::MultiIndexContainerTraceIDIterator
                       it = m_MeshContainer->m_Container.get< TraceID >().find(iMeshID);
  ContourMeshStructure tempStructure(*it);

  tempStructure.CollectionID = iCollectionID;
  m_MeshContainer->m_Container.get< TraceID >().replace(it, tempStructure);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::highlightFirstActor(bool iHighlight)
{
  m_FirstMeshActor->GetProperty()->SetSpecular(iHighlight);
  m_FirstMeshActor->GetProperty()->SetAmbient(iHighlight);
  m_SecondClick = iHighlight;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::updateMeshesActors(bool iRealMeshes)
{
  // Go through container and update visibility
  MeshContainer::MultiIndexContainerTraceIDIterator iter, c_end;

  iter = m_MeshContainer->m_Container.get< TraceID >().begin();
  c_end = m_MeshContainer->m_Container.get< TraceID >().end();

  while ( iter != c_end )
    {
    iter->ActorXY->SetVisibility(iRealMeshes);
    iter->ActorXZ->SetVisibility(!iRealMeshes);
    ++iter;
    }

  // update visu
  this->qvtkWidget->update();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkActor *
QGoTrackEditingWidget::computeSphere(unsigned int iTraceID, double *iCenter, double iRadius)
{
  // Sphere
  vtkSmartPointer< vtkSphereSource > sphereSource =
    vtkSmartPointer< vtkSphereSource >::New();
  sphereSource->SetCenter(iCenter);
  sphereSource->SetRadius(iRadius);
  sphereSource->Update();

  // add trace ID into the polydata
  vtkSmartPointer<vtkIntArray> trackIDArray = vtkSmartPointer<vtkIntArray>::New();
  trackIDArray->SetNumberOfComponents(1);
  trackIDArray->SetNumberOfValues(1);
  trackIDArray->SetName("MESH");
  trackIDArray->SetValue(0,iTraceID);

  sphereSource->GetOutput()->GetPointData()->AddArray(trackIDArray);

  vtkSmartPointer< vtkPolyDataMapper > sphereMapper =
    vtkSmartPointer< vtkPolyDataMapper >::New();
  sphereMapper->SetInput( sphereSource->GetOutput() );

  vtkActor *sphereActor = vtkActor::New();
  sphereActor->SetMapper(sphereMapper);
  sphereActor->SetVisibility(false);

  return sphereActor;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::getClosestPoints()
{
  MeshContainer::MultiIndexContainerTraceIDIterator it, it2, end;

  it = m_MeshContainer->m_Container.get< TraceID >().begin();
  end = m_MeshContainer->m_Container.get< TraceID >().end();

  it2 = it;

  while ( it != end )
    {
    it2 = it;
    it2++;
    while ( it2 != end )
      {
      double distance =
        sqrt( vtkMath::Distance2BetweenPoints( it->Nodes->GetCenter(),
                                               it2->Nodes->GetCenter() ) );
      if ( it->TraceID != it2->TraceID )
        {
        m_MinimalDistance = std::min(m_MinimalDistance, distance);
        }
      ++it2;
      }
    it2 = m_MeshContainer->m_Container.get< TraceID >().begin();
    ++it;
    }
}
