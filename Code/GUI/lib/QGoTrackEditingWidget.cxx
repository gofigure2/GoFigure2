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

//-------------------------------------------------------------------------
QGoTrackEditingWidget::
QGoTrackEditingWidget(QWidget *iParent): QDialog(iParent)
{
  this->setupUi(this);

  renderer = vtkSmartPointer<vtkRenderer>::New();

  m_VtkEventQtConnector = vtkEventQtSlotConnect::New();
  m_InteractorStyle3D   = vtkInteractorStyleImage3D::New();

  m_SecondClick = false;

  QObject::connect( this->previewPushButton, SIGNAL( pressed() ),
                    this, SLOT( preview() ) );

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
void
QGoTrackEditingWidget::
setTracks( std::list<Track> iListOfTracks )
{
  m_ListOfTracks = iListOfTracks;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
generateTrackRepresentation()
{
  std::list<Track>::iterator trackListIterator = m_ListOfTracks.begin();

  while( trackListIterator != m_ListOfTracks.end() )
    {
    int trackID = (*trackListIterator).first;
    std::cout << "-------------------------" << std::endl;
    std::cout << "trackID: " << trackID << std::endl;
    int    previousMeshID       = -1;
    double* previousMeshPosition = NULL;
    std::list<Mesh>::iterator meshListIterator = (*trackListIterator).second.begin();

    while( meshListIterator!= (*trackListIterator).second.end())
      {
      int     currentMeshID = (*meshListIterator).first;
      int timePoint = (*meshListIterator).second.first;
      double* currentMeshPosition = (*meshListIterator).second.second;

      std::cout << "meshID: " << currentMeshID << std::endl;

      // Actor/IDs Pair
      std::pair< vtkActor*, std::pair< bool, std::pair< int,  int> > >
          actorIDsPair;

      // IDs Pair
      std::pair<  int,  int> idsPair;
      idsPair.first = trackID;
      idsPair.second = currentMeshID;

      //--------------
      //Create actors
      //--------------
      //-------------------------------
      // Create a sphere
      vtkActor* sphereActor = CreateSphereActor( currentMeshPosition, NULL );
      actorIDsPair.first = sphereActor;
      actorIDsPair.second.first = true;
      actorIDsPair.second.second = idsPair;

      m_Actor2IDMap.insert(actorIDsPair);

      //-------------------------------
      // Create polyline
      // meshID= -1 for more efficiency
      if( previousMeshID >= 0 )
        {
        vtkActor* polylineActor = CreatePolylineActor(previousMeshPosition, currentMeshPosition, NULL, NULL);
        actorIDsPair.first = polylineActor;
        actorIDsPair.second.first = false;
        actorIDsPair.second.second = idsPair;

        m_Actor2IDMap.insert(actorIDsPair);
        }

      //-------------------------------

      previousMeshID = currentMeshID;
      previousMeshPosition = currentMeshPosition;

      ++meshListIterator;
      }

    ++trackListIterator;
    }
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

    //Color coding
    // Setup the colors array
    /*vtkSmartPointer<vtkUnsignedCharArray> colors =
        vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");

    // Add the three colors we have created to the array
    colors->InsertNextTuple3(iColor1[0]*255, iColor1[1]*255, iColor1[2]*255);
    colors->InsertNextTuple3(iColor2[0]*255,iColor2[1]*255, iColor2[2]*255);
    polyData->GetPointData()->SetScalars(colors);*/

    //setup actor and mapper
    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInput(polyData);

    vtkActor* actor = vtkActor::New();
    actor->SetMapper(mapper);

    //actor->GetProperty()->SetColor( const_cast<double*>(iColor) ); // sphere color white

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

/*
  std::map< vtkActor*, std::pair<bool, std::pair< int,  int> > >::iterator
      actor2IDMapIterator;
  actor2IDMapIterator = m_Actor2IDMap.begin();
  while( actor2IDMapIterator != m_Actor2IDMap.end() )
    {
    renderer->AddActor( actor2IDMapIterator->first );
    ++actor2IDMapIterator;
    }
*/

  this->qvtkWidget->GetInteractor()->SetInteractorStyle(m_InteractorStyle3D);
  m_InteractorStyle3D->EnablePickMode();
  //this->qvtkWidget->GetInteractor()->SetRenderWindow(this->RenderWindow);


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
/*
  // Where it is in the list to get the IDs
  std::map< vtkActor*, std::pair< bool, std::pair< int,  int> > >::iterator actor2IDMapIterator;
  actor2IDMapIterator = m_Actor2IDMap.find(m_CurrentActor);
  if (actor2IDMapIterator != m_Actor2IDMap.end() )
    {
    std::cout << "Object found" << std::endl;
    std::cout << "mesh?:" << actor2IDMapIterator->second.first << std::endl;
    std::cout << "track ID: " << actor2IDMapIterator->second.second.first << std::endl;
    std::cout << "time:" << actor2IDMapIterator->second.second.second << std::endl;
    }

  // Update IDs list
  // if connection (ie not a mesh)
  if( !actor2IDMapIterator->second.first )
    {
    // Is it in the list?
    bool isPresent = findInCutList(actor2IDMapIterator->second.second.first,
                                   actor2IDMapIterator->second.second.second);
    if(isPresent)
      {
      std::cout << "UN HIGHLIGHT" <<  std::endl;
      m_CurrentActor->GetProperty()->SetOpacity(1);
      }
    else
      {
      std::cout << "HIGHTLIGHT" << std::endl;
      m_CurrentActor->GetProperty()->SetOpacity(0.3);
      }
    }
  else
    {
  // first click = actor is red
    if( !m_SecondClick )
      {
      m_CurrentActor->GetProperty()->SetLineWidth(5);
      m_FirstActor = m_CurrentActor;

      m_FirstPair.first = actor2IDMapIterator->second.second.first;
      m_FirstPair.second = actor2IDMapIterator->second.second.second;
      m_SecondClick = true;
      }
    else
      {
      m_FirstActor->GetProperty()->SetLineWidth(1);
      m_SecondActor = m_CurrentActor;
      //m_FirstActor->SetMapper(m_CurrentActor->GetMapper());

      std::pair<int, int> secondPair;
      secondPair.first = actor2IDMapIterator->second.second.first;
      secondPair.second = actor2IDMapIterator->second.second.second;
      bool isPresent = findInMergeList(m_FirstPair, secondPair);
      m_SecondClick = false;
      }
    }
    */
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
QGoTrackEditingWidget::
findInCutList(int iTrackId, int iMeshID)
{
  std::list< std::pair< int,  int> >::iterator cutListIterator = m_CutList.begin();
  while( cutListIterator != m_CutList.end() )
    {
    // if we find it in the list
    if(    (*cutListIterator).first == iTrackId
        && (*cutListIterator).second == iMeshID)
      {
      std::cout<< "IDs found - remove it" << std::endl;
      // Remove from list
      m_CutList.erase(cutListIterator);
      return true;
      }
    ++cutListIterator;
    }
  std::cout<< "IDs NOT found - insert it" << std::endl;
  // Add in list
  std::pair<  int,  int> idsPair;
  idsPair.first = iTrackId;
  idsPair.second = iMeshID;
  m_CutList.push_back(idsPair);
  return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool
QGoTrackEditingWidget::
findInMergeList( std::pair< int, int> iFirstPair, std::pair< int, int > iSecondPair )
{

  std::list< std::pair< std::pair< int,  int>, std::pair< int,  int> > >::iterator
      mergeListIterator = m_MergeList.begin();

  while( mergeListIterator != m_MergeList.end() )
    {
    // if we find it in the list
    if(   ( (*mergeListIterator).first.first == iFirstPair.first
         && (*mergeListIterator).first.second == iFirstPair.second
         && (*mergeListIterator).second.first == iSecondPair.first
         && (*mergeListIterator).second.second == iSecondPair.second)
        ||
          ( (*mergeListIterator).first.first == iSecondPair.first
         && (*mergeListIterator).first.second == iSecondPair.second
         && (*mergeListIterator).second.first == iFirstPair.first
         && (*mergeListIterator).second.second == iFirstPair.second) )
      {
      std::cout<< "IDs found - remove it" << std::endl;
      // Remove from list
      m_MergeList.erase(mergeListIterator);

      // Delete Actor!!!
      std::list< std::pair <std::pair< std::pair< int,  int>, std::pair< int,  int>  >, vtkActor* > >::iterator
          mergeListIterator2 = m_MergeListActor.begin();

      while( mergeListIterator2 != m_MergeListActor.end() )
        {
        if( ( (*mergeListIterator2).first.first.first == iFirstPair.first
            && (*mergeListIterator2).first.first.second == iFirstPair.second
            && (*mergeListIterator2).first.second.first == iSecondPair.first
            && (*mergeListIterator2).first.second.second == iSecondPair.second)
            ||
             ( (*mergeListIterator2).first.first.first == iSecondPair.first
            && (*mergeListIterator2).first.first.second == iSecondPair.second
            && (*mergeListIterator2).first.second.first == iFirstPair.first
            && (*mergeListIterator2).first.second.second == iFirstPair.second) )
          {
          renderer->RemoveActor( (*mergeListIterator2).second );
          (*mergeListIterator2).second->Delete();

          return true;
          }
        ++mergeListIterator2;
        }
      }
    ++mergeListIterator;
    }

  std::cout<< "IDs NOT found - insert it" << std::endl;
  // Add in list
  std::pair<  int,  int> idsPair;
  idsPair.first = iFirstPair.first;
  idsPair.second = iFirstPair.second;

  std::pair<  int,  int> idsPair2;
  idsPair2.first = iSecondPair.first;
  idsPair2.second = iSecondPair.second;

  std::pair<  std::pair<  int,  int>,  std::pair<  int,  int> > idFullPair;
  idFullPair.first = idsPair;
  idFullPair.second = idsPair2;

  m_MergeList.push_back(idFullPair);

  // Create and actor too
  double color[3] = {1, 1, 1};
  vtkActor* actor = CreatePolylineActor(m_FirstActor->GetCenter(), m_SecondActor->GetCenter(),
      m_FirstActor->GetProperty()->GetColor(), m_SecondActor->GetProperty()->GetColor());

  renderer->AddActor(actor);
  this->qvtkWidget->GetRenderWindow()->Render();

  std::pair<std::pair< std::pair< int,  int>, std::pair< int,  int> >, vtkActor* > actorConnection;
  actorConnection.first = idFullPair;
  actorConnection.second =  actor;
  m_MergeListActor.push_back( actorConnection );

  return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
setTracks2( std::map< unsigned int, std::pair< const double* , vtkPolyData*> > iTrack )
{
  m_ListOfTracks2 = iTrack;
  generateTrackRepresentation2();
  preview();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
generateTrackRepresentation2()
{
  std::map< unsigned int, std::pair< const double* , vtkPolyData* > >::iterator 
      trackListIterator =
      m_ListOfTracks2.begin();

  while( trackListIterator != m_ListOfTracks2.end() )
    {
    unsigned int trackID = trackListIterator->first;
    std::cout << "-------------------------" << std::endl;
    std::cout << "trackID: " << trackID << std::endl;
    int    previousMeshID       = -1;
    double* previousMeshPosition = new double[3];

    vtkPolyData* trackPolyData = ( trackListIterator->second ).second;

    for(vtkIdType i = 0; i< trackPolyData->GetNumberOfPoints(); ++i)
      {
      double *currentMeshPosition = trackPolyData->GetPoint( i );
      vtkIntArray* array = dynamic_cast<vtkIntArray*>(
          trackPolyData->GetPointData()->GetArray("TemporalInformation") );
      int currentMeshID = array->GetValue(i);

      // IDS
      std::pair< int, int> idsPair;
      idsPair.first = trackID;
      // time point
      idsPair.second = currentMeshID;

      // Actor
      vtkActor* sphereActor = CreateSphereActor( currentMeshPosition, (*trackListIterator).second.first );

      // Actor/IDs Pair
      std::pair< vtkActor*, std::pair< bool, std::pair< int,  int> > >
          actorIDsPair;

      actorIDsPair.first = sphereActor;
      actorIDsPair.second.first = true;
      actorIDsPair.second.second = idsPair;

      m_Actor2IDMap.insert(actorIDsPair);

      if( previousMeshID >= 0 )
        {
        vtkActor* polylineActor = CreatePolylineActor(previousMeshPosition, currentMeshPosition,
            (*trackListIterator).second.first, (*trackListIterator).second.first);
        actorIDsPair.first = polylineActor;
        actorIDsPair.second.first = false;
        actorIDsPair.second.second = idsPair;

        m_Actor2IDMap.insert(actorIDsPair);
        }

      previousMeshID = currentMeshID;
      previousMeshPosition[0] = currentMeshPosition[0];
      previousMeshPosition[1] = currentMeshPosition[1];
      previousMeshPosition[2] = currentMeshPosition[2];
      }
    delete[] previousMeshPosition;
    ++trackListIterator;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
setMeshIDs( std::list< std::list< unsigned int > > iListoFfMeshes)
{
  //m_MeshList = iListoFfMeshes;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
setTracksPolyData( std::list< vtkPolyData* > iListOfTracks)
{
  //m_TrackList = iListOfTracks;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void
QGoTrackEditingWidget::
initializeVisualization()
{*/
  /*
  // the 2 lists must have the same size
  if( m_TrackList.size() != m_MeshList.size() )
    {
    std::cout << "Track list and mesh list have different sizes" << std::endl;
    return;
    }

  // Iterators
  std::list< vtkPolyData* >::iterator trackIterator = m_TrackList.begin();
  std::list< std::list< unsigned int > >::iterator meshIterator = m_MeshList.begin();

  while( trackIterator != m_TrackList.end() )
    {
    // Create actors and update lists
    vtkPolyData* trackPolyData = (*trackIterator);
    int numberOfPointsInPolyData = trackPolyData->GetNumberOfPoints();
    vtkIntArray* tempArray = dynamic_cast<vtkIntArray*>(
        trackPolyData->GetPointData()->GetArray("TemporalInformation") );

    // the 2 lists must have the same size
    if( (*meshIterator.size()) != numberOfPointsInPolyData )
      {
      std::cout << "PolyData and mesh list have different number of points" << std::endl;
      return;
      }

    for(vtkIdType i = 0; i < numberOfPointsInPolyData; ++i)
      {
      double *currentMeshPosition = trackPolyData->GetPoint( i );
      int currentMeshTime = tempArray->GetValue(i);

      vtkActor* sphereActor = CreateSphereActor( currentMeshPosition, NULL );


      }
    ++meshIterator;
    ++trackIterator;
    }
    */
/*}*/
//-------------------------------------------------------------------------

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

  // For each track, create the actors
  while( trackIDsIt != listOfTrackIDs.end() )
    {
    std::list<unsigned int> listOfMeshIDs =
        m_MeshContainer->GetAllTraceIDsGivenCollectionID( (*trackIDsIt) );
    std::list<unsigned int>::iterator listOfMeshIDsIt = listOfMeshIDs.begin();

    // Create the meshes actor
    // update the container
    while( listOfMeshIDsIt != listOfMeshIDs.end() )
      {
      m_MeshContainer->ResetCurrentElement();
      m_MeshContainer->UpdateCurrentElementFromExistingOne( (*listOfMeshIDsIt) );

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

      ++listOfMeshIDsIt;
      }

    // Go through map to create polylines
    std::map<unsigned int, unsigned int>::iterator polyLIt = m_Time2MeshID.begin();
    if( polyLIt == m_Time2MeshID.end() )
      {
      std::cout << "List is empty" << std::endl;
      return;
      }

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

    ++trackIDsIt;
    }
}
//-------------------------------------------------------------------------
// Go through all container and creates actors
//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
cutTrack( vtkActor* )
{
  //std::map< vtkActor* , int >::iterator it = m_Line2MeshID.
}
