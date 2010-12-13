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

#include "vtkProperty.h"

// visu
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkViewImage3DCommand.h"

//-------------------------------------------------------------------------
QGoTrackEditingWidget::
QGoTrackEditingWidget(QWidget *iParent): QDialog(iParent)
{
  this->setupUi(this);

  m_VtkEventQtConnector = vtkEventQtSlotConnect::New();
  m_InteractorStyle3D   = vtkInteractorStyleImage3D::New();

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
      vtkActor* sphereActor = CreateSphereActor( currentMeshPosition );
      actorIDsPair.first = sphereActor;
      actorIDsPair.second.first = true;
      actorIDsPair.second.second = idsPair;

      m_Actor2IDMap.insert(actorIDsPair);

      //-------------------------------
      // Create polyline
      // meshID= -1 for more efficiency
      if( previousMeshID >= 0 )
        {
        vtkActor* polylineActor = CreatePolylineActor(previousMeshPosition, currentMeshPosition);
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
CreateSphereActor( double* iCenter)
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
  aSphere->GetProperty()->SetColor(0,0,1); // sphere color blue

  sphere->Delete();
  map->Delete();

  return aSphere;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkActor*
QGoTrackEditingWidget::
CreatePolylineActor( double* iCenter1, double* iCenter2)
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
    actor->GetProperty()->SetColor(1,1,1); // sphere color white

    return actor;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTrackEditingWidget::
preview()
{
//setup render window, renderer, and interactor
  vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
  this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);

  std::map< vtkActor*, std::pair<bool, std::pair< int,  int> > >::iterator
      actor2IDMapIterator;
  actor2IDMapIterator = m_Actor2IDMap.begin();
  while( actor2IDMapIterator != m_Actor2IDMap.end() )
    {
    renderer->AddActor( actor2IDMapIterator->first );
    ++actor2IDMapIterator;
    }

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
    return;
    }

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
  // second click - actor original color
  // create line
  // update list
    // Is it in the list?
    // to be implemented
    // if we have 2 points - create line in red
    bool isPresent = findInMergeList();
    }

  // Update actor color
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
bool
QGoTrackEditingWidget::
findInMergeList()
{
  /*
  std::list< std::pair< std::pair< int,  int>, std::pair< int,  int> > >::iterator
      cutListIterator = m_MergeList.begin();
  while( cutListIterator != m_MergeList.end() )
    {
    // if we find it in the list
    if(    (*cutListIterator).first == iTrackId
        && (*cutListIterator).second == iMeshID)
      {
      std::cout<< "IDs found - remove it" << std::endl;
      // Remove from list
      m_MergeList.erase(cutListIterator);
      return true;
      }
    ++cutListIterator;
    }
  std::cout<< "IDs NOT found - insert it" << std::endl;
  // Add in list
  std::pair<  int,  int> idsPair;
  idsPair.first = iTrackId;
  idsPair.second = iMeshID;
  m_MergeList.push_back(idsPair);
  return false;*/
}
