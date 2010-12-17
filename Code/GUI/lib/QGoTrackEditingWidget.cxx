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

#include <limits>

//-------------------------------------------------------------------------
QGoTrackEditingWidget::
QGoTrackEditingWidget(QWidget *iParent): QDialog(iParent)
{
  this->setupUi(this);

  renderer = vtkSmartPointer<vtkRenderer>::New();

  m_VtkEventQtConnector = vtkEventQtSlotConnect::New();
  m_InteractorStyle3D   = vtkInteractorStyleImage3D::New();

  m_NumberOfTracks = 0;
  m_FirstRender = true;
  m_MaxTrackID = std::numeric_limits<unsigned int>::min();

  m_SecondClick = false;

  m_VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject * >( m_InteractorStyle3D ),
    vtkViewImage3DCommand::MeshPickingEvent,
    this, SLOT( UpdateCurrentActorSelection(vtkObject *) ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTrackEditingWidget::
~QGoTrackEditingWidget()
{
  // DELETE ACTORS!!!
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkActor*
QGoTrackEditingWidget::
CreateSphereActor( double* iCenter, const double* iColor)
{
// create sphere geometry
  vtkSphereSource *sphere = vtkSphereSource::New();
  sphere->SetRadius(1.0);
  sphere->SetThetaResolution(18);
  sphere->SetPhiResolution(18);
  sphere->SetCenter( iCenter );

  // map to graphics library
  vtkPolyDataMapper *map = vtkPolyDataMapper::New();
  map->SetInput(sphere->GetOutput());

  // actor coordinates geometry, properties, transformation
  vtkActor *aSphere = vtkActor::New();
  aSphere->SetMapper(map);
  aSphere->GetProperty()->SetColor( const_cast<double*>(iColor) ); // sphere color blue

  sphere->Delete();
  map->Delete();

  return aSphere;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkActor*
QGoTrackEditingWidget::
CreatePolylineActor( double* iCenter1, double* iCenter2,
    const double* iColor1, const double* iColor2)
{
  //create a vtkPoints object and storevtkRenderWindow the points in it
    vtkSmartPointer<vtkPoints> points =
      vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(iCenter1);
    points->InsertNextPoint(iCenter2);

    vtkSmartPointer<vtkPolyLine> polyLine =
        vtkSmartPointer<vtkPolyLine>::New();
    polyLine->GetPointIds()->SetNumberOfIds(2);
    for( int i = 0; i < 2; i++)
      {
      polyLine->GetPointIds()->SetId(i,i);
      }

    //Create a cell array to store the lines in and add the lines to it
    vtkSmartPointer<vtkCellArray> cells =
        vtkSmartPointer<vtkCellArray>::New();
    cells->InsertNextCell(polyLine);

    //Create a polydata to store everything in
    vtkSmartPointer<vtkPolyData> polyData =
        vtkSmartPointer<vtkPolyData>::New();

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
  if(m_CurrentActor == NULL)
    {
    std::cout<< "No ActorPicked" << std::endl;
    return;
    }

  std::map< vtkActor* , int >::iterator polyToMeshID =
      m_Line2MeshID.find(m_CurrentActor);

  if(polyToMeshID != m_Line2MeshID.end())
    {
    std::cout<< "Actor is a line" << std::endl;

    if( m_CurrentActor->GetProperty()->GetOpacity() == 1 )
      {
      m_CurrentActor->GetProperty()->SetOpacity(0.3);
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
      m_SecondActor = m_CurrentActor;
      mergeTrack( m_FirstActor, m_SecondActor);
      m_SecondClick = false;
      }
    else
      {
      m_FirstActor = m_CurrentActor;
      m_SecondClick = true;
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
  std::list<unsigned int> listOfTrackIDs = m_MeshContainer->GetAllCollectionIDs();
  std::list<unsigned int>::iterator trackIDsIt = listOfTrackIDs.begin();

  if(m_FirstRender)
    {  // For each track, create the actors
    while( trackIDsIt != listOfTrackIDs.end() )
      {
      std::cout<< "collection ID: " << (*trackIDsIt) << std::endl;
      m_TrackIDsMapping[m_NumberOfTracks] = (*trackIDsIt);

      if( (*trackIDsIt) > m_MaxTrackID)
        {
        m_MaxTrackID = (*trackIDsIt);
        }

      std::list<unsigned int> listOfMeshIDs =
              m_MeshContainer->GetAllTraceIDsGivenCollectionID( (*trackIDsIt) );
      std::list<unsigned int>::iterator listOfMeshIDsIt = listOfMeshIDs.begin();

      // Create the meshes actor
      // update the container
      while( listOfMeshIDsIt != listOfMeshIDs.end() )
        {
        m_MeshContainer->ResetCurrentElement();
        m_MeshContainer->UpdateCurrentElementFromExistingOne( (*listOfMeshIDsIt) );
        m_MeshContainer->SetCurrentElementCollectionID( m_NumberOfTracks );
        m_MeshContainer->InsertCurrentElement();
        ++listOfMeshIDsIt;
        }

      ++m_NumberOfTracks;
      ++trackIDsIt;
      }
    trackIDsIt = listOfTrackIDs.begin();
    m_FirstRender = false;
    }

  ////////////////////////////////////////////////////////////////////////////////


  // For each track, create the actors
  for( int i = 0; i < m_NumberOfTracks ; ++i )
    {
    std::cout<< "collection ID: " << i << std::endl;
    std::map<unsigned int, unsigned int> m_Time2MeshID;
    std::list<unsigned int> listOfMeshIDs =
        m_MeshContainer->GetAllTraceIDsGivenCollectionID( i );
    std::list<unsigned int>::iterator listOfMeshIDsIt = listOfMeshIDs.begin();

    // Create the meshes actor
    // update the container
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

      // Add actor to visu
      renderer->AddActor(actor);

      /*
       * \todo Find a better solution - Nicolas
       */
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

      // Fill map - to construct lines
      m_Time2MeshID[time] = (*listOfMeshIDsIt);

      // Insert Element
      m_MeshContainer->InsertCurrentElement();

      m_Actor2MeshID[actor] = (*listOfMeshIDsIt);

      ++listOfMeshIDsIt;
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
        vtkActor* polyLine = CreatePolylineActor(firstActor->GetCenter(),
                                                 secondActor->GetCenter());

         /*
         * \todo should color be hard coded? hoew to define it? - Nicolas
         */
          double color[3] = {1, 1, 1};
        polyLine->GetProperty()->SetColor(color);
        // Add actor to visu
        renderer->AddActor( polyLine );

        // key is actor
        m_Line2MeshID[polyLine] = polyLIt->second;
        //m_MeshID2Neigbours[] =

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
// Go through all container and creates actors
//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
cutTrack( vtkActor* iActor)
{
  std::map< vtkActor* , int >::iterator it = m_Line2MeshID.find( iActor );
  int timePoint;

  // Find the mesh ID
  if( it != m_Line2MeshID.end() )
    {
    unsigned int collectionID = m_MeshContainer->GetCollectionIDOfGivenTrace( it->second );

    std::cout << "Cut collection: " << collectionID << std::endl;

    std::list<unsigned int> listOfMeshIDs =
        m_MeshContainer->GetAllTraceIDsGivenCollectionID( collectionID );
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
      if( time < tLimit)
        {
        // Collection ID
        m_MeshContainer->SetCurrentElementCollectionID( m_NumberOfTracks );
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
  std::map< vtkActor* , int >::iterator it = m_Line2MeshID.begin();
  while( it != m_Line2MeshID.end() )
    {
    renderer->RemoveActor( it->first );
    ++it;
    }
}
//-------------------------------------------------------------------------
// ONLY CALLED AT THE END
//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
mapContainerIDs2RealIDs()
{
  //left: current ID, right: real ID
  std::map< unsigned int, unsigned int>::iterator iter =
      m_TrackIDsMapping.begin();

  while( iter != m_TrackIDsMapping.end())
    {
    if( iter->second > m_NumberOfTracks )
      {
      std::list<unsigned int> listOfMeshIDs =
          m_MeshContainer->GetAllTraceIDsGivenCollectionID( iter->first );
      std::list<unsigned int>::iterator iterator = listOfMeshIDs.begin();

      while( iterator != listOfMeshIDs.end())
        {
        m_MeshContainer->ResetCurrentElement();
        m_MeshContainer->UpdateCurrentElementFromExistingOne( (*iterator) );
        m_MeshContainer->SetCurrentElementCollectionID( iter->second );
        m_MeshContainer->InsertCurrentElement();
        m_MeshContainer->DeleteElement((*iterator));

        ++iterator;
        }
      }
    else
      {
      // increase max track ID
      ++m_MaxTrackID;

      // move first track
      std::list<unsigned int> listOfMeshIDs =
          m_MeshContainer->GetAllTraceIDsGivenCollectionID( iter->second );
      std::list<unsigned int>::iterator iterator = listOfMeshIDs.begin();

      while( iterator != listOfMeshIDs.end())
        {
        m_MeshContainer->ResetCurrentElement();
        m_MeshContainer->UpdateCurrentElementFromExistingOne( (*iterator) );
        m_MeshContainer->SetCurrentElementCollectionID( m_MaxTrackID );
        m_MeshContainer->InsertCurrentElement();
        m_MeshContainer->DeleteElement((*iterator));

        ++iterator;
        }

      // move secoind track
      std::list<unsigned int> listOfMeshIDs2 =
          m_MeshContainer->GetAllTraceIDsGivenCollectionID( iter->first );
      std::list<unsigned int>::iterator iterator2 = listOfMeshIDs2.begin();

      while( iterator != listOfMeshIDs2.end())
        {
        m_MeshContainer->ResetCurrentElement();
        m_MeshContainer->UpdateCurrentElementFromExistingOne( (*iterator2) );
        m_MeshContainer->SetCurrentElementCollectionID( iter->second );
        m_MeshContainer->InsertCurrentElement();
        m_MeshContainer->DeleteElement((*iterator2));

        ++iterator;
        }
      }
    }
}
//-------------------------------------------------------------------------
// ONLY CALLED AT THE END
//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
mergeTrack( vtkActor* iFirstActor, vtkActor* iSecondActor)
{
  // Get mesh IDs
  unsigned int firstMesh;
  std::map< vtkActor*, unsigned int >::iterator iter = m_Actor2MeshID.find(iFirstActor);
  if( iter != m_Actor2MeshID.end() )
    {
    firstMesh = iter->second;
    }
  else
    {
    std::cout << "First actor ID not found" << std::endl;
    return;
    }

  unsigned int secondMesh;
  iter = m_Actor2MeshID.find(iSecondActor);
  if(iter != m_Actor2MeshID.end() )
    {
    secondMesh = iter->second;
    }
  else
    {
    std::cout << "Second actor ID not found" << std::endl;
    return;
    }

  //Check if actors are border of track
  std::pair< std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> >
      border1 = isOnBorder(firstMesh);

  std::cout << "Border for mesh1" << std::endl;
  std::cout << "mesh1: " << firstMesh << std::endl;
  std::cout << "low limit ID: " << border1.first.first << " time "
                             << border1.first.second << std::endl;
  std::cout << "high limit ID: " << border1.second.first << " time "
                              << border1.second.second << std::endl;

  std::pair< std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> >
      border2 = isOnBorder(secondMesh);

  std::cout << "Border for mesh2" << std::endl;
  std::cout << "mesh2: " << secondMesh << std::endl;
  std::cout << "low limit ID: " << border2.first.first << " time "
                             << border2.first.second << std::endl;
  std::cout << "high limit ID: " << border2.second.first << " time "
                              << border2.second.second << std::endl;
  // Check for overlap
  if(    ( border2.first.first < border1.second.first )
      && ( border1.first.first < border2.second.first ) )
    {
    std::cout << " Tracks are overlaping" << std::endl;
    return;
    }

  //Get Highest time to know which meshes we should update
  unsigned int trackToUpdate = 0;
  unsigned int trackToDelete = 0;
  if( border1.first.first < border2.first.first)
    {
    trackToDelete = firstMesh;
    trackToUpdate = secondMesh;
    }
  else
    {
    trackToUpdate = firstMesh;
    trackToDelete = secondMesh;
    }

  std::cout<< " Mesh to update: " << trackToUpdate << std::endl;
  std::cout<< " Mesh to delete: " << trackToDelete << std::endl;

  // Change the ID of the track by the other one
  updateTracksIDs( trackToDelete, trackToUpdate);

  // update visu
  removeLineActors();
  initializeVisualization();

}
//-------------------------------------------------------------------------
// ONLY CALLED AT THE END
//-------------------------------------------------------------------------
std::pair< std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> >
QGoTrackEditingWidget::
isOnBorder( unsigned int iMeshID)
{
  // Get the collectionID
  unsigned int collectionID = m_MeshContainer->GetCollectionIDOfGivenTrace( iMeshID );

  std::cout << "Merge collection: " << collectionID << std::endl;

  // list of meshes of the collection
  std::list<unsigned int> listOfMeshIDs =
      m_MeshContainer->GetAllTraceIDsGivenCollectionID( collectionID );
  std::list<unsigned int>::iterator iterator = listOfMeshIDs.begin();

  std::pair< std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> >
    borders;

  std::pair<unsigned int, unsigned int> minBorder;
  minBorder.first = 0;
  minBorder.second = std::numeric_limits<unsigned int>::max();

  std::pair<unsigned int, unsigned int> maxBorder;
  maxBorder.first = 0;
  maxBorder.second = std::numeric_limits<unsigned int>::min();

  // Go through all meshes
  while( iterator != listOfMeshIDs.end() )
    {
    m_MeshContainer->ResetCurrentElement();
    m_MeshContainer->UpdateCurrentElementFromExistingOne( (*iterator) );

    unsigned int time = m_MeshContainer->GetCurrentElementTimePoint();
    m_MeshContainer->InsertCurrentElement();

    if( minBorder.second > time )
      {
      minBorder.first = *iterator;
      minBorder.second = time;
      }

    if( maxBorder.second < time )
      {
      maxBorder.first = *iterator;
      maxBorder.second = time;
      }

    ++iterator;
    }

  borders.first = minBorder;
  borders.second = maxBorder;

  return borders;
}

void
QGoTrackEditingWidget::
updateTracksIDs( unsigned int iIDToDelete, unsigned int iIDToUpdate)
{
  // Get track to update ID
  unsigned int collectionID =
      m_MeshContainer->GetCollectionIDOfGivenTrace( iIDToUpdate );

  // Update track to delete IDs with update ID
  unsigned int collectionID2 = m_MeshContainer->GetCollectionIDOfGivenTrace( iIDToDelete );

  std::list<unsigned int> listOfMeshIDs =
      m_MeshContainer->GetAllTraceIDsGivenCollectionID( collectionID2 );
  std::list<unsigned int>::iterator iterator = listOfMeshIDs.begin();

  while( iterator != listOfMeshIDs.end() )
    {
    m_MeshContainer->ResetCurrentElement();
    m_MeshContainer->UpdateCurrentElementFromExistingOne( (*iterator) );
    m_MeshContainer->SetCurrentElementCollectionID( collectionID );
    m_MeshContainer->InsertCurrentElement();
    ++iterator;
    }
}
